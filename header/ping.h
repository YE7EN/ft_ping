#ifndef PING_H
#define PING_H

#include <stdbool.h>
#include <netinet/in.h>
#include <netinet/ip_icmp.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <float.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <arpa/inet.h>



typedef struct s_ping
{
	bool	verbose;
	bool	numeric;
	bool	has_count;
	bool	has_timeout;

	int		count;
	int		packet_size;
	int		timeout;
	int		linger;
	int		id;
	int		sockfd;
	int		seq;
	int		transmitted;
	int		received;

	double	rtt_min;
	double	rtt_max;
	double	rtt_sum;
	double	rtt_sum_sq;
	
	char	*dest;
	char	display_name[256]; // [256] max len hostname.

	struct sockaddr_in 	dest_addr;

	struct timeval		start_time;
}	t_ping;


// MAIN.C
int main(int argc, char **argv);


// INIT.C
void	init(t_ping *ping);
void	print_help(void);
void	parse_args(int argc, char **argv, t_ping *ping);
void	init_hostname(t_ping *ping);
void	init_socket(t_ping *ping);


// PACKET.C
uint16_t	checksum(void *data, int len);
void		build_packet(t_ping *ping, char *buffer);

// SEND.C
void	send_packet(t_ping *ping, char *buffer);

// RECEPTION.C
void	receive_packet(t_ping *ping);
void	decode_packet(t_ping *ping, char *recv_buffer, ssize_t recv_len);

#endif