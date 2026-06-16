#include "../header/ping.h"

void	receive_packet(t_ping *ping)
{
	char recv_buffer[1024];
	ssize_t recv_len = recvfrom(ping->sockfd, recv_buffer, sizeof(recv_buffer), 0, NULL, NULL);
	
	if (recv_len <= 0)
		return ;

	decode_packet(ping, recv_buffer, recv_len);
}

void	decode_packet(t_ping *ping, char *recv_buffer, ssize_t recv_len)
{
	struct iphdr	*ip = (struct iphdr*)recv_buffer;
	int				ip_header_len = ip->ihl * 4;
	struct icmphdr	*icmp = (struct icmphdr *)(recv_buffer + ip_header_len);
	struct timeval	time_recv;
	gettimeofday(&time_recv, NULL);


	int		type = icmp->type;
	int		id = ntohs(icmp->un.echo.id);
	int		seq = ntohs(icmp->un.echo.sequence);
	int		ttl = ip->ttl;

	char	src_ip[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &ip->saddr, src_ip, sizeof(src_ip));
	

	if (type == ICMP_ECHOREPLY && id == ping->id)
	{
		struct timeval	*time_sent = (struct timeval *)(recv_buffer + ip_header_len + sizeof(struct icmphdr));
		double rtt = (time_recv.tv_sec - time_sent->tv_sec) * 1000.0
					+ (time_recv.tv_usec - time_sent->tv_usec) /1000.0;
		printf("%ld bytes from %s: icmp_seq=%d ttl=%d time=%.3f ms\n",
			recv_len - ip_header_len, src_ip, seq, ttl, rtt );
	}
}
