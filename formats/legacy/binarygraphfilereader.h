#ifndef BINARYGRAPHFILEREADER_H
#define BINARYGRAPHFILEREADER_H

#include <formats_globals.h>

#include <utils/endian.h>

#include <string>
#include <functional>
#include <system_error>

#include <cstdio>

#ifndef ATTRIBUTE_PACKED
#if !defined(_MSC_BUILD)
#define ATTRIBUTE_PACKED __attribute__ ((__packed__))
#else
#define ATTRIBUTE_PACKED
#endif

#endif

namespace displace {
namespace formats {
namespace legacy {

class BinaryGraphFileReader {
public:
    BinaryGraphFileReader() {
    }

    template <typename Key, typename Value>
    using LoaderFunction = std::function<bool(Key, Value)>;

    template <typename Key, typename Value>
    bool importFromStream(const std::string &filename, LoaderFunction<Key,Value> use) {
        using namespace displace::formats::helpers;

        FILE *file = fopen(filename.c_str(), "rb");
        if (file == nullptr) {
            throw std::system_error(errno,std::generic_category());
        }

        #pragma pack(push,1)
        struct ATTRIBUTE_PACKED {
            uint32_t signature;
            uint8_t keysize, valuesize;
        } header;
        #pragma pack(pop)

        size_t res;
        if ((res = fread(&header, sizeof(header), 1, file)) != 1) {
            throw std::system_error(errno,std::generic_category());
        }

        if (fromLittleEndian( header.signature ) != 0x01020304 || header.keysize != sizeof(Key) || header.valuesize != sizeof(Value)) {
            throw std::system_error(0,std::generic_category(), "Bad header or sizes");
        }

        while (!feof(file)) {
            #pragma pack(push,1)
            struct ATTRIBUTE_PACKED {
                Key k;
                Value v;
            } rec;
            #pragma pack(pop)

            if ((res = fread(&rec, sizeof(rec), 1, file )) != 1) {
                if (feof(file))
                    break;

                throw std::system_error(errno,std::generic_category());
            }

            if (!use(fromLittleEndian(rec.k), fromLittleEndian(rec.v)))
                throw std::system_error(0,std::generic_category(), "cannot insert data");
        }

        fclose(file);
        return true;
    }

};

} // ns
}
}


#endif // BINARYGRAPHFILEREADER_H
