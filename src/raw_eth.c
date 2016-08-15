#include "../header/raw_socket.h"

// Формируем ETH заголовок
u_char *eth_filling(u_char *dmac, u_char *smac, u_char *data, int dlen)
{
	struct eth_header  *eth_hdr;	// Указатель на заголовок ethernet
	u_char 	*frame;					// Сформированный фрейм
	int flength;					// Длина фрейма

    flength = dlen + sizeof(struct eth_header);
    // Выделяем память под фрейм, копируя данные
    frame = calloc(sizeof(u_char), flength);
    memcpy(frame + sizeof(struct eth_header), data, dlen);

    //Заполняем заголовок
    eth_hdr = (struct eth_header *)frame;
    memcpy(eth_hdr->smac, smac, 6);
    memcpy(eth_hdr->dmac, dmac, 6);
    eth_hdr->proto = 0x08;

	// Подчищаем старые данные, поскольку выделена новая память
	free(data);
	return frame;
}
