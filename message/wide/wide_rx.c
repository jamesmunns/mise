//DLE
//SOH
//ID
//ID
//DLE
//STX
//...
//CKSM
//CKSM
//DLE
//ETX

//IDLE



/////////

#define DLE (0x10)

typedef enum
{
    IDLE,
    HEADER_DLE,
    PAYLOAD,
    VALIDATE
} rx_state_t;

typedef struct
{
    uint16_t    messageType;
    uint16_t    messageId;
    uint8_t     data[512];
    uint16_t    crc;

    //Meta
    uint16_t    size;
    uint32_t    timestamp;

} rx_packet_t;

static bool         dle_received;
static rx_state_t   currentState;
static rx_packet_t  currentPacket;
static uint8_t      rxBuffer;
static uint16_t     rxBufferPosition;


void init(void)
{
    dle_received = false;

}

void work(void)
{
    char* c;

    if(getChar(c))
    {
        switch(currentState)
        {
            case IDLE:
                idle(*c);
                break;
            case HEADER_DLE:
                header_dle(*c);
                break;
            case PAYLOAD:
                payload(*c);
                break;
        }
    }

    if(VALIDATE == currentState)
    {
        if(validate())
        {
            // dispatch message
        }
        else
        {
            // Log error?
        }

        currentState = IDLE;
    }

    
}

// State functions
void idle(char c)
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

void header_dle(char c)
{
    if(c == STX)
    {
        currentState = PAYLOAD;
        rxBufferPosition = 0;
    }
    else
    {
        // No Sync
        currentState = IDLE
    }
    
}

void payload(char c)
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

#define COMBINE_TO_U16(_HIGH, _LOW) ( ( 0xFF00 & (uin16_t)_HIGH << 8 ) | ( 0xFF & _LOW ) )

bool validate(void)
{
    const uint16_t header_size = ( sizeof(currentPacket.messageType)
                                 + sizeof(currentPacket.messageId)
                                 + sizeof(currentPacket.crc) );

    if(rxBufferPosition <= header_size)
    {
        return false;
    }

    if(false) // if(crc(rxBuffer. rxBufferPosition))
    {
        return false;
    }

    currentPacket.messageType = COMBINE_TO_U16(rxBuffer[0],                  rxBuffer[1]                 );
    currentPacket.messageId   = COMBINE_TO_U16(rxBuffer[2],                  rxBuffer[3]                 );
    currentPacket.crc         = COMBINE_TO_U16(rxBuffer[rxBufferPosition-2], rxBuffer[rxBufferPosition-1]);

    currentPacket.size = rxBufferPosition - header_size;

    memcpy(currentPacket.data, &rxBuffer[4], currentPacket.size);

    return true;
}
