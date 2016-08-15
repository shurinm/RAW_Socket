#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <linux/if_packet.h>

// структура IP зваголовка
struct ip_header{
#if __BYTE_ORDER == __LITTLE_ENDIAN
    unsigned int ip_hl:4;           // длига заголовка
    unsigned int ip_v:4;            // версия
#endif
#if __BYTE_ORDER == __BIG_ENDIAN
    unsigned int ip_v:4;            // версия
    unsigned int ip_hl:4;           // длина заголовка
#endif
    u_int8_t ip_tos;                // тип обслуживания
    u_short ip_len;                 // длина сегмента
    u_short ip_id;                  // идентификатор
    u_short ip_off;                 // смещение фрагмента
#define IP_RF 0x8000                /* reserved fragment flag */
#define IP_DF 0x4000                /* dont fragment flag */
#define IP_MF 0x2000                /* more fragments flag */
#define IP_OFFMASK 0x1fff           /* mask for fragmenting bits */
    u_int8_t ip_ttl;                //время жизни
    u_int8_t ip_p;                  // nип инкапсулированного протокола
    u_short ip_sum;                 // контрольная сумма
    int ip_src, ip_dst;             // адрес отправителя, адрес получателя
};

// структура UDP заголовка
struct udp_header {
    u_short uh_sport;       // номер порта отправителя
    u_short uh_dport;       // номер порта получателя
    u_short length;         // длина
    u_short chsum;          // контрольная сумма
};

// стрктура псевдозаголовка  для UDP и TCP пакетов
struct psevdo_header {
    int     sours;  // адрес отправителя
    int     dest;   // адрес получателя
    u_char  zero;   // нулевое поле
    u_char  proto;  // тип протакола
    u_short length; // длина пакета
};

struct eth_header
{
    u_char  dmac[6];    // MAC адрес получателя    
    u_char  smac[6];    // MAC адрес отправителя
    short   proto;      // Тип инкапсулированного протокала
};

// модуль RAW_IP.c
u_char *ip_filling(u_char *ip_src, u_char *ip_dst, u_char *data, int dlen);
u_short chsum_ip (u_char *ip_header); 

// модуль RAW_ETH.c
u_char *eth_filling(u_char *dmac, u_char *smac, u_char *data, int dlen);

// модуль RAW_UDP.c
u_short udp_checksum(u_char *hdr_udp, u_char *hdr_psd);
u_char *udp_filling(u_char *sours, u_char *dest, short uh_sport, short uh_dport, u_char *data, int dlen);

