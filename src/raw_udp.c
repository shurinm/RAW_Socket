#include "../header/raw_socket.h"

//  Формирует UDP заголовок 
u_char *udp_filling(u_char *sours, u_char *dest, short uh_sport, short uh_dport, u_char *data, int dlen)
{
    struct psevdo_header hdr_psd;   // Псевдозаголовок
    struct udp_header *hdr_udp;         // Заголовок UDP
    u_char *packet;                 // Указатель на начало заголовка
    int pack_len;                   // Длина пакета 
    
    // Формируем псевдозаголовок (Необходим для подсчета контрольной суммы)
    pack_len = sizeof(struct udp_header) + dlen;
    hdr_psd.sours = inet_addr(sours);
    hdr_psd.dest = inet_addr(dest);
    hdr_psd.zero = 0;
    hdr_psd.proto = IPPROTO_UDP;
    hdr_psd.length = htons(pack_len);

    // Выделяем память под пакет и записываем передаваемые данные
    packet = calloc(sizeof(u_char), pack_len);
    memcpy(packet + sizeof(struct udp_header), data, dlen);

    // Заполняем заголовок
    hdr_udp = (struct udp_header *)packet;
    hdr_udp->uh_sport = htons(uh_sport);
    hdr_udp->uh_dport = htons(uh_dport);
    hdr_udp->length = htons(pack_len);
    hdr_udp->chsum = htons(udp_checksum((u_char *)hdr_udp, (u_char *)&hdr_psd));

    return packet;
}


// Выполняет подсчет контрольной суммы UDP. 
u_short udp_checksum(u_char *hdr_udp, u_char *hdr_psd)
{
    u_short fbyte;      // Первый байт 16ти битного блока (Для разворота)
    u_short sbyte;      // Второй байт 16ти битного блока (Для разворота)
    int chsum;          // Контрольная сумма
    u_short length;     // Длина пакета 

    // Считаем контрольную сумму псевдозаголовка, разворачивая байты
    chsum = 0;
    for (int i = 0; i < 12; i += 2){
        fbyte = hdr_psd[i];
        sbyte = hdr_psd[i + 1];
        chsum += (fbyte<<8)|sbyte;
    }

    // Считаем контрольную сумму самого пакета. Если количество байт нечетно, то сразу правильно учтем последний байт и уменьшим length до четности
    length = ntohs(((struct psevdo_header *)hdr_psd)->length);
    if (length%2 == 1){
        fbyte = hdr_udp[length - 1];
        sbyte = 0;
        chsum += (fbyte<<8)|sbyte;
        length--;
    }

    // Подсчет всего остального
    for (int i = 0; i < length; i += 2){
         if (i == 6)
            continue;
        fbyte = hdr_udp[i];
        sbyte = hdr_udp[i + 1];
        chsum += (fbyte<<8)|sbyte;
    }

    // Приводим к необходимому виду
    chsum = chsum + (chsum>>16);
    return (u_short)~chsum;
}
