
#include "wide.h"
#include "wide_rx.h"
#include "wide_tx.h"
#include "stdint.h"
#include "stdbool.h"
#include <string.h>
#include <stdio.h>


static uint8_t tx_buffer[1000];
static uint8_t rx_buffer[1000];

static uint16_t tx_pos;
static uint16_t rx_pos;

void fake_tx(char c);

int main( void )
{
    memset(tx_buffer, 0x00, sizeof(tx_buffer));
    memset(rx_buffer, 0x00, sizeof(rx_buffer));

    rx_pos = 0;
    tx_pos = 0;

    uint8_t msg1[] = { 0x01, 0x02, 0x03, 0x04, 0x05 };

    wide_tx_init(&fake_tx);

    wide_packet_t pkt;
    wide_packet_t pkt2;

    pkt.messageId   = 100;
    pkt.messageType = 200;
    pkt.data = msg1;
    pkt.size = 5;

    wide_tx_send(&pkt);

    printf("%d\n",rx_pos);

    wide_rx_init();

    for(uint16_t i = 0; i < rx_pos; i++)
    {
        printf("%02X ", rx_buffer[i]);
        if(wide_rx_work(&pkt2, rx_buffer[i]))
        {
            printf("\nDONE\n");

            printf("\n");

            printf("ID:   %d\n", pkt2.messageId);
            printf("Type: %d\n", pkt2.messageType);
            printf("Size: %d\n", pkt2.size);

            for(uint16_t i = 0; i < pkt2.size; i++)
            {
                printf("%02X ", pkt2.data[i]);
            }

            printf("\n");
        }
    }




}

void fake_tx(char c)
{
    rx_buffer[rx_pos] = c;
    rx_pos++;
}