#ifndef WIDE_TX_H
#define WIDE_TX_H

#include "wide.h"
#include "stdint.h"
#include "stdbool.h"
#include <string.h>

// Function pointers
typedef void     (*sendChar)(char);

void wide_tx_init(sendChar oneCharTx);
void wide_tx_send(wide_packet_t* txPacket);

#endif