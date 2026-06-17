#include "../header/ping.h"

uint16_t	checksum(void *data, int len)
{
	uint16_t *buffer = data;
	uint32_t sum = 0;

	for (; len > 1; len -= 2)
		sum += *buffer++;

	if (len == 1)
		sum += *(uint8_t *)buffer;

	while (sum >> 16)
		sum = (sum & 0xFFFF) + (sum >> 16);

	return (uint16_t)~sum;
}

void	build_packet(t_ping *ping, char *buffer)
{

	struct icmphdr	*hdr = (struct icmphdr *)buffer;
	char			*payload = buffer + sizeof(struct icmphdr);
	int				total_size = sizeof(struct icmphdr) + ping->packet_size;

	for (int i = 0; i < ping->packet_size; i++)
		payload[i] = i;

	if (ping->packet_size >= (int)sizeof(struct timeval))
	{
		struct timeval *tv = (struct timeval *)payload;
		gettimeofday(tv, NULL);
	}

	hdr->type = ICMP_ECHO;
	hdr->code = 0;
	hdr->un.echo.id = htons(ping->id);
	hdr->un.echo.sequence = htons(ping->seq);
	hdr->checksum = 0;
	hdr->checksum = checksum(buffer, total_size);
}
