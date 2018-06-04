#ifndef SERVER_H
#define SERVER_H

#include <string>
#include <map>
#include <cstdint>
#include <set>
#include <mutex>

#include "const.h"

class Server {
	int sock;
	int ctrl_sock;
	std::string MCAST_ADDR;
	int DATA_PORT;
	int CTRL_PORT;
	int PSIZE; 
	int FSIZE; 
	int RTIME;
	std::string NAZWA;

	std::mutex fifo_mutex;
	std::mutex retransmission_packages_mutex;

	std::map<uint64_t, Audio> fifo_map;
	std::set<uint64_t> retransmission_packages;

	char* audio_to_char(char result[], Audio *audio);
	void read_and_send();
	void init_send_socket();
	void init_ctrl_socket();
	void control();
	void collect_packages(std::string statement);
	void retransmission();
	uint64_t htonll(uint64_t value);
public:
	void check_and_set_params(int argc, char *argv[]);
	void run();
};

#endif