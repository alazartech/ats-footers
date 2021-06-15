#ifndef ATSFOOTERS_INTERNAL_H
#define ATSFOOTERS_INTERNAL_H

#include <cstdint>
#include <sstream>
#include <vector>

#include "atsfooters.hpp"

struct ats_footer_internal {
    uint8_t aux_and_pulsar_low;
    uint8_t pulsar_high;
    uint16_t tt_low;
    uint16_t tt_med;
    uint16_t tt_high;
    uint16_t rn_low;
    uint16_t rn_high;
    uint16_t fc_low;
    uint8_t fc_high;
    uint8_t type;
};

void parse_footer(const ats_footer_internal *source,
                  ats_footer_type_0 *destination);

void parse_footer(const ats_footer_internal *source,
                  ats_footer_type_1 *destination);

/// Describes how record footer data is embdedded in DMA buffers
enum class record_footer_embedding {
    /// NPT footer data replaces the last samples of each record. With multiple
    /// channels active, this means that the position of NPT footer data in the
    /// buffer will vary depending on the interleaving.
    ///
    /// There is only a single record footer per record, shared between all
    /// channels
    channel_data_shared,

    /// Same as `channels_data_shared`, but there is one record per channel.
    channel_data_one_per_channel,

    /// NPT footer data is present at "fixed" locations in DMA buffers,
    /// irrespective of channel interleaving. In this configuration, the last
    /// bytes of each "record block" are replaced with NPT footer data. "record
    /// blocks" are the memory regions occupied by records for each active
    /// channels acquired during the same trigger event.
    raw_buffer,
};

record_footer_embedding get_record_footer_embedding(ats_board_type board_type,
                                                    bool fifo);

/// Record footers are encoded in a block of data which is sometimes larger than
/// the footer itself. This function returns the size of that block, in bytes.
size_t record_footer_block_size(ats_board_type board_type,
                                ats_data_domain data_domain);

/// Vertical resolution of digitizer models
size_t resolution_bits(ats_board_type board_type);

/// The number of bytes per sample that a given product normally generates. This
/// is the board resolution padded to the next byte boundary.
size_t default_bytes_per_sample(ats_board_type board_type);

/// Represents the location of a contiguous block of memory in a data buffer.
struct contiguous_data_block_location {
    size_t offset_bytes; //< Offset of the block from the start of the buffer
    size_t size_bytes;   //< Size of the block in bytes
};

void copy(span<const char> source, contiguous_data_block_location location,
          span<char> destination);

std::ostream &operator<<(std::ostream &os,
                         const contiguous_data_block_location &location);

/// Represents the location of a data block in a data buffer that may be split
/// across multiple memory regions.
struct data_block_location {
    /// Each of the parts is a contiguous block of memory
    std::vector<contiguous_data_block_location> parts;

    /// The total size of the data block. This is the sum of `size_bytes` of all
    /// the `parts`.
    size_t size_bytes();
};

void copy(span<const char> source, data_block_location location,
          span<char> destination);

std::ostream &operator<<(std::ostream &os, const data_block_location &location);

/// Query the location of internal record footers in buffers of data acquired
/// using a given configuration.
///
/// Each of the `data_block_location`s `b` returned by this function is such
/// that `b.size_bytes() == sizeof(ats_footer_internal)`.
std::vector<data_block_location>
get_internal_footer_locations(ats_footer_configuration configuration,
                              size_t footer_count);

void parse_internal_footers(span<char> data,
                            std::vector<data_block_location> locations,
                            span<ats_footer_internal> destination);

#endif /* ATSFOOTERS_INTERNAL_H */
