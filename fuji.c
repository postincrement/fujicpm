

#ifdef __CPM__

#include <cpm.h>

#endif // __CPM__

#include <stdio.h>
#include <stdint.h>

///////////////////////////////////////

void SendSPI(uint8_t * ptr, uint8_t len)
{}

///////////////////////////////////////

/* 
   Reference
   https://www.atarimax.com/jindroush.atari.org/asio.html
 */

#define SIO_HEADER_LEN        5
#define SIO_PAYLOAD_LEN       128

#define MAX_SIO_PACKET_LEN    (SIO_HEADER_LEN+SIO_PAYLOAD_LEN)

static uint8_t sioPacket[MAX_SIO_PACKET_LEN];

static uint8_t * sioPayload = sioPacket + SIO_HEADER_LEN;
static uint16_t sioPayloadLen = 0;

static uint8_t deviceId = 0x70;
static uint8_t command  = 0x71;

void DumpSPI()
{
  uint16_t len = SIO_HEADER_LEN + sioPayloadLen;
  uint8_t * ptr = sioPacket;
  for (uint16_t i = 0; i < len; ++i)
    printf(" %02x", *ptr++);
  printf("\n");
}


// Calculate 8-bit checksum
uint8_t sio_checksum(uint8_t *buf, unsigned short len)
{
  unsigned int chk = 0;
  for (int i = 0; i < len; i++)
    chk = ((chk + buf[i]) >> 8) + ((chk + buf[i]) & 0xff);
  return chk;
}


void sio_send()
{ 
  uint8_t * ptr = sioPacket;
  uint8_t checksum;

  *ptr = deviceId;
  checksum += *ptr++;

  *ptr = command;
  checksum += *ptr++;

  *ptr = 0x00;
  checksum += *ptr++;

  *ptr = 0x00;
  checksum += *ptr++;

  *ptr++ = checksum;

  printf("xmit:");
  DumpSPI();
}

///////////////////////////////////////

int main(int argc, char *argv[])
{
  // parse command line
  if (argc == 1) {
    printf("usage: fuji -v command\n");
    return 0;
  }

  sio_send();

  return 0;
}
