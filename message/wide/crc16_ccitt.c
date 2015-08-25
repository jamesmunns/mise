// Courtesy of:
//   http://www.nongnu.org/avr-libc/user-manual/group__util__crc.html
uint16_t crcCCITT(uint16_t crc, const uint8_t data)
{
    uint8_t i;

    crc = crc ^ ((uint16_t)data << 8);
    for (i=0; i<8; i++)
    {
        if (crc & 0x8000)
        {
            crc = (crc << 1) ^ 0x1021;
        }
        else
        {
            crc <<= 1;
        }
    }
    return crc;
}