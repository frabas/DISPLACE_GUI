/** @file 
 * @author Federico Fuga <fuga@studiofuga.com>
 * @date 01/02/18
 * @addtogroup Bm3Utils
 * @{
 */

#ifndef BRIDGEMATE3_STRONGTYPE_H
#define BRIDGEMATE3_STRONGTYPE_H

#include <utility>

namespace utils {

/**
 * @brief A Strong Types POD
 * For some usage of this type, @sa PairId, @sa ScoreValue and @sa ScoreTypes.h
 * @tparam T the contained POD type
 * @tparam TAG A Tag for the Strong Types.
 */
template <typename T, typename TAG>
class StrongType {
private:
    T mValue;

public:
    using Type = T;

    /// @brief the Default Constructor
    StrongType() : mValue() {}

    /**
     * @brief Constructor: Creates a StrongType from a POD of type T
     * @param v the value of the POD
     */
    explicit StrongType(T v)
    : mValue(std::move(v)) {

    }

    /**
     * @brief Converts (extracts) the POD value from the StrongType
     * @return the value as a POD (of type T)
     */
    const T &value() const { return mValue; }

};

/// @brief Equality operator for StrongType.
template <typename T, typename TAG>
inline bool operator ==(StrongType<T,TAG> lh, StrongType<T,TAG> rh) {
    return (lh.value() == rh.value());
}

/// @brief Unequality operator for StrongType.
template <typename T, typename TAG>
inline bool operator !=(StrongType<T,TAG> lh, StrongType<T,TAG> rh) {
    return (lh.value() != rh.value());
}

}

/// @}

#endif //BRIDGEMATE3_STRONGTYPE_H
