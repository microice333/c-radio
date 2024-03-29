#include <stdlib.h>
#include <cstdio>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <cstdint>
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>
#include <vector>
#include <ctime>
#include <thread>
#include <iostream>

#include "err.h"
#include "server.h"
#include "const.h"

namespace po = boost::program_options;
using namespace std;

void Audio::to_char_array(char result[], int size) {
	for (int i = 0; i < 8; i++) {
		result[i] = session_id_c[i];
	}

	for (int i = 8; i < SESSION_AND_BYTE_SIZE; i++) {
		result[i] = first_byte_num_c[i - 8];
	}

	for (int i = SESSION_AND_BYTE_SIZE; i < size; i++) {
   		result[i] = audio_data[i - SESSION_AND_BYTE_SIZE];
	}
}

void Server::init_send_socket() {
	int optval;
	struct sockaddr_in remote_address;

	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock < 0)
		syserr("socket");

	optval = 1;
	if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (void*)&optval, sizeof optval) < 0)
		syserr("setsockopt broadcast");

	optval = TTL_VALUE;
	if (setsockopt(sock, IPPROTO_IP, IP_MULTICAST_TTL, (void*)&optval, sizeof optval) < 0)
		syserr("setsockopt multicast ttl");

	remote_address.sin_family = AF_INET;
	remote_address.sin_port = htons((in_port_t)DATA_PORT);
	if (inet_aton(MCAST_ADDR.c_str(), &remote_address.sin_addr) == 0)
		syserr("inet_aton");
	if (connect(sock, (struct sockaddr *)&remote_address, sizeof remote_address) < 0)
		syserr("connect");
}

void Server::init_ctrl_socket() {
	struct sockaddr_in local_address;

	ctrl_sock = socket(AF_INET, SOCK_DGRAM, 0);
  	
	if (ctrl_sock < 0)
		syserr("socket");
  	
	local_address.sin_family = AF_INET;
	local_address.sin_addr.s_addr = htonl(INADDR_ANY);
	local_address.sin_port = htons((in_port_t)CTRL_PORT);
  	
	if (bind(ctrl_sock, (struct sockaddr *)&local_address, sizeof local_address) < 0)
		syserr("bind");
}

void Server::check_and_set_params(int argc, char *argv[]) {
	po::options_description desc("Allowed options");

	desc.add_options()
	("MCAST_ADDR,a", po::value<string>(&MCAST_ADDR), "Multicast address")
	("DATA_PORT,P", po::value<int>(&DATA_PORT)->default_value(DEFAULT_DATA_PORT), "UDP data port")
	("CTRL_PORT,C", po::value<int>(&CTRL_PORT)->default_value(DEFAULT_CTRL_PORT), "UDP retransmission port")
	("PSIZE,p", po::value<int>(&PSIZE)->default_value(DEFAULT_PSIZE), "Packet size")
	("FSIZE,f", po::value<int>(&FSIZE)->default_value(DEFAULT_FSIZE), "FIFO size")
	("RTIME,R", po::value<int>(&RTIME)->default_value(DEFAULT_RTIME), "Retransmission time")
	("NAZWA,n", po::value<string>(&NAZWA)->default_value(DEAFULT_NAZWA), "Sender name");

	po::variables_map vm;
	
	try {
		po::store(po::parse_command_line(argc, argv, desc), vm);
	} catch (exception e) {
		fatal("Uknown parameter!");
	}
	
	po::notify(vm);

	if (!vm.count("MCAST_ADDR")) {
		fatal("Unset MCAST_ADDR!");
	}
}

void Server::read_and_send() {
	init_send_socket();

	ssize_t len, snd_len;
	uint64_t byte_num = 0;
	char buffer[PSIZE];
	Audio data_to_send;
	time_t session_id = time(0);
	uint fifo_size = FSIZE / PSIZE;
	char result[PSIZE + SESSION_AND_BYTE_SIZE];
	
	bzero(buffer, sizeof buffer);

	while (cin.read(buffer, PSIZE)) {
		data_to_send.session_id =  htobe64(session_id);
		data_to_send.first_byte_num = htobe64(byte_num);
		data_to_send.audio_data = new char[PSIZE];

		for (int i = 0; i < PSIZE; i++) {
			data_to_send.audio_data[i] = buffer[i];
		}

		bzero(result, sizeof result);
		data_to_send.to_char_array(result, PSIZE + SESSION_AND_BYTE_SIZE);

		byte_num += PSIZE;
		len = sizeof(result);

		sock_mutex.lock();

		fifo_map[byte_num] = data_to_send;
	    		
		if (fifo_map.size() > fifo_size) 
			fifo_map.erase(fifo_map.begin());
	    	
		snd_len = write(sock, result, sizeof result);
		sock_mutex.unlock();

		if (snd_len != len) 
			syserr("partial / failed write");
	 	  
		bzero(buffer, sizeof buffer);
	}

	close(sock);
}

void Server::run() {
	thread th_read = thread([=] { read_and_send(); });
	thread th_control = thread([=] { control(); });
	thread th_retransmission = thread ([=] { retransmission(); });

	th_control.detach();
	th_retransmission.detach();
	th_read.join();
}
	

void Server::control() {
	struct sockaddr_in client_address;
	socklen_t snda_len, rcva_len;
	char buffer[BSIZE];
	ssize_t snd_len, len;

 	bzero(buffer, sizeof buffer);

	init_ctrl_socket();

	while(true) {
		rcva_len = (socklen_t) sizeof(client_address);
		snda_len = (socklen_t) sizeof(client_address);
		len = recvfrom(ctrl_sock, buffer, sizeof(buffer), 0,
			(struct sockaddr *) &client_address, &rcva_len);

		if (len < 0) {
			syserr("error on datagram from client socket");
		} else {
			string statement = string(buffer);

			if (statement == LOOKUP) {
				string reply = REPLY + MCAST_ADDR + " " + to_string(DATA_PORT) + " " + NAZWA + "\n";
        		
				snd_len = sendto(ctrl_sock, reply.c_str(), (size_t) reply.size(), 0,
					(struct sockaddr *) &client_address, snda_len);
        		
				if (snd_len != (ssize_t) reply.size())
					syserr("write");
			} else if (statement.find(REXMIT) == 0) {
				collect_packages(statement.substr(REXMIT.size()));
			}
		}
	}

	close(ctrl_sock);
}

void Server::collect_packages(string statement) {
	vector<string> results;

	boost::split(results, statement, [](char c){return c == ',';});
	
	retransmission_packages_mutex.lock();

	for (string byte_num : results) {
		uint64_t value;
		
		istringstream iss(byte_num);
		iss >> value;
		
		retransmission_packages.insert(value);
	}

	retransmission_packages_mutex.unlock();
}

void Server::retransmission() {
	ssize_t len;

	while (true) {
		this_thread::sleep_for(std::chrono::milliseconds(RTIME));
		retransmission_packages_mutex.lock();
		
		set<uint64_t> retransmission_packages_copy(retransmission_packages);
		retransmission_packages.clear();
		
		retransmission_packages_mutex.unlock();

		sock_mutex.lock();
		
		for (auto byte_num : retransmission_packages_copy) {
			if (fifo_map.find(byte_num) != fifo_map.end()) {
				char result[PSIZE + SESSION_AND_BYTE_SIZE];
				bzero(result, sizeof result);
				fifo_map[byte_num].to_char_array(result, PSIZE + SESSION_AND_BYTE_SIZE);

				len = sizeof(result);
    			
				if (write(sock, result, sizeof result) != len) 
					syserr("partial / failed write");	
			}
		}

		sock_mutex.unlock();
	}
}