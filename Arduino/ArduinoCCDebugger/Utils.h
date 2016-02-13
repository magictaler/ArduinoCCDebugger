/*
  Copyright (c) 2016 Dmitry Pakhomenko.
  dmitryp@magictale.com
  http://magictale.com
 
  This example code is in the public domain.
 */
 
#ifndef Utils_h
#define Utils_h


static void serial_print_p(const char str[])
{
  char c;
  if(!str) return;
  while((c = pgm_read_byte(str++)))
    Serial.write(c);
}

static void serial_println_p(const char str[])
{
  serial_print_p(str);
  Serial.println();
}

static uint8_t bcdToBin(uint8_t value)
{
  uint8_t hex = 10;
  hex *= (value & 0xF0) >> 4;
  hex += (value & 0x0F);
  return hex; 
}

static uint8_t binToBcd(uint8_t value)
{
  uint8_t MSD = 0;
  while (value >= 10)
  {
    value -= 10;
    MSD += 0x10;
  }
  return MSD + value;
}

static int powint(int x, int y)
{
  int val=x;
  for(int z=0;z<=y;z++)
  {
    if (z==0)
      val=1;
    else
    val=val*x;
  }
  return val;
}

#endif


