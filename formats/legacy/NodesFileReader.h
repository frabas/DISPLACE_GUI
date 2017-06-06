//
// Created by Federico Fuga on 04/03/16.
//

#ifndef DISPLACE_NODESFILEREADER_H
#define DISPLACE_NODESFILEREADER_H

#include <formats_globals.h>

#include <functional>
#include <iostream>
#include <string>
#include <tuple>
#include <vector>
#include <set>

#include <boost/lexical_cast.hpp>

namespace displace {
    namespace formats {
        namespace legacy {

            class FORMATSSHARED_EXPORT NodesFileReader {
            public:
                template<typename CoordType, typename AttributeType>
                using Node = std::tuple<CoordType, CoordType, AttributeType>;

                NodesFileReader() { }

                ~NodesFileReader() noexcept = default;

                template<typename CoordType, typename AttributeType>
                bool read(std::istream &stream, unsigned int nrows,
                          std::vector<Node<CoordType, AttributeType>> &nodes) {
                    unsigned int linenum = 0;
                    unsigned int iteration = 0;
                    while (stream) {
                        std::string line;
                        std::getline(stream, line);

                        auto nidx = linenum % nrows;

                        while (nodes.size() <= nidx)
                            nodes.push_back(Node<CoordType, AttributeType>());

                        switch (iteration) {
                            case 0:
                                std::get<0>(nodes[nidx]) = boost::lexical_cast<CoordType>(line);
                                break;
                            case 1:
                                std::get<1>(nodes[nidx]) = boost::lexical_cast<CoordType>(line);
                                break;
                            case 2:
                                std::get<2>(nodes[nidx]) = boost::lexical_cast<AttributeType>(line);
                                break;
                        }

                        ++linenum;
                        if (linenum == (iteration + 1) * nrows)
                            ++iteration;
                    }

                    return true;
                }

                template<typename CoordType>
                using CoordFeedback = std::function<void(int, int, CoordType)>;
                template<typename AttributeType>
                using AttributeFeedback = std::function<void(int, AttributeType)>;

                template<typename CoordType, typename AttributeType>
                bool read(std::istream &stream, unsigned int nrows, CoordFeedback<CoordType> cfeedback,
                          AttributeFeedback<AttributeType> afeedback) {
                    unsigned int linenum = 0;
                    unsigned int iteration = 0;
                    while (stream) {
                        std::string line;
                        std::getline(stream, line);

                        auto nidx = linenum % nrows;

                        switch (iteration) {
                            case 0:
                            case 1:
                                if (cfeedback != nullptr)
                                    cfeedback(iteration, nidx, boost::lexical_cast<CoordType>(line));
                                break;
                            case 2:
                                if (afeedback != nullptr)
                                    afeedback(nidx, boost::lexical_cast<AttributeType>(line));
                                break;
                        }

                        ++linenum;
                        if (linenum == (iteration + 1) * nrows)
                            ++iteration;

                    }

                    return true;
                }

            };
        }
    }
}

#endif //DISPLACE_NODESFILEREADER_H
