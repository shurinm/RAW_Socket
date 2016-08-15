#include "../header/raw_socket.h"

#define SRC_IP      "192.168.2.43"  // IP источника
#define DST_IP      "192.168.2.73"  // IP назначения
#define SRC_PORT    1345            // Порт источника
#define DST_PORT    1336            // Порт назначения
#define MSG         "Hello, World!" // Посылаемое сообщение

// Создаем равсокет
int main()
{
	struct sockaddr_ll  addr;   // Данные назначения для физического уровня
    u_char *data;               // Данные для следующего уровня
    int psize;                  // Размер данных
    // Создаем RAW сокет
	int raw_soccet= socket(AF_PACKET, SOCK_RAW, IPPROTO_RAW);
	if (raw_soccet==-1) 
	{
		perror("raw_soccet ERROR");
		exit(-1);
	}
	 // Обворачиваем заголовком транспортного уровня
    data = udp_filling(SRC_IP, DST_IP, SRC_PORT, DST_PORT, MSG, strlen(MSG));
    psize = strlen(MSG) + sizeof(struct udp_header);

    // Обворачиваем заголовком сетевого уровня
    data = ip_filling(SRC_IP, DST_IP, data, psize);
    psize += sizeof(struct ip_header);

    // Обворачиваем заголовком канального уровня
    u_char smac[] = {0x38, 0x63, 0xbb, 0xae, 0xcc, 0xbc};   // MAC источника
    u_char dmac[] = {0x48, 0x5b, 0x39, 0x7a, 0xcf, 0x7f};   // MAC назначения
    data = eth_filling(dmac, smac, data, psize);
    psize += sizeof(struct eth_header);

    //Отправляем
    memset(&addr, 0, sizeof(addr));
    addr.sll_family = AF_PACKET;
    addr.sll_ifindex = 2;
    addr.sll_halen = 6;
    memcpy(addr.sll_addr, dmac, 6);

    //printf("INDEX: %d", if_nametoindex("enp3s0"));
    if (sendto(raw_soccet, data, psize, 0, (struct sockaddr *)&addr, sizeof(addr)) == -1)
    {
        perror("sendto");
        close(raw_soccet);
        exit(-1);
    }

    return 0;
}
