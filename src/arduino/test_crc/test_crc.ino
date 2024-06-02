void setup() 
{
  Serial.begin(115200);
  Serial.print("CRC('hello')=");
  const char *msg = "hello";
  Serial.println(getCRC((uint8_t *)msg, strlen(msg), 0));
}

void loop() 
{
}

uint8_t getCRC(uint8_t message[], uint8_t length, uint8_t previous_crc)
{
  static const uint8_t CRC7_POLY = 0x91;
  uint8_t i, j, crc = previous_crc;

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