#ifndef ATS_FOOTERS
#define ATS_FOOTERS

#include <cassert>
#include <cstdbool>
#include <cstddef>
#include <cstdint>
#include <vector>

#ifndef ATSFOOTERSLIB
#    ifdef _MSC_VER
#        ifdef ATSFOOTERSLIBEXPORT
#            define ATSFOOTERSLIB __declspec(dllexport) __cdecl
#        else
#            define ATSFOOTERSLIB __declspec(dllimport) __cdecl
#        endif
#    else
#        define ATSFOOTERSLIB
#    endif
#endif

/// Bare-bones replacement for C++20's std::span
template <class T> class span {
    T *m_data;     //< Points to an array of `T` elements
    size_t m_size; //< Number of `T` elements in `data`

  public:
    span(T *data, size_t size) noexcept : m_data(data), m_size(size) {}
    size_t size() const { return m_size; }
    T *data() const { return m_data; }
    T *begin() const noexcept { return m_data; }
    T *end() const noexcept { return m_data + m_size; }
    T &operator[](size_t i) const { return m_data[i]; }
    span<T> subspan(size_t offset) {
        assert(m_size > offset);
        return span(m_data + offset, m_size - offset);
    }
};

/// Identifies the ATS footer types that various products generate
enum class ats_footer_type {
    type_0,
    type_1,
};

/// Data corresponding to ATS footer type 0
struct ats_footer_type_0 {
    uint64_t trigger_timestamp;
    uint32_t record_number;
    uint32_t frame_count;
    bool aux_in_state;
};

/// Data corresponding to ATS footer type 1
struct ats_footer_type_1 {
    uint64_t trigger_timestamp;
    uint32_t record_number;
    uint32_t frame_count;
    bool aux_in_state;
    int16_t analog_value;
};

/// AlazarTech products
enum class ats_board_type {
    ats850 = 1,
    ats310 = 2,
    ats330 = 3,
    ats855 = 4,
    ats315 = 5,
    ats335 = 6,
    ats460 = 7,
    ats860 = 8,
    ats660 = 9,
    ats665 = 10,
    ats9462 = 11,
    ats9434 = 12,
    ats9870 = 13,
    ats9350 = 14,
    ats9325 = 15,
    ats9440 = 16,
    ats9410 = 17,
    ats9351 = 18,
    ats9310 = 19,
    ats9461 = 20,
    ats9850 = 21,
    ats9625 = 22,
    atg6500 = 23,
    ats9626 = 24,
    ats9360 = 25,
    axi9870 = 26,
    ats9370 = 27,
    atu7825 = 28,
    ats9373 = 29,
    ats9416 = 30,
    ats9637 = 31,
    ats9120 = 32,
    ats9371 = 33,
    ats9130 = 34,
    ats9352 = 35,
    ats9453 = 36,
    ats9146 = 37,
    ats9000 = 38,
    atst371 = 39,
    ats9437 = 40,
    ats9618 = 41,
    ats9358 = 42,
    forest = 43,
    ats9353 = 44,
    ats9872 = 45,
    ats9470 = 46,
    ats9628 = 47,
};

enum class ats_data_domain {
    time = 0x1000,
    frequency = 0x2000,
};

enum class ats_data_layout {
    buffer_interleaved = 0x100000,
    record_interleaved = 0x200000,
    sample_interleaved = 0x300000,
};

/// Acquisition configuration necessary for extracting ATS footer data
struct ats_footer_configuration {
    ats_board_type board_type;
    ats_data_domain data_domain;
    size_t active_channel_count;
    ats_data_layout data_layout;
    size_t bytes_per_record_per_channel;
    size_t records_per_buffer_per_channel;

    /// Indicates if the acquisition was performed with the
    /// `ADMA_FIFO_ONLY_STREAMING` option
    bool fifo;
};

ats_footer_type ATSFOOTERSLIB get_ats_footer_type(ats_board_type board_type);

void ATSFOOTERSLIB ats_parse_footers(span<char> data,
                                     ats_footer_configuration configuration,
                                     span<ats_footer_type_0> footers);

void ATSFOOTERSLIB ats_parse_footers(span<char> data,
                                     ats_footer_configuration configuration,
                                     span<ats_footer_type_1> footers);

extern "C" int ATSFOOTERSLIB c_ats_parse_footers_type_0(
    char *data, size_t data_size_bytes, ats_footer_configuration configuration,
    ats_footer_type_0 *footers, size_t footer_count, char *error_message,
    size_t error_message_max_size);

extern "C" int ATSFOOTERSLIB c_ats_parse_footers_type_1(
    char *data, size_t data_size_bytes, ats_footer_configuration configuration,
    ats_footer_type_1 *footers, size_t footer_count, char *error_message,
    size_t error_message_max_size);

#endif // ATS_FOOTERS
