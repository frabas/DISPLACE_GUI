#ifndef ENDIAN_H_
#define ENDIAN_H_

#include <utils/portable_endian.h>

#if !defined(_HAS_STD_BYTE)
#define _HAS_STD_BYTE 0
#endif
#include <cstdint>
#undef _HAS_STD_BYTE

namespace displace {
namespace formats {
namespace helpers {

// Default are disabled

#define DELETED { static_assert(sizeof(T) == 0, "Non specialized functions are deleted"); }

template <typename T>
T inline toBigEndian (T) DELETED

template <typename T>
T inline fromBigEndian (T) DELETED

template <typename T>
T inline toLittleEndian (T) DELETED

template <typename T>
T inline fromLittleEndian (T) DELETED


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
