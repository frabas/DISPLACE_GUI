

#include "GeoGraphLoader.h"
#include "GeoGraph.h"

#include <fstream>
#include <vector>

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

void GeoGraphLoader::loadNodes(GeoGraph &graph, std::istream &in)
{
    std::vector<std::string> lines;
    size_t linenum = 0;

    try {
        while (in) {
            std::string line;
            std::getline(in, line);

            boost::trim(line);
            if (line.empty()) {
                continue;
            }
            lines.emplace_back(line);
        }
    } catch (boost::bad_lexical_cast &ex) {
        std::ostringstream ss;
        ss << "Bad Conversion on fill_from_coord file line " << linenum <<
           " : " << ex.what() << "\n";
        throw std::runtime_error(ss.str());
    }

    auto nrow = lines.size() / 3;
    if (lines.size() %3 != 0)
        throw std::runtime_error("Not valid number of lines.");

    struct Det {
        float x, y;
        int harb;
    };
    std::vector<Det> details;
    while (details.size() < nrow)
        details.push_back(Det{});

    linenum = 0;
    for (auto &line: lines) {
        if (linenum < nrow) {
            auto val = boost::lexical_cast<float>(line);
            details[linenum].x = val;
        } else if (linenum < 2 * nrow) {
            auto val = boost::lexical_cast<float>(line);
            details[linenum - nrow].y = val;
        } else if (linenum < 3 * nrow) {
            auto val = boost::lexical_cast<int>(line);
            details[linenum - 2*nrow].harb =  val;
        } else {
            break;  // finish.
        }
        ++linenum;
    }

    linenum = 0;
    for (auto &node : details) {
        graph.addNode(linenum++, node.x, node.y);
    }

    std::cout << "Added: " << linenum << "nodes\n";
}

void GeoGraphLoader::loadEdges(GeoGraph &graph, std::istream &in)
{
    std::vector<std::string> lines;
    size_t linenum = 0;

    try {
        while (in) {
            std::string line;
            std::getline(in, line);

            boost::trim(line);
            if (line.empty()) {
                continue;
            }
            lines.emplace_back(line);
        }
    } catch (boost::bad_lexical_cast &ex) {
        std::ostringstream ss;
        ss << "Bad Conversion on graph file line " << linenum <<
           " : " << ex.what() << "\n";
        throw std::runtime_error(ss.str());
    }

    auto nrow = lines.size() / 3;
    if (lines.size() %3 != 0)
        throw std::runtime_error("Not valid number of lines.");

    linenum = 0;

    struct Det {
        int from, to;
        float dist;
    };
    std::vector<Det> details;
    while (details.size() < nrow)
        details.push_back(Det{});

    for (auto &line: lines) {
        if (linenum < nrow) {
            auto val = boost::lexical_cast<int>(line);
            details[linenum].from = val;
        } else if (linenum < 2 * nrow) {
            auto val = boost::lexical_cast<int>(line);
            details[linenum - nrow].to = val;
        } else if (linenum < 3 * nrow) {
            auto val = boost::lexical_cast<float>(line);
            details[linenum - 2*nrow].dist =  val;
        } else {
            break;  // finish.
        }
        ++linenum;
    }

    for (auto &node : details) {
        graph.addEdge(node.from, node.to, node.dist);
    }
}


void GeoGraphLoader::load(GeoGraph &graph, std::string coordFileName, std::string graphFileName)
{

    std::ifstream coordStream;

    coordStream.open(coordFileName, std::ios::in);

    if (coordStream.fail()) {
        throw std::runtime_error("Cannot load coord file");
    }

    loadNodes(graph, coordStream);

    std::ifstream edgeStream;

    edgeStream.open(graphFileName, std::ios::in);

    if (edgeStream.fail()) {
        throw std::runtime_error("Cannot load edges file");
    }

    loadEdges(graph, edgeStream);

 }
