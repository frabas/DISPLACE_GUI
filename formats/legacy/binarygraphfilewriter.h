#ifndef BINARYGRAPHFILEWRITER_H
#define BINARYGRAPHFILEWRITER_H

#include <formats_globals.h>

#include <utils/endian.h>

#include <string>
#include <functional>

#include <cstdio>

#include <system_error>

#ifdef _MSC_VER
#define PACKED
#else
#define PACKED __attribute__ ((__packed__))
#endif

namespace displace {
namespace formats {
namespace legacy {

using namespace displace::formats::helpers;

template <typename Key, typename Value>
class BinaryGraphFileWriter {

    FILE *mFile = nullptr;
public:
    BinaryGraphFileWriter() {
    }

    bool open (const std::string &filename) {
        mFile = fopen(filename.c_str(), "wb");
        if (mFile == nullptr) {
            throw std::system_error(errno,std::generic_category());
        }

        #pragma pack(push,1)
        struct PACKED {
            uint32_t signature;
            uint8_t keysize, valuesize;
        } header;
        #pragma pack(pop)

        header.signature = toLittleEndian((uint32_t)0x01020304);
        header.keysize = sizeof(Key);
        header.valuesize = sizeof(Value);

        if (fwrite(&header, sizeof(header), 1, mFile) != 1) {
            throw std::system_error(errno,std::generic_category());
        }
        return true;
    }

    bool write (Key key, Value value) {
        #pragma pack(push,1)
        struct PACKED {
            Key k;
            Value v;
        } rec;
        #pragma pack(pop)

        rec.k = toLittleEndian(key);
        rec.v = toLittleEndian(value);

        if (fwrite(&rec, sizeof(rec), 1, mFile ) != 1) {
            throw std::system_error(errno,std::generic_category());
        }
        return true;
    }

    bool close() {
        fclose(mFile);
        return true;
    }
};

} // ns
}
}


#endif // BINARYGRAPHFILEWRITER_H
