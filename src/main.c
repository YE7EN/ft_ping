#include "../header/ping.h"


int main(int argc, char **argv)
{

	t_ping ping = {0};

	init(&ping);

	parse_args(argc, argv, &ping);

	init_hostname(&ping);

	init_socket(&ping);

	print_header();


	char buffer[sizeof(struct icmphdr) + ping.packet_size];
	send_packet(&ping, buffer);


	receive_packet(&ping);

	return (0);
}
