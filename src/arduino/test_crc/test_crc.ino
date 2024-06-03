void setup() 
{
  Serial.begin(115200);
  Serial.print("CRC('hello')=");
  const char *msg = "hello";
  Serial.println(getCRC((uint8_t *)msg, strlen(msg), 0));
  msg = "a bit longer string but not too much long anyway";
  Serial.print("CRC(longer string)=");
  Serial.println(getCRC((uint8_t *)msg, strlen(msg), 0));
  msg = "a quite longer string much much longer you know a quite longer string much much longer you know a quite longer string much much longer you know a quite longer string much much longer you know a quite longer string much much longer you know a quite longer string much much longer you know a quite longer string much much longer you know a quite longer string much much longer you know a quite longer string much much longer you know ";
  Serial.print("CRC(long)=");
  Serial.println(getCRC((uint8_t *)msg, strlen(msg), 0));
}

void loop() 
{
}

uint8_t getCRC(uint8_t message[], uint32_t length, uint8_t previous_crc)
{
  static const unsigned char CRC7_POLY = 0x91;
  uint32_t i;
  uint8_t j, crc = previous_crc;
 
  for (i = 0; i < length; i++)
  {
    crc ^= message[i];
    for (j = 0; j < 8; j++)
    {
      if (crc & 1)
        crc ^= CRC7_POLY;
      crc >>= 1;
    }
  }
  return crc;
}