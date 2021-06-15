///
/// @file
///
/// General-purpose utility functions that have no dependency
///

#include <cstddef>
#include <ostream>
#include <stdexcept>
#include <vector>

template <class T>
std::ostream &operator<<(std::ostream &os, const std::vector<T> &vec) {
    bool first = true;
    os << "{";
    for (const auto &el : vec) {
        if (first) {
            first = false;
        } else {
            os << ", ";
        }
        os << el;
    }
    os << "}";
    return os;
}

/// Interprets the object pointed to by `object` as a span of bytes
template <class T> span<char> as_byte_span(T *object) {
    if (object == nullptr)
        throw std::runtime_error(
            "Cannot convert null pointer's data to byte span");
    return span<char>{(char *)object, sizeof(T)};
}
