#include "../header/ping.h"

void	send_packet(t_ping *ping, char *buffer)
{
	build_packet(ping, buffer);

	ssize_t sent = sendto(ping->sockfd, buffer, sizeof(struct icmphdr) + ping->packet_size,
		0, (struct sockaddr *)&ping->dest_addr, sizeof(ping->dest_addr));
	
	if (sent < 0)
		perror("ft_ping: sendto failure");
	else
		ping->transmitted++;
}
