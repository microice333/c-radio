#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "err.h"

#define BSIZE         1024
#define REPEAT_COUNT  30

struct __attribute__((__packed__)) Audio {
  union {
    uint64_t session_id;
    char session_c[8];
  };
  union {
      uint64_t first_byte_num;
      char byte_num_char[8];
    };
    char audio_data[512];

    ~Audio() {
      delete[] audio_data;
    }
};

int main (int argc, char *argv[]) {
  /* argumenty wywołania programu */
  char *multicast_dotted_address;
  in_port_t local_port;

  /* zmienne i struktury opisujące gniazda */
  Audio data_read;
  int sock;
  struct sockaddr_in local_address;
  struct ip_mreq ip_mreq;

  /* zmienne obsługujące komunikację */
  char buffer[512];
  (void) memset(buffer, 0, sizeof(buffer));
  ssize_t rcv_len;
  ssize_t len, prev_len, remains;
  int i;

  // data_read.audio_data = buffer;

  /* parsowanie argumentów programu */
  if (argc != 3)
    fatal("Usage: %s multicast_dotted_address local_port\n", argv[0]);
  multicast_dotted_address = argv[1];
  local_port = (in_port_t)atoi(argv[2]);

  /* otworzenie gniazda */
  sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (sock < 0)
    syserr("socket");

  /* podpięcie się do grupy rozsyłania (ang. multicast) */
  ip_mreq.imr_interface.s_addr = htonl(INADDR_ANY);
  if (inet_aton(multicast_dotted_address, &ip_mreq.imr_multiaddr) == 0)
    syserr("inet_aton");
  if (setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (void*)&ip_mreq, sizeof ip_mreq) < 0)
    syserr("setsockopt");

  /* podpięcie się pod lokalny adres i port */
  local_address.sin_family = AF_INET;
  local_address.sin_addr.s_addr = htonl(INADDR_ANY);
  local_address.sin_port = htons(local_port);
  if (bind(sock, (struct sockaddr *)&local_address, sizeof local_address) < 0)
    syserr("bind");

  uint64_t prev = 0;
  while(1) {
    (void) memset(data_read.audio_data, 0, sizeof(data_read.audio_data));
    // printf("%ld %ld %s\n", data_read.session_id, data_read.first_byte_num, data_read.audio_data);
    recv(sock, &data_read, sizeof(data_read), 0);
    // rcv_len = read(sock, buffer, sizeof buffer);
    // printf("%ld %ld\n", data_read.session_id, data_read.first_byte_num);
      uint64_t xd = be64toh(data_read.first_byte_num);
      // fprintf(stderr, "%ld\n", xd);
      if (xd == 1024 || xd == 512 || xd == 1536 || xd == 5632 || xd == 3584|| xd == 2048 || xd == 4096 || xd == 8192) {
        fprintf(stderr, "dostalem %ld\n", xd);
      }

    // if (data_read.first_byte_num - prev != 512) {
      // fprintf(stderr, "brak %ld a dostalem %ld\n", prev, data_read.first_byte_num);
    // }

    prev = data_read.first_byte_num;
    write(1, data_read.audio_data, sizeof data_read.audio_data);
    // printf("%s\n", data_read.audio_data);
    // if (rcv_len < 0)
    //   syserr("read");
    // else {
    //   printf("read %zd bytes: %.*s\n", rcv_len, (int)rcv_len, buffer);
    // }
  }
  /* w taki sposób można odpiąć się od grupy rozsyłania */
  if (setsockopt(sock, IPPROTO_IP, IP_DROP_MEMBERSHIP, (void*)&ip_mreq, sizeof ip_mreq) < 0)
        syserr("setsockopt");

  /* koniec */
  close(sock);
  exit(EXIT_SUCCESS);
}
