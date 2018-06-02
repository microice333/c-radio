#include "err.h"
#include "server.h"

using namespace std;

int main (int argc, char *argv[]) {
	Server server;
	
	server.check_and_set_params(argc, argv);
	server.run();
  	
  	exit(EXIT_SUCCESS);
}
