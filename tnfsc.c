


#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>


/* 
   Reference
    https://github.com/FujiNetWIFI/fujinet-platformio/blob/master/lib/TNFSlib/tnfs-protocol.md 
*/

#define TNFSD_PORT	  16384	  /* UDP port to send to */
#define MAXMSGSZ	    532	    /* maximum size of a TNFS message */
#define TNFS_HEADERSZ	4	      /* minimum header size */

static uint8_t  nfsdPacket[MAXMSGSZ];
static uint16_t nfsdPacketLen;

static uint8_t * payload = nfsdPacket + TNFS_HEADERSZ;
static uint16_t nfsdConnRx;
static uint16_t nfsdTimeout;

///////////////////////////////////////

void Dump(const char * title, uint8_t * ptr, uint8_t len)
{
    if (title)
        puts(title);  
    for (uint8_t i = 0; i < len; ++i) {
        printf(" %02x", *ptr);
        ++ptr;
    }
    printf("\n");
}

///////////////////////////////////////

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>

int clientSocket;
struct sockaddr_in serverAddr;

const char * serverHostName = "localhost";
const uint16_t serverPort   = 16384;

int client_open()
{
    // lookup hostname
    struct hostent *hostEntry = gethostbyname(serverHostName);
    if (hostEntry == NULL) {
		fprintf(stderr, "error: could find host '%s'\n", serverHostName);
        exit(-1);
    }

    printf("info: connecting to %s\n", inet_ntoa(*(struct in_addr *)(hostEntry->h_addr_list[0])));

    serverAddr.sin_family       = AF_INET;
    serverAddr.sin_port         = htons(serverPort);
    serverAddr.sin_addr.s_addr  = *(in_addr_t *)(hostEntry->h_addr_list[0]);

	// create socket
	clientSocket = socket(AF_INET, SOCK_DGRAM, 0);
	if (clientSocket == -1) {
		fprintf(stderr, "error: could not create client socket");
        exit(-1);
	}
	
	printf("Connected to %s:%u\n", serverHostName, serverPort);

    return 0;
}

void client_close()
{
    close(clientSocket);
}

void client_send()
{
    ssize_t len = sendto(clientSocket, &nfsdPacket, nfsdPacketLen, 
                         0, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
    if (len != nfsdPacketLen) {
        fprintf(stderr, "error: send failed value");
        perror(" ");
        exit(-1);
    }
}

void client_receive()
{
    nfsdPacketLen = recv(clientSocket, &nfsdPacket, sizeof(nfsdPacket), MSG_WAITALL);
    if (nfsdPacketLen <= 0) {
        fprintf(stderr, "error: receive failed");
        perror(" ");
        exit(-1);
    }
}


///////////////////////////////////////

static uint16_t nfsdConnectionID = 0;
static uint8_t  nfsdSequence = 0;
static uint8_t  nfsdCommand = 0;

uint16_t tnfs16uint(unsigned char * value)
{
    return (*(value+1) << 8) + *value;
}

void uint16tnfs(unsigned char *buf, uint16_t value)
{
    *buf=value & 0xFF;
    *(buf+1)=(value >> 8) & 0xFF;
}

void EncodeNFSD(uint8_t command, uint16_t connId)
{ 
    uint16tnfs(nfsdPacket+0, connId);
    nfsdPacket[2] = nfsdSequence;
    nfsdPacket[3] = command;
}

enum {
    TNFSD_COMMAND_MOUNT = 0
};

void SendCommand(uint16_t len)    
{
    nfsdPacketLen = len;
    EncodeNFSD(TNFSD_COMMAND_MOUNT, 0);  
    Dump("xmit", nfsdPacket, nfsdPacketLen);
    client_send();
    client_receive();
    Dump("recv", nfsdPacket, nfsdPacketLen);
    if (nfsdPacketLen < TNFS_HEADERSZ) {
        fprintf(stderr, "error: bad response to mount");
        exit(-1);
    }
    nfsdConnRx = tnfs16uint(nfsdPacket+0);
}

uint16_t tnfs_mount(const char * dir)
{
    nfsdCommand = TNFSD_COMMAND_MOUNT;

    // version
    uint8_t * ptr = payload;
    uint16tnfs(ptr, 0x201);
    ptr += 2;

    // mount point
    uint8_t len = strlen(dir);
    strncpy((char *)ptr, dir, len);
    ptr += len;
    *ptr++ = 0x00;

    // username
    *ptr++ = 0x00;

    // password
    *ptr++ = 0x00;

    SendCommand(ptr - nfsdPacket);

    if ((nfsdConnRx == 0) || (nfsdPacketLen < 9)) {
        fprintf(stderr, "error: mount failed");
        exit(-1);
    }

    nfsdTimeout = tnfs16uint(nfsdPacket+0);

    return nfsdConnRx;
}


///////////////////////////////////////

int main(int argc, char *argv[])
{
    client_open();

    uint16_t connId = tnfs_mount("/");

    printf("info: connected with id %04x\n", connId);

    client_close();

  return 0;
}
