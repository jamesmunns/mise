#ifndef WIDE_RX_H
#define WIDE_RX_H

#include "wide.h"
#include "stdint.h"
#include "stdbool.h"
#include <string.h>

void wide_rx_init(void);
bool wide_rx_work(wide_packet_t* ret_pkt, char c);

#endif