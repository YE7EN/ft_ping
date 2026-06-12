#include "../header/ping.h"

void	init(t_ping *ping)
{
	ping->packet_size = 56;

	ping->rtt_min = DBL_MAX;
	ping->linger = 10;

}


void	print_help(void)
{
	printf("Usage: ft_ping [OPTION...] HOST ...\n");
	printf("Send ICMP ECHO_REQUEST packets to network hosts.\n");
	printf("\n");
	printf("  -c NUMBER                  stop after sending NUMBER packets\n");
	printf("  -n                         do not resolve host addresses\n");
	printf("  -s NUMBER                  send NUMBER data octets\n");
	printf("  -v                         verbose output\n");
	printf("  -w N                       stop after N seconds\n");
	printf("  -W N                       number of seconds to wait for response\n");
	printf("  -?                         give this help list\n");
}


void	parse_args(int argc, char **argv, t_ping *ping)
{
	int opt;
	opterr = 0;

	while ((opt = getopt(argc, argv, "vnc?:s:w:W:")) != -1)
	{
		switch(opt) {
			case 'v':
				ping->verbose = true;
				// printf("ping->verbose = %d\n\n", ping->verbose);
				break;
			case 'n':
				ping->numeric = true;
				// printf("ping->numeric = %d\n\n", ping->numeric);
				break;
			case 'c':
				ping->count = atoi(optarg);
				ping->has_count = true;
				// printf("ping->count = %d\nping->hascount = %d\n\n",ping->count, ping->has_count);
				break;
			case 's':
				ping->packet_size = atoi(optarg);
				// printf("ping->packet_size = %d\n\n",ping->packet_size);
				break;
			case 'w':
				ping->timeout = atoi(optarg);
				ping->has_timeout = true;
				// printf("ping->timeout = %d\nping->has_timeout = %d\n\n", ping->timeout, ping->has_timeout);
				break;
			case 'W':
				ping->linger = atoi(optarg);
				// printf("ping->linger %d\n\n",ping->linger);
				break;
			case '?' :
				if (optopt == '?')
				{
					print_help();
					exit(0);
				}
				fprintf(stderr, "ft_ping: invalid option -- '%c'\n", optopt);
				fprintf(stderr, "Try 'ping --help' or 'ping --usage' for more information.\n");
				exit(64);
		}
	}
	if (argv[optind])
	{
		ping->dest = argv[optind];
		// printf("\nping->dest = %s\n", ping->dest);
	}
	else
	{
		fprintf(stderr,"ft_ping: missing host operand\nTry 'ping --help' or 'ping --usage' for more information.\n");
		exit(64);
	}
		
}


int main(int argc, char **argv)
{

	t_ping ping = {0};

	init(&ping);

	parse_args(argc, argv, &ping);

	return (0);
}