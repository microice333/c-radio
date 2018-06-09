#ifndef SERVER_H
#define SERVER_H

#include <string>
#include <map>
#include <cstdint>
#include <set>
#include <mutex>

struct Audio {
	union {
		uint64_t session_id;
		char session_id_c[8];
	};
	union {
  		uint64_t first_byte_num;
  		char first_byte_num_c[8];
  	};
  	char *audio_data;

  	void to_char_array(char result[], int size);

  	~Audio() {
  		delete[] audio_data;
  	}
};

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

	std::mutex sock_mutex;
	std::mutex retransmission_packages_mutex;

	std::map<uint64_t, Audio> fifo_map;
	std::set<uint64_t> retransmission_packages;

	void read_and_send();
	void init_send_socket();
	void init_ctrl_socket();
	void control();
	void collect_packages(std::string statement);
	void retransmission();
public:
	void check_and_set_params(int argc, char *argv[]);
	void run();
};

#endif