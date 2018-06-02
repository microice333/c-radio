#include "parameters.h"
#include "err.h"

using namespace std;

void check_and_set_params(int argc, char *argv[], string *mcast_addr, 
	int *data_port, int *ctrl_port, int *psize, int *fsize, int *rtime, 
	string *nazwa) {
	po::options_description desc("Allowed options");

	desc.add_options()
	("MCAST_ADDR,a", po::value<string>(mcast_addr), "Multicast address")
	("DATA_PORT,P", po::value<int>(data_port)->default_value(DEFAULT_DATA_PORT), "UDP data port")
	("CTRL_PORT,C", po::value<int>(ctrl_port)->default_value(DEFAULT_CTRL_PORT), "UDP retransmission port")
	("PSIZE,p", po::value<int>(psize)->default_value(DEFAULT_PSIZE), "Packet size")
	("FSIZE,f", po::value<int>(fsize)->default_value(DEFAULT_FSIZE), "FIFO size")
	("RTIME,R", po::value<int>(rtime)->default_value(DEFAULT_RTIME), "Retransmission time")
	("NAZWA,n", po::value<string>(nazwa)->default_value(DEAFULT_NAZWA), "Sender name");

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