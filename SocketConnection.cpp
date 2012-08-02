#include "Socket.h"

#include <netdb.h>
#include <sys/socket.h>

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <algorithm>

#include "Packet.h"

using std::max;

/* PACKET HEADER (13 bytes total):
   - four bytes MESSAGE_HEADER_INIT, for confirmation
   - packet number, unsigned int
   - length of message after header, unsigned int
   - message type (constants in packet.h), 1 byte
   */

SocketConnection::SocketConnection(int socket, sockaddr *addr, socklen_t addrlen, int my_id, int their_id) {
    this->socket = socket;
    this->num_sent = 0;
    this->lastTimeReceived = time(NULL);

	this->addr = (struct sockaddr *)malloc(addrlen);
	memcpy(this->addr, addr, addrlen);
	this->addrlen = addrlen;
	this->my_id = my_id;
	this->their_id = their_id;

	this->largestPacketNum = 0;
}

SocketConnection::~SocketConnection() {
    free(this->addr);
}

ReadPacket* SocketConnection::receive_packet() {
	if(read_packets.size() == 0)
		return NULL;
	ReadPacket *rp = read_packets.front();
	read_packets.pop();

	largestPacketNum = max(largestPacketNum, rp->packet_number);

	return rp;
}

void SocketConnection::recv_data(char *buf, int length) {
    if (length < 5) {
    	return;
    }
    int header;
    memcpy((void *)&header, (void *)buf, 4);
    int num = ntohl(header);
    char type = buf[4];

    ReadPacket* rp = new ReadPacket(type, length - 5, num);
    if(length - 5 > 0) {
      memcpy(rp->buf, buf + 5, length - 5);
	}

    lastTimeReceived = time(NULL);
	read_packets.push(rp);
}

void SocketConnection::send_packet(WritePacket const& wp) {
    int header_ints[3];

    int size = wp.getSize();

    char *msg = new char[13 + size];
    header_ints[0] = htonl(MESSAGE_HEADER_INIT);
    header_ints[1] = htonl(my_id);
    header_ints[2] = htonl(num_sent++);
    memcpy((void *)msg, (void *)header_ints, 12);
    msg[12] = wp.getMessageType();

    memcpy((void *)(msg + 13), (void const *)wp.getContents(), size);

    sendto(socket, msg, 13 + size, MSG_NOSIGNAL, addr, addrlen);

    delete[] msg;
}
