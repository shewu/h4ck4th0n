#include "Packet.h"

#include <inttypes.h>
#include <assert.h>
#include <netdb.h>
#include <stdint.h>

#include <cstring>
#include <cstdio>
#include <string>

using std::string;

// Constructor
WritePacket::WritePacket(char type, int ms) {
    max_size = ms;
    size = 0;
    buf = new char[max_size];
    message_type = type;
}

// Destructor
WritePacket::~WritePacket() {
    delete[] buf;
}

void WritePacket::reset() {
	size = 0;
}

void WritePacket::reset(int pos) {
	size = pos;
}

// Constructor
// The argument 's' is an initial guess
// for the size of the packet, but it
// does not have to be correct.
ReadPacket::ReadPacket(char type, long s, int n) {
    size = s;
    buf = new char[size];
    message_type = type;
    index = 0;
    packet_number = n;
}

// Destructor
ReadPacket::~ReadPacket() {
    delete[] buf;
}

// Read and write characters

char ReadPacket::read_char() {
    if(index == size)
        return '\0';
    index++;
    return buf[index-1];
}

void WritePacket::write_char(char c) {
    while(size + 1 > max_size)
        _increase_buf_size();
    buf[size] = c;
    size++;
}

// Read and write ints

int ReadPacket::read_int() {
    if(index > size - 4)
        return 0;
    int i;
    memcpy((void *)&i, (void *)&buf[index], 4);
    index += 4;
    return ntohl(i);
}

void WritePacket::write_int(int i) {
    while(size + 4 > max_size)
        _increase_buf_size();
    i = htonl(i);
    memcpy((void *)&buf[size], (void *)&i, 4);
    size += 4;    
}

short ReadPacket::read_short() {
    if(index > size - 2)
        return 0;
    short s;
    memcpy((void *)&s, (void *)&buf[index], 2);
    index += 2;
    return ntohl(s);
}

void WritePacket::write_short(short s) {
    while(size + 2 > max_size)
        _increase_buf_size();
    s = htonl(s);
    memcpy((void *)&buf[size], (void *)&s, 2);
    size += 2;
}

// Read and write strings

// Maximal length of strings is 2^15 - 1.
// We sent strings by first writing their
// length as a 16-bit integer, followed
// by the characters of the string.

string ReadPacket::read_string() {
    int length = (int)read_short();
    if(length == 0 || index + length > size)
        return (string)"";
    string s = string(buf + index, length);
    index += length;
    return s;
}

void WritePacket::write_string(string const& s) {
	assert(s.size() < (1 << 15));

    write_short((short)s.size());
    while(size + (int)s.size() > max_size)
        _increase_buf_size();
    memcpy((void *)(buf + size), (void *)s.data(), s.size());
    size += s.size();
}

// Read and write floats

// To ensure that floats are sent in a consistent format,
// I found these functions online which convert floats to
// and form a specified format.

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
    if(index > size - 4)
        return 0.0f;
    return (float)unpack754_32(read_int());
}

// Make sure buf is large enough

void WritePacket::_increase_buf_size() {
	int new_max_size = (max_size > 0 ? max_size * 2 : 1);
    char *buf2 = new char[new_max_size];
    memcpy(buf2, buf, size);
    delete[] buf;
    buf = buf2;
    max_size = new_max_size;
}

// Getters for WritePacket

char WritePacket::getMessageType() const {
	return message_type;
}

int WritePacket::getSize() const {
	return size;
}

char const* WritePacket::getContents() const {
	return buf;
}

void WritePacket::backup(int nbytes) {
	assert(size >= nbytes);
	size -= nbytes;
}
