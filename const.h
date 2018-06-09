#ifndef PARAMETERS_H
#define PARAMETERS_H

const int DEFAULT_DATA_PORT = 20000 + (370768 % 10000);
const int DEFAULT_CTRL_PORT = 30000 + (370768 % 10000);
const int DEFAULT_PSIZE = 512;
const int DEFAULT_FSIZE = 131072;
const int DEFAULT_RTIME = 250;
const std::string DEAFULT_NAZWA = "Nienazwany Nadajnik";

const int TTL_VALUE = 4;
const int BSIZE = 1024;
const int SESSION_AND_BYTE_SIZE = 16;

const std::string LOOKUP = "ZERO_SEVEN_COME_IN\n";
const std::string REPLY = "BOREWICZ_HERE ";
const std::string REXMIT = "LOUDER_PLEASE ";

#endif