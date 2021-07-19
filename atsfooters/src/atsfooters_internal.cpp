#include "atsfooters_internal.hpp"

#include <cassert>
#include <iostream>
#include <sstream>

#include "utils.hpp"

void parse_footer(const ats_footer_internal *source,
                  ats_footer_type_0 *destination) {
    if (source->type != 0) {
        std::ostringstream ostr;
        ostr << "Error: Footer type " << int(source->type)
             << " is not the value expected";
        throw std::runtime_error(ostr.str());
    }

    *destination = ats_footer_type_0{
        (uint64_t)source->tt_low + ((uint64_t)source->tt_med << 16)
            + ((uint64_t)source->tt_high << 32),
        (uint32_t)source->rn_low + (source->rn_high << 16),
        (uint32_t)source->fc_low + (source->fc_high << 16),
        (source->aux_and_pulsar_low & 0x01) ? true : false};
}

void parse_footer(const ats_footer_internal *source,
                  ats_footer_type_1 *destination) {
    if (source->type != 1) {
        std::ostringstream ostr;
        ostr << "Error: Footer type " << int(source->type)
             << " is not the value expected";
        throw std::runtime_error(ostr.str());
    }

    uint16_t rawd
        = (source->aux_and_pulsar_low & 0xF0) | (source->pulsar_high << 8);

    *destination = ats_footer_type_1{
        (uint64_t)source->tt_low + ((uint64_t)source->tt_med << 16)
            + ((uint64_t)source->tt_high << 32),
        (uint32_t)source->rn_low + (source->rn_high << 16),
        (uint32_t)source->fc_low + (source->fc_high << 16),
        (source->aux_and_pulsar_low & 0x01) ? true : false,
        *reinterpret_cast<int16_t *>(&rawd)};
}

size_t resolution_bits(ats_board_type board_type) {
    switch (board_type) {
    case ats_board_type::ats850:
    case ats_board_type::ats860:
    case ats_board_type::atu7825:
    case ats_board_type::ats9870:
    case ats_board_type::axi9870:
    case ats_board_type::ats9850:
    case ats_board_type::ats9872:
        return 8;
    case ats_board_type::ats9461:
    case ats_board_type::ats9410:
        return 10;
    case ats_board_type::ats310:
    case ats_board_type::ats330:
    case ats_board_type::ats9120:
    case ats_board_type::ats9130:
    case ats_board_type::ats9350:
    case ats_board_type::ats9310:
    case ats_board_type::ats9351:
    case ats_board_type::ats9325:
    case ats_board_type::ats9000:
    case ats_board_type::ats9352:
    case ats_board_type::ats9360:
    case ats_board_type::ats9358:
    case ats_board_type::ats9370:
    case ats_board_type::ats9371:
    case ats_board_type::ats9373:
    case ats_board_type::forest:
    case ats_board_type::ats9353:
        return 12;
    case ats_board_type::ats460:
    case ats_board_type::ats9434:
    case ats_board_type::ats9440:
    case ats_board_type::ats9416:
    case ats_board_type::ats9437:
    case ats_board_type::ats9453:
    case ats_board_type::ats9146:
    case ats_board_type::ats9470:
        return 14;
    case ats_board_type::ats660:
    case ats_board_type::ats9462:
    case ats_board_type::ats9625:
    case ats_board_type::ats9626:
    case ats_board_type::ats9637:
    case ats_board_type::ats9618:
    case ats_board_type::ats9628:
        return 16;
    default:
        /// Invalid board type
        assert(false);
    }
    return 0;
}

size_t default_bytes_per_sample(ats_board_type board_type) {
    return (resolution_bits(board_type) + 7) / 8;
}

size_t record_footer_block_size(ats_board_type board_type,
                                ats_data_domain data_domain) {
    switch (data_domain) {
    case ats_data_domain::time:
        return 16;
    case ats_data_domain::frequency:
        switch (board_type) {
        case ats_board_type::ats9350:
        case ats_board_type::ats9351:
            return 64;
        case ats_board_type::ats9352:
        case ats_board_type::ats9353:
            return 32;
        default:
            return 128;
        }
    default:
        std::ostringstream sstr;
        sstr << "Data domain " << static_cast<int>(data_domain)
             << " is invalid";
        throw std::runtime_error(sstr.str());
    }
}

record_footer_embedding get_record_footer_embedding(ats_board_type board_type,
                                                    bool fifo) {
    switch (board_type) {
    case ats_board_type::ats9130:
    case ats_board_type::ats9416:
        return record_footer_embedding::raw_buffer;
    case ats_board_type::ats9352:
    case ats_board_type::ats9353:
        return fifo ? record_footer_embedding::raw_buffer
                    : record_footer_embedding::channel_data_one_per_channel;
    default:
        return record_footer_embedding::channel_data_shared;
    }
}

std::ostream &operator<<(std::ostream &os,
                         const contiguous_data_block_location &location) {
    os << "{offset: " << location.offset_bytes
       << ", size: " << location.size_bytes << "}";
    return os;
}

void copy(span<char> source, contiguous_data_block_location location,
          span<char> destination) {
    assert(source.size() >= location.offset_bytes + location.size_bytes);
    std::copy(source.data() + location.offset_bytes,
              source.data() + location.offset_bytes + location.size_bytes,
              destination.data());
}

