#ifndef PACKET_H
#define PACKET_H

#include <string>
#include "network_protocol.h"

/* WritePackets are passed to the SocketConnection class
   (see socket.h) to be sent out. ReadPackets are read
   from a SocketConnection.

   Every packet (incoming or outgoing) has a ``message_type''.
   The type says what kind of message the packet contains.
   In most cases, the packet will contain additional data.
   These classes allow you to read/write this data like
   a typical stream, and supports the following primitives:
   char, int, short, float, and std::string.

   See network_protocol.h for the specification of
   the networking protocol. It should contain constants
   for the message_type field, as well as information
   about how data should be stored for a given message
   type.

   Note: if the initial value of max_size
   (passed to the constructor of WritePacket) is not large enough,
   then WritePacket will be able to resize the buffer as
   necessary. Of course, for performance, it is best to give
   as good an approximation as possible.
*/

class WritePacket {
    public:
        int max_size, size;
        char *buf;
        char message_type;

        WritePacket(char message_type, int max_size = 1);
        virtual ~WritePacket();

        void write_char(char c);
        void write_int(int i);
        void write_short(short s);
        void write_float(float f);
        void write_string(std::string s);

    private:
        void increase_buf_size();
};

class ReadPacket {
    public:
        int size;
        char *buf;
        char message_type;
        int index;
        int packet_number;

        ReadPacket(char message_type, int size, int packet_number);
        virtual ~ReadPacket();

        char read_char();
        int read_int();
        short read_short();
        float read_float();
        std::string read_string();
};

#endif
