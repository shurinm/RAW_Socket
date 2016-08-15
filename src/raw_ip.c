#include "../header/raw_socket.h"

// Формируем IP заголовок
u_char *ip_filling(u_char *ip_src, u_char *ip_dst, u_char *data, int dlen)
{
    u_char *packet;                 // Указатель на начало пакета
    int pack_len;                   // Размер пакета
    struct ip_header *hdr_ip;       // Заголовок IP

    pack_len = sizeof(struct ip_header) + dlen;
    // Выделяем память под пакет и записываем передаваемые данные
    packet = calloc(sizeof(u_char), pack_len);
    memcpy(packet + sizeof(struct ip_header), data, dlen);

    // Заполняем заголовок
    hdr_ip = (struct ip_header *)packet;
    hdr_ip->ip_hl = 0x5;
    hdr_ip->ip_v = 0x4;
    hdr_ip->ip_len = htons(pack_len);
    hdr_ip->ip_ttl = 89;
    hdr_ip->ip_p = IPPROTO_UDP;
    hdr_ip->ip_src = inet_addr(ip_src);
    hdr_ip->ip_dst = inet_addr(ip_dst);
    hdr_ip->ip_sum = htons(chsum_ip((u_char *)hdr_ip));

    // Подчищаем старые данные, поскольку выделена новая память
    free(data);
    return packet;
}

// функция для подсчета контрольной суммы IP пакета
u_short chsum_ip (u_char *ip_header) 
{
    int shsum;
    u_short fbyte;
    u_short sbyte;
    int ip_leng;

    shsum = 0;
    ip_leng = sizeof(struct ip_header);

    for (int i = 0; i < ip_leng; i += 2)
    {
        if (i == 10)
            continue;
        fbyte = (ip_header[i]);
        sbyte = (ip_header[i + 1]);
        shsum += (fbyte<<8)|sbyte;
    }
    shsum = shsum + (shsum>>16);
    shsum = ~shsum;
    return shsum&0xFFFF;
}