std::ostream &operator<<(std::ostream &os,
                         const data_block_location &location) {
    bool first = true;
    os << "{";
    for (auto part : location.parts) {
        if (first)
            first = false;
        else
            os << ", ";
        os << part;
    }
    os << "}";
    return os;
}

void copy(span<char> source, data_block_location location,
          span<char> destination) {
    size_t offset = 0;
    for (auto part : location.parts) {
        copy(source, part, destination.subspan(offset));
        offset += part.size_bytes;
    }
}

std::vector<data_block_location>
get_internal_footer_locations(ats_footer_configuration configuration,
                              size_t footer_count) {
    const size_t footer_block_size_bytes = record_footer_block_size(
        configuration.board_type, configuration.data_domain);

    const size_t bytes_per_sample
        = default_bytes_per_sample(configuration.board_type);

    if (!configuration.bytes_per_record_per_channel)
        throw std::runtime_error("Error: record size is 0");

    if (!configuration.records_per_buffer_per_channel)
        throw std::runtime_error("Error: records per buffer is 0");

    if (!configuration.active_channel_count)
        throw std::runtime_error("Error: active channel count is 0");

    const size_t samples_per_record
        = configuration.bytes_per_record_per_channel / bytes_per_sample;
    const auto records_per_buffer
        = configuration.records_per_buffer_per_channel;
    const auto record_size_bytes = configuration.bytes_per_record_per_channel;
    const auto active_channel_count = configuration.active_channel_count;

    long long sample_stride_bytes = bytes_per_sample;
    long long channel_stride_bytes = bytes_per_sample;
    long long record_stride_bytes = record_size_bytes;
    long long buffer_stride_bytes
        = record_size_bytes * records_per_buffer * active_channel_count;
    if (active_channel_count > 1) {
        switch (configuration.data_layout) {
        case ats_data_layout::sample_interleaved:
            sample_stride_bytes = active_channel_count * bytes_per_sample;
            channel_stride_bytes = bytes_per_sample;
            record_stride_bytes = record_size_bytes * active_channel_count;
            break;
        case ats_data_layout::record_interleaved:
            sample_stride_bytes = bytes_per_sample;
            channel_stride_bytes = record_size_bytes;
            record_stride_bytes = record_size_bytes * active_channel_count;
            break;
        case ats_data_layout::buffer_interleaved:
            sample_stride_bytes = bytes_per_sample;
            channel_stride_bytes = records_per_buffer * record_size_bytes;
            record_stride_bytes = record_size_bytes;
            break;
        default:
            std::ostringstream sstr;
            sstr << "Error: data layout "
                 << static_cast<int>(configuration.data_layout) << " invalid";
            throw std::runtime_error(sstr.str());
        }
    }

    const auto embedding = get_record_footer_embedding(configuration.board_type,
                                                       configuration.fifo);

    if (!record_size_bytes)
        throw std::runtime_error("Error: Record size is 0");

    const size_t footer_block_size_samples_per_channel
        = footer_block_size_bytes / active_channel_count / bytes_per_sample;
    const size_t footer_block_size_samples
        = footer_block_size_bytes / bytes_per_sample;
    const int bytes_per_footer = 16;
    const int samples_per_footer
        = static_cast<int>(bytes_per_footer / bytes_per_sample);
    std::vector<data_block_location> output(footer_count);
    for (size_t footer = 0; footer < footer_count; footer++) {
        const size_t r = footer % records_per_buffer;
        const size_t buf = footer / records_per_buffer;

        switch (embedding) {
        case record_footer_embedding::channel_data_shared:
            for (size_t c = 0; c < active_channel_count; c++) {
                for (size_t s = 0;
                     s < samples_per_footer / active_channel_count; s++) {
                    const size_t source_offset
                        = (size_t)(buf * buffer_stride_bytes
                                   + r * record_stride_bytes
                                   + c * channel_stride_bytes
                                   + (samples_per_record
                                      - footer_block_size_samples_per_channel
                                      + s)
                                         * sample_stride_bytes);
                    output[footer].parts.push_back({
                        source_offset,
                        bytes_per_sample,
                    });
                }
            }
            break;
        case record_footer_embedding::raw_buffer:
            output[footer].parts.push_back(
                {buf * buffer_stride_bytes
                     + (r + 1) * record_size_bytes * active_channel_count
                     - footer_block_size_bytes,
                 16});
            break;
        case record_footer_embedding::channel_data_one_per_channel:
            for (size_t s = 0; s < samples_per_footer; s++) {
                const size_t source_offset
                    = (size_t)(buf * buffer_stride_bytes
                               + r * record_stride_bytes
                               + (samples_per_record - footer_block_size_samples
                                  + s)
                                     * sample_stride_bytes);
                output[footer].parts.push_back(
                    {source_offset, bytes_per_sample});
            }
        }
    }

    return output;
}

void parse_internal_footers(span<char> data,
                            std::vector<data_block_location> locations,
                            span<ats_footer_internal> destinations) {
    if (!data.size())
        throw std::runtime_error("Error: data buffer size is 0");

    if (!data.data())
        throw std::runtime_error("Error: NULL data buffer");

    if (locations.size() != destinations.size())
        throw std::runtime_error("Error: locations and destination internal "
                                 "footers array sizes differ");

    for (size_t i = 0; i < locations.size(); i++)
        copy(data, locations[i], as_byte_span(&destinations[i]));
}
