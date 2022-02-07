//
// Created by happycactus on 03/02/22.
//

#ifndef DISPLACE_VALUESPERMETPERPOPCONTAINER_H
#define DISPLACE_VALUESPERMETPERPOPCONTAINER_H

#include <vector>
#include <stdexcept>
#include <iostream>

class ValuesPerMetPerPopContainer {
    using MetierVector = std::vector<double>;

    size_t valueCount = 0, metierCount = 0, popCount = 0;
    size_t validMetierCount = 0;

    std::vector<MetierVector> data;
    std::vector<size_t> metierMapper;

    size_t metierIndex(size_t metierNumber) const
    {
        if (metierNumber < metierMapper.size()) {
            return metierMapper[metierNumber];
        }
        return NullIndex;
    }

    size_t newMetierIndex(size_t metierNumber)
    {
        auto i = validMetierCount;
        metierMapper[metierNumber] = validMetierCount++;
        return i;
    }

    size_t metierVectorIndex(size_t index, size_t pop) const
    {
        return index * popCount + pop;
    }

    static constexpr size_t NullIndex = size_t(-1);

public:
    ValuesPerMetPerPopContainer()
    {

    }

    ValuesPerMetPerPopContainer(int maxVal, int maxMet, int maxPop)
    {
        init(maxVal, maxMet, maxPop);
    }

    void init(int maxVal, int maxMet, int maxPop)
    {
        if (valueCount != 0 || metierCount != 0 || popCount != 0) {
            throw std::runtime_error("Double initialization of ValuesPerMetPerPopContainer");
        }

        valueCount = maxVal;
        metierCount = maxMet;
        popCount = maxPop;
        data.resize(valueCount * popCount);
        metierMapper.resize(metierCount);
        std::fill(metierMapper.begin(), metierMapper.end(), NullIndex);
    }

    double &operator()(int index, int metier, int pop)
    {
        auto metIndex = metierIndex(metier);
        if (metIndex == NullIndex) {
            metIndex = newMetierIndex(metier);
        }
        auto pindex = metierVectorIndex(index, pop);
        while (data[pindex].size() <= metIndex) {
            data[pindex].push_back({});
        }
        return data[pindex][metIndex];
    }

    double const &operator()(int index, int metier, int pop) const
    {
        auto metIndex = metierIndex(metier);
        if (metIndex != NullIndex) {
            return data[metierVectorIndex(index, pop)][metIndex];
        }
        throw std::out_of_range("Metier Out of Range");
    }

    bool hasValue(int index, int metier, int pop) const
    {
        auto mIndex = metierMapper[metier];
        return mIndex != NullIndex &&
               metier < data[metierVectorIndex(index, pop)].size();
    }

    double value(int index, int metier, int pop) const
    {
        auto metIndex = metierIndex(metier);
        auto pIndex = metierVectorIndex(index, pop);
        if (metIndex != NullIndex && metIndex < data[pIndex].size()) {
            return data[pIndex][metIndex];
        }
        return {};
    }

    void zero(int index, int metier, int pop)
    {
        auto metIndex = metierIndex(metier);
        auto pIndex = metierVectorIndex(index, pop);
        if (metIndex != NullIndex && metIndex < data[pIndex].size()) {
            data[pIndex][metIndex] = 0;
        }
    }

    void clear()
    {
        std::fill(metierMapper.begin(), metierMapper.end(), NullIndex);
        std::fill(data.begin(), data.end(), MetierVector{});
    }

    size_t allocated_size() const
    {
        return 0;  // TODO fix this
    }

    size_t dimension(int index) const
    {
        switch (index) {
            case 0:
                return valueCount;
            case 1:
                return metierCount;
            case 2:
                return popCount;
            default:
                throw std::invalid_argument("Index oyut of bounds 0..2");
        }
    }

};


#endif //DISPLACE_VALUESPERMETPERPOPCONTAINER_H
