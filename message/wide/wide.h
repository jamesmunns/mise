#ifndef WIDE_H
#define WIDE_H

#include "stdint.h"
#include "stdbool.h"
#include <string.h>

#define WIDE_MAX_PACKET_SIZE ( 512 )
#define WIDE_MAX_HEADER_SIZE (   6 )

typedef struct
{
    // Sent on wire
    uint16_t    messageType;
    uint16_t    messageId;
    uint8_t*    data;
    uint16_t    crc;

    //Meta
    uint16_t    size;
    uint32_t    timestamp;

} wide_packet_t;

typedef enum
{
    MESSAGE_TYPE_HB_OFFSET    = 0,
    MESSAGE_TYPE_LB_OFFSET    = 1,
    MESSAGE_ID_HB_OFFSET      = 2,
    MESSAGE_ID_LB_OFFSET      = 3,

    MESSAGE_DATA_START_OFFSET = 4,

    MESSAGE_CRC_HB_NEG_OFFSET = -2,
    MESSAGE_CRC_LB_NEG_OFFSET = -1
} wide_packet_offset_t;

#endif