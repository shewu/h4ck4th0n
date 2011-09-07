#ifndef PACKET_H
#define PACKET_H

/* Note: if the initial value of max_size given is not large enough,
   then this will work correctly but probably be slow due to
   constant rebuffering. */

class WritePacket {
    public:
        int max_size, size;
        char *buf;
        char message_type;

        WritePacket(int ms, char type);
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

        ReadPacket(int size, char message_type, int packet_number);
        virtual ~ReadPacket();

        char read_char();
        int read_int();
        short read_short();
        float read_float();
        std::string read_string();
}

#endif
