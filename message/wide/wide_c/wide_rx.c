/*****************************************************************************
 * Includes
 *****************************************************************************/
#include "wide.h"
#include "wide_rx.h"
#include "crc16_ccitt.h"

/*****************************************************************************
 * Types
 *****************************************************************************/
typedef enum
{
    IDLE,
    HEADER_DLE,
    PAYLOAD,
    VALIDATE
} rx_state_t;

/*****************************************************************************
 * Macros
 *****************************************************************************/
#define COMBINE_TO_U16(_HIGH, _LOW) ( ( 0xFF00 & (uint16_t)(_HIGH) << 8 ) | ( 0xFF & (_LOW) ) )

/*****************************************************************************
 * Local Static Variables
 *****************************************************************************/
static bool           dle_received;
static rx_state_t     currentState;
static wide_packet_t  currentPacket;
static uint8_t        rxBuffer[WIDE_MAX_PACKET_SIZE];
static uint16_t       rxBufferPosition;

/*****************************************************************************
 * Static function declarations
 *****************************************************************************/
static void idle(const char c);
static void header_dle(const char c);
static void payload(const char c);
static bool validate(void);
static bool crcCheck(const uint8_t* buffer, const uint16_t size);

/*****************************************************************************
 * Externally Visible Functions
 *****************************************************************************/
void wide_rx_init(void)
{
    dle_received = false;
    currentState = IDLE;

    memset(&currentPacket, 0x00, sizeof(currentPacket));

    rxBufferPosition = 0;
}

bool wide_rx_work(wide_packet_t* ret_pkt, char c)
{
    bool ret_val = false;

    switch(currentState)
    {
        case IDLE:
            idle(c);
            break;
        case HEADER_DLE:
            header_dle(c);
            break;
        case PAYLOAD:
            payload(c);
            break;
    }

    if(VALIDATE == currentState)
    {
        if(validate())
        {
            ret_val = true;
            memcpy(ret_pkt, &currentPacket, sizeof(wide_packet_t));
        }

        currentState = IDLE;
    }

    return ret_val;
}

/*****************************************************************************
 * Static functions
 *****************************************************************************/
static void idle(const char c)
{
    if(c == DLE)
    {
        currentState = HEADER_DLE;
    }
    else
    {
        currentState = IDLE;
    }
    dle_received = false; // Only used for repeated DLEs
}

static void header_dle(const char c)
{
    if(c == STX)
    {
        currentState = PAYLOAD;
        rxBufferPosition = 0;
    }
    else
    {
        // No Sync
        currentState = IDLE;
    }

}

static void payload(const char c)
{
    if(DLE == c)
    {
        // If already received, process this one. If not, record this one.
        dle_received = !dle_received;

        if(dle_received)
        {
            // DLE used as signal character
            return;
        }
    }

    if(dle_received)
    {
        if(c != ETX)
        {
            // Something bad happened
            currentState = IDLE;
        }
        else
        {
            // End of packet!
            currentState = VALIDATE;
        }
        return;
    }

    rxBuffer[rxBufferPosition] = c;
    rxBufferPosition++;

    // Size ran out
    if(rxBufferPosition == sizeof(rxBuffer))
    {
        currentState = IDLE;
    }

}

static bool validate(void)
{
    if(rxBufferPosition <= WIDE_MAX_HEADER_SIZE)
    {
        return false;
    }

    if(!crcCheck(rxBuffer, rxBufferPosition))
    {
        return false;
    }

    currentPacket.messageType = COMBINE_TO_U16(rxBuffer[MESSAGE_TYPE_HB_OFFSET],
                                               rxBuffer[MESSAGE_TYPE_LB_OFFSET]);
    currentPacket.messageId   = COMBINE_TO_U16(rxBuffer[MESSAGE_ID_HB_OFFSET],
                                               rxBuffer[MESSAGE_ID_LB_OFFSET]);

    currentPacket.crc         = COMBINE_TO_U16(rxBuffer[rxBufferPosition + MESSAGE_CRC_HB_NEG_OFFSET],
                                               rxBuffer[rxBufferPosition + MESSAGE_CRC_LB_NEG_OFFSET]);

    currentPacket.size = rxBufferPosition - WIDE_MAX_HEADER_SIZE;

    currentPacket.data = &rxBuffer[MESSAGE_DATA_START_OFFSET];

    return true;
}

static bool crcCheck(const uint8_t* buffer, const uint16_t size)
{
    uint16_t i;
    uint16_t crc = 0;


    for(i = 0; i < size; i++)
    {
        crc = crcCCITT( crc, buffer[i] );
    }

    return crc == 0;
}