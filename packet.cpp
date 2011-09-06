#include "packet.h"

#include <netdb.h>
#include <cstring>
#include <stdint.h>
#include <inttypes.h>

WritePacket::WritePacket(int ms, char type) {
    max_size = ms;
    size = 0;
    buf = new char[max_size];
    message_type = type;
}

WritePacket::~WritePacket() {
    delete buf;
}

ReadPacket::ReadPacket(int s, int type, int n) {
    size = s;
    buf = new char[size];
    message_type = type;
    index = 0;
    packet_number = n;
}

ReadPacket::~ReadPacket() {
    delete buf;
}

// Read and write characters

char ReadPacket::read_char() {
    if(index == size)
        return '\0';
    index++;
    return buf[index-1];
}

void WritePacket::write_char(char c) {
    if(size == max_size)
        increase_buf_size();
    buf[size] = c;
    size++;
}

// Read and write ints

int ReadPacket::read_int() {
    if(index > max_size - 4)
        return 0;
    int i;
    memcpy((void *)&i, (void *)&buf[index], 4);
    index += 4;
    return ntohl(i);
}

void WritePacket::write_int(int i) {
    if(size + 4 > max_size)
        increase_buf_size();
    i = htonl(i);
    memcpy((void *)&buf[size], (void *)&i, 4);
    size += 4;    
}

// Read and write floats

#define pack754_32(f) (pack754((f), 32, 8))
#define pack754_64(f) (pack754((f), 64, 11))
#define unpack754_32(i) (unpack754((i), 32, 8))
#define unpack754_64(i) (unpack754((i), 64, 11))

inline uint64_t pack754(long double f, unsigned bits, unsigned expbits) {
    long double fnorm;
    int shift;
    long long sign, exp, significand;
    unsigned significandbits = bits - expbits - 1; // -1 for sign bit

    if (f == 0.0) return 0; // get this special case out of the way

    // check sign and begin normalization
    if (f < 0) { sign = 1; fnorm = -f; }
    else { sign = 0; fnorm = f; }

    // get the normalized form of f and track the exponent
    shift = 0;
    while(fnorm >= 2.0) { fnorm /= 2.0; shift++; }
    while(fnorm < 1.0) { fnorm *= 2.0; shift--; }
    fnorm = fnorm - 1.0;

    // calculate the binary form (non-float) of the significand data
    significand = fnorm * ((1LL<<significandbits) + 0.5f);

    // get the biased exponent
    exp = shift + ((1<<(expbits-1)) - 1); // shift + bias

    // return the final answer
    return (sign<<(bits-1)) | (exp<<(bits-expbits-1)) | significand;
}

inline long double unpack754(uint64_t i, unsigned bits, unsigned expbits) {
    long double result;
    long long shift;
    unsigned bias;
    unsigned significandbits = bits - expbits - 1; // -1 for sign bit

    if (i == 0) return 0.0;

    // pull the significand
    result = (i&((1LL<<significandbits)-1)); // mask
    result /= (1LL<<significandbits); // convert back to float
    result += 1.0f; // add the one back on

    // deal with the exponent
    bias = (1<<(expbits-1)) - 1;
    shift = ((i>>significandbits)&((1LL<<expbits)-1)) - bias;
    while(shift > 0) { result *= 2.0; shift--; }
    while(shift < 0) { result /= 2.0; shift++; }

    // sign it
    result *= (i>>(bits-1))&1? -1.0: 1.0;

    return result;
}

void WritePacket::write_float(float f) {
    write_int((int)pack754_32(f));
}

float ReadPacket::read_float() {
    return (float)unpack754_32(read_int());
}

// Make sure buf is large enough

void WritePacket::increase_buf_size() {
    char *buf2 = new char[max_size + 4];
    memcpy(buf2, buf, size);
    delete buf;
    buf = buf2;
    max_size += 4;
}
