#ifndef ENDIAN_H
#define ENDIAN_H

#include <endian.h>

#include <cstdint>

namespace displace {
namespace formats {
namespace helpers {

// Default are disabled

template <typename T>
T inline toBigEndian (T v) { char X[!sizeof(T)]; }

template <typename T>
T inline fromBigEndian (T v) { char X[!sizeof(T)]; }

template <typename T>
T inline toLittleEndian (T v) { char X[!sizeof(T)]; }

template <typename T>
T inline fromLittleEndian (T v) { char X[!sizeof(T)]; }


// Specializations

template <>
uint16_t inline toBigEndian(uint16_t v) {
    return htobe16(v);
}

template <>
uint32_t inline toBigEndian(uint32_t v) {
    return htobe32(v);
}

template <>
uint16_t inline fromBigEndian(uint16_t v) {
    return be16toh(v);
}

template <>
uint32_t inline fromBigEndian(uint32_t v) {
    return be32toh(v);
}


// LE

template <>
uint16_t inline toLittleEndian(uint16_t v) {
    return htole16(v);
}

template <>
uint32_t inline toLittleEndian(uint32_t v) {
    return htole32(v);
}

template <>
uint16_t inline fromLittleEndian(uint16_t v) {
    return le16toh(v);
}

template <>
uint32_t inline fromLittleEndian(uint32_t v) {
    return le32toh(v);
}


}
}
}

#endif // ENDIAN_H
