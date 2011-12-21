#include "socket.h"
#include "packet.h"
#include "hack.h"

#include <ctime>
#include <netdb.h>
#include <sys/socket.h>
#include <cstring>
#include <cstdio>

#define MESSAGE_HEADER_INIT 0x74d32f8c

/* PACKET HEADER (13 bytes total):
   - four bytes MESSAGE_HEADER_INIT, for confirmation
   - packet number, unsigned int
   - length of message after header, unsigned int
   - message type (constants in packet.h), 1 byte
   */

SocketConnection::SocketConnection(int sock) {
    socket = sock;
    num_sent = 0;
    lastTimeReceived = time(NULL);
}

SocketConnection::~SocketConnection() {
    
}

ReadPacket* SocketConnection::receive_packet() {
    char header[13];
    int bytes_received = recv(socket, header, 13, MSG_DONTWAIT);
    if(bytes_received < 13)
        return NULL;

    int header_ints[3];
    memcpy((void *)header_ints, (void *)header, 12);
    int conf = ntohl(header_ints[0]);
    int num = ntohl(header_ints[1]);
    int len = ntohl(header_ints[2]);
    char type = header[12];

    if(conf != MESSAGE_HEADER_INIT)
        return NULL;

    ReadPacket* rp = new ReadPacket(type, len, num);
    bytes_received = recv(socket, rp->buf, len, MSG_DONTWAIT);
    if(len > 0 && bytes_received < len) {
		// note we check that len > 0 because we might have bytes_received = -1
		// if no bytes are received here
        delete rp;
        return NULL;
    }
    lastTimeReceived = time(NULL);
    return rp;
}

void SocketConnection::send_packet(WritePacket* wp) {
    int header_ints[3];

    char *msg = new char[13 + wp->size];
    header_ints[0] = htonl(MESSAGE_HEADER_INIT);
    header_ints[1] = htonl(num_sent++);
    header_ints[2] = htonl(wp->size);
    memcpy((void *)msg, (void *)header_ints, 12);
    msg[12] = wp->message_type;
    
    memcpy((void *)(msg + 13), (void *)wp->buf, wp->size);

    send(socket, msg, 13 + wp->size, 0);

    delete[] msg;
}
