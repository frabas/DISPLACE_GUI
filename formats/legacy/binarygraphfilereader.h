#ifndef BINARYGRAPHFILEREADER_H
#define BINARYGRAPHFILEREADER_H

#include <utils/endian.h>

#include <string>
#include <functional>
#include <system_error>

#include <cstdio>

namespace displace {
namespace formats {
namespace legacy {

class BinaryGraphFileReader {
public:
    class Exception : public std::system_error {
    };

    BinaryGraphFileReader() {
    }

    template <typename Key, typename Value>
    using LoaderFunction = std::function<bool(Key, Value)>;

    template <typename Key, typename Value>
    bool importFromStream(const std::string &filename, LoaderFunction<Key,Value> use) {
        using namespace displace::formats::helpers;

        FILE *file = fopen(filename.c_str(), "rb");
        if (file == nullptr) {
            throw Exception(errno);
        }

        struct __attribute__ ((__packed__)) {
            uint32_t signature;
            uint8_t keysize, valuesize;
        } header;

        if (!fread(&header, sizeof(header), 1, file) != 1) {
            throw Exception(errno);
        }

        if (fromLittleEndian( header.signature ) != 0x01020304 || header.keysize != sizeof(Key) || header.valuesize != sizeof(Value)) {
            throw Exception(0, "Bad file structure");
        }

        while (!feof(file)) {
            struct __attribute__ ((__packed__)) {
                Key k;
                Value v;
            } rec;

            if (fread(&rec, sizeof(rec), 1, file ) != 1) {
                throw Exception(errno);
            }

            if (!use(fromLittleEndian(rec.k), fromLittleEndian(rec.v)))
                throw Exception(0, "Cannot insert data");
        }

        fclose(file);
        return true;
    }

};

} // ns
}
}


#endif // BINARYGRAPHFILEREADER_H
