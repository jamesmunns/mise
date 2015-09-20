/*****************************************************************************
 * Includes
 *****************************************************************************/
#include "wide.h"
#include "wide_tx.h"
#include "crc16_ccitt.h"

/*****************************************************************************
 * Types
 *****************************************************************************/
typedef enum
{
    TX_BATCH_MODE, // Not implemented
    TX_SINGLE_MODE,
    TX_DISABLED_MODE
} tx_send_mode_t;

/*****************************************************************************
 * Local Static Variables
 *****************************************************************************/
static tx_send_mode_t currentMode;
static sendChar       singleTxFunc;

/*****************************************************************************
 * Static function declarations
 *****************************************************************************/
static void single_send(const wide_packet_t* txPacket);
static void sendChunk(uint16_t* crc, const uint8_t* buffer, const uint16_t size);

/*****************************************************************************
 * Externally Visible Functions
 *****************************************************************************/
void wide_tx_init(sendChar oneCharTx)
{
    singleTxFunc = NULL;

    if(oneCharTx)
    {
        currentMode  = TX_SINGLE_MODE;
        singleTxFunc = oneCharTx;
    }
    else
    {
        currentMode = TX_DISABLED_MODE;
    }
}

void wide_tx_send(wide_packet_t* txPacket)
{
    switch(currentMode)
    {
        case TX_SINGLE_MODE:
            single_send(txPacket);
            break;

        case TX_DISABLED_MODE:
        default:
            break;
    }
}

/*****************************************************************************
 * Static functions
 *****************************************************************************/
static void single_send(const wide_packet_t* txPacket)
{
    uint8_t  header[4];
    uint16_t crc = 0;

    if(singleTxFunc == NULL)
    {
        return;
    }

    // Start message
    (singleTxFunc)(DLE);
    (singleTxFunc)(STX);

    // Assemble header
    header[0] = (0xFF) & (txPacket->messageType >> 8);
    header[1] = (0xFF) & (txPacket->messageType     );

    header[2] = (0xFF) & (txPacket->messageId >> 8);
    header[3] = (0xFF) & (txPacket->messageId     );

    // Send Header
    sendChunk(&crc, header, sizeof(header));

    // Send Payload
    sendChunk(&crc, txPacket->data, txPacket->size);

    // Send CRC and end of transmission, reuse header
    header[0] = (0xFF) & (crc >> 8);
    header[1] = (0xFF) & (crc     );

    sendChunk(NULL, header, 2);

    // End the message
    (singleTxFunc)(DLE);
    (singleTxFunc)(ETX);
}

static void sendChunk(uint16_t* crc, const uint8_t* buffer, const uint16_t size)
{
    uint16_t bytesSent;
    for(bytesSent = 0; bytesSent < size; bytesSent++)
    {
        // Does this character need to be escaped?
        if(buffer[bytesSent] == DLE)
        {
            (singleTxFunc)(DLE);
        }

        if(crc != NULL)
        {
            // Update CRC
            *crc = crcCCITT(*crc, buffer[bytesSent]);
        }

        // Send byte
        (singleTxFunc)(buffer[bytesSent]);
    }
}