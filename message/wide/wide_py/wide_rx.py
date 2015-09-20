DLE = 0x10
STX = 0x02
ETX = 0x03
MAX_PACKET_SIZE = 512
WIDE_MAX_HEADER_SIZE = 6


class WidePacket(object):
    def __init__(self, messageType, messageID, crc, payload):
        self.messageType = messageType
        self.messageID   = messageID
        self.crc         = crc
        self.payload     = payload

class WideRx(object):
    def __init__(self, serial = None):
        self._serial  = serial
        self.resetState()

    def resetState(self):
        self._stateFn     = self._idleFn
        self._packetBuf   = []
        self._dleReceived = False

    def _idleFn(self, char_in):
        if( char_in == DLE ):
            self._stateFn = self._headerFn

        return None

    def _headerFn(self, char_in):
        if( char_in == STX ):
            self._stateFn = self._soakPayload
        else:
            self.resetState()

        return None

    def _soakPayload(self, char_in):
        if( char_in == DLE ):
            self._dleReceived = not self._dleReceived

            if(self._dleReceived):
                return None

        if(self._dleReceived):

            retVal = None

            if( char_in == ETX ):
                retVal = self._validatePayload()

            # We are either done, or had an error (non-ETX after DLE)
            self.resetState()
            return retVal

        self._packetBuf.append(char_in)

        if len(self._packetBuf) >= MAX_PACKET_SIZE:
            self.resetState()

        return

    def _validatePayload(self):
        if len(self._packetBuf) <= WIDE_MAX_HEADER_SIZE:
            return None

        if not crcCheck(self._packetBuf):
            return None

        # TODO use struct unpack
        def combineToU16(hb,lb):
            temp  = ( hb << 8 ) & 0xFF00
            temp |= ( lb      ) & 0x00FF
            return temp

        # TODO: un-magic this
        messageType = combineToU16( self._packetBuf[0],
                                    self._packetBuf[1] )
        messageID   = combineToU16( self._packetBuf[2],
                                    self._packetBuf[3] )
        crc         = combineToU16( self._packetBuf[-2],
                                    self._packetBuf[-1] )

        payload = self._packetBuf[4:-2]

        return WidePacket( messageType = messageType,
                           messageID   = messageID,
                           crc         = crc,
                           payload     = payload )

    def workSingleStep(self, char_in):
        return self._stateFn(char_in)

    def workAuto(self):
        assert self._serial != None
        assert self._serial.isOpen()

        while True:
            char_in = self._serial.read(1)

            if char_in == '':
                # Idle period occured
                self.resetState()
                return None

            result = self._stateFn()

            if result != None:
                return result

# TODO: Replace with a standard CRC16 CCITT
def crcCheck(pktBuffer):

    crc = 0

    for x in pktBuffer:
        crc = crc ^ ((x << 8) & 0xFF00)

        for _ in xrange(8):
            if (crc & 0x8000):
                crc = (crc << 1) ^ 0x1021
            else:
                crc <<= 1

    return (crc & 0xFFFF) == 0


