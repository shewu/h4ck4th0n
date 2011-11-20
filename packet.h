#ifndef PACKET_H
#define PACKET_H

#include "network_protocol.h"

/* Note: if the initial value of max_size given is not large enough,
   then this will work correctly as it can resize the buffer. Of
   course, it is best to be as accurate as possible. */

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
        increase_buf_size();
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
}

#endif
