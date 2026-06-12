#ifndef PING_H
#define PING_H

#include <stdbool.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <float.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

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

#endif