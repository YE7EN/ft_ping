#include "../header/ping.h"

volatile sig_atomic_t g_stop = 0;

void	handle_signal(int signal)
{
	(void)signal;
	g_stop = 1;
}

void	loop(t_ping *ping)
{
	char buffer[sizeof(struct icmphdr) + ping->packet_size];
	
	while (!g_stop)
	{

		send_packet(ping, buffer);
		receive_packet(ping);
		ping->seq++;
		
		if(ping->has_count && ping->transmitted >= ping->count)
			break;
		
		sleep(1);
	}
	print_stats(ping);
}

int main(int argc, char **argv)
{

	t_ping ping = {0};

	init(&ping);

	parse_args(argc, argv, &ping);

	init_hostname(&ping);

	init_socket(&ping);

	signal(SIGINT, handle_signal);

	print_header(&ping);

	loop(&ping);


	return (0);
}
