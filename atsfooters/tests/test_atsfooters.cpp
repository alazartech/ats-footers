#include "atsfooters.hpp"

#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <vector>

#include "utils.hpp"

void check_record_numbers(std::vector<uint32_t> rec_nums) {
    for (size_t i = 0; i < rec_nums.size(); i++) {
        const size_t actual = rec_nums[i];
        const size_t expected = (i + 1);
        if (actual != expected) {
            std::ostringstream ostr;
            ostr << "Error: footer " << i << " has record number " << actual
                 << " instead of " << expected;
            throw std::runtime_error(ostr.str());
        }
    }
}

void check_timestamps(std::vector<uint64_t> timestamps,
                      uint64_t expected_ticks_per_trigger) {
    const uint64_t tolerance = expected_ticks_per_trigger / 20; // 5% tolerance
    if (!timestamps.size())
        return;
    if (timestamps[0] > expected_ticks_per_trigger + tolerance) {
        std::ostringstream ostr;
        ostr << "Error: first footer has timestamp " << timestamps[0]
             << " which is higher than the expected "
             << expected_ticks_per_trigger << ". Timestamps: " << timestamps;
        throw std::runtime_error(ostr.str());
    }

    const uint64_t max_distance = expected_ticks_per_trigger + tolerance;
    const uint64_t min_distance = expected_ticks_per_trigger - tolerance;
    for (int i = 1; i < timestamps.size(); i++) {
        if (timestamps[i - 1] > timestamps[i]) {
            std::ostringstream ostr;
            ostr << "Error: timestamp of footer " << i - 1 << " ("
                 << timestamps[i - 1] << ") is higher than the next"
                 << ". Timestamps: " << timestamps;
            throw std::runtime_error(ostr.str());
        }
        const uint64_t actual = timestamps[i] - timestamps[i - 1];

        if (actual < min_distance) {
            std::ostringstream ostr;
            ostr << "Error: timestamp difference between footer " << i - 1
                 << " and the next (" << actual << ") is less than minimum "
                 << min_distance << ". Timestamps: " << timestamps;
            throw std::runtime_error(ostr.str());
        }
        if (actual > max_distance) {
            std::ostringstream ostr;
            ostr << "Error: timestamp difference between footer " << i - 1
                 << " and the next (" << actual << ") is more than maximum "
                 << max_distance << ". Timestamps: " << timestamps;
            throw std::runtime_error(ostr.str());
        }
    }
}

std::vector<uint32_t> record_numbers(span<ats_footer_type_0> footers) {
    std::vector<uint32_t> out;
    for (const auto &f : footers)
        out.push_back(f.record_number);
    return out;
}

std::vector<uint64_t> trigger_timestamps(span<ats_footer_type_0> footers) {
    std::vector<uint64_t> out;
    for (const auto &f : footers)
        out.push_back(f.trigger_timestamp);
    return out;
}

std::vector<uint32_t> record_numbers(span<ats_footer_type_1> footers) {
    std::vector<uint32_t> out;
    for (const auto &f : footers)
        out.push_back(f.record_number);
    return out;
}

std::vector<uint64_t> trigger_timestamps(span<ats_footer_type_1> footers) {
    std::vector<uint64_t> out;
    for (const auto &f : footers)
        out.push_back(f.trigger_timestamp);
    return out;
}

struct footer_data_file_config {
    std::string filename;
    ats_footer_configuration config;
    size_t buffers_per_acquisition;
    double expected_ticks_per_trigger;
};

void check_data_file(footer_data_file_config config) {
    try {
        std::cout << "Checking data file " << config.filename << "\n";
        std::ifstream stream{config.filename, std::ios::binary};
        if (!stream)
            throw std::runtime_error("Could not open file.");
        std::vector<char> contents{std::istreambuf_iterator<char>(stream), {}};
        span<char> data{(char *)contents.data(), contents.size()};
        switch (get_ats_footer_type(config.config.board_type)) {
        case ats_footer_type::type_0: {
            std::vector<ats_footer_type_0> footers(
                config.config.records_per_buffer_per_channel
                * config.buffers_per_acquisition);
            ats_parse_footers(data, config.config,
                              span(footers.data(), footers.size()));
            check_record_numbers(
                record_numbers(span(footers.data(), footers.size())));
            check_timestamps(
                trigger_timestamps(span(footers.data(), footers.size())),
                static_cast<uint64_t>(config.expected_ticks_per_trigger));
            break;
        }
        case ats_footer_type::type_1: {
            std::vector<ats_footer_type_1> footers(
                config.config.records_per_buffer_per_channel
                * config.buffers_per_acquisition);
            ats_parse_footers(data, config.config,
                              span(footers.data(), footers.size()));
            check_record_numbers(
                record_numbers(span(footers.data(), footers.size())));
            check_timestamps(
                trigger_timestamps(span(footers.data(), footers.size())),
                static_cast<uint64_t>(config.expected_ticks_per_trigger));
            break;
        }
        default:
            throw std::runtime_error("Invalid footer type");
        }
    } catch (const std::exception &e) {
        std::cerr << "Error while parsing data file " << config.filename << ". "
                  << e.what() << "\n";
        throw;
    }
}

// clang-format off
static const footer_data_file_config footer_data_file_configs[] = {
    // filename                      | board type             | data domain               | ch. count | data layout                        | bytes/rec | rec/buf | fifo   | buf/acq. | ticks per trig |
    {"data-ats9373-1ch-fifo.bin",     {ats_board_type::ats9373, ats_data_domain::time,      1,          ats_data_layout::sample_interleaved, 2048 * 2  , 2       , false }, 2        , 2e6            },
    {"data-ats9373-2ch-fifo.bin",     {ats_board_type::ats9373, ats_data_domain::time,      2,          ats_data_layout::sample_interleaved, 2048 * 2  , 2       , false }, 2        , 2e6            },
    {"data-ats9373-fft-1ch-2176.bin", {ats_board_type::ats9373, ats_data_domain::frequency, 1,          ats_data_layout::sample_interleaved, 2176      , 2       , false }, 2        , 2e6            },
    {"data-ats9130-1ch-fifo.bin",     {ats_board_type::ats9130, ats_data_domain::time,      1,          ats_data_layout::sample_interleaved, 2048 * 2  , 2       , true  }, 2        , 50e3           },
    {"data-ats9130-2ch-fifo.bin",     {ats_board_type::ats9130, ats_data_domain::time,      2,          ats_data_layout::sample_interleaved, 2048 * 2  , 2       , true  }, 2        , 50e3           },
    {"data-ats9350-1ch.bin",          {ats_board_type::ats9350, ats_data_domain::time,      1,          ats_data_layout::record_interleaved, 2048 * 2  , 2       , false }, 2        , 500e3          },
    {"data-ats9350-2ch.bin",          {ats_board_type::ats9350, ats_data_domain::time,      2,          ats_data_layout::buffer_interleaved, 2048 * 2  , 2       , false }, 2        , 500e3          },
    {"data-ats9350-fft-1ch-2112.bin", {ats_board_type::ats9350, ats_data_domain::frequency, 1,          ats_data_layout::sample_interleaved, 2112      , 2       , false }, 2        , 500e3          },
    {"data-ats9352-2ch.bin",          {ats_board_type::ats9352, ats_data_domain::time,      2,          ats_data_layout::buffer_interleaved, 2048 * 2  , 2       , false }, 2        , 500e3          },
    {"data-ats9352-2ch-fifo.bin",     {ats_board_type::ats9352, ats_data_domain::time,      2,          ats_data_layout::sample_interleaved, 2048 * 2  , 2       , true  }, 2        , 500e3          },
    {"data-ats9352-fft-2ch-4128.bin", {ats_board_type::ats9352, ats_data_domain::frequency, 2,          ats_data_layout::record_interleaved, 4128      , 2       , false }, 2        , 2e3            },
    {"data-ats9146-1ch-2048spr.bin",  {ats_board_type::ats9146, ats_data_domain::time,      1,          ats_data_layout::buffer_interleaved, 2048 * 2  , 10      , false }, 10       , 2.5e3          },
    {"data-ats9146-2ch-2048spr.bin",  {ats_board_type::ats9146, ats_data_domain::time,      2,          ats_data_layout::buffer_interleaved, 2048 * 2  , 10      , false }, 10       , 2.5e3          },
    {"data-ats9872-2ch.bin",          {ats_board_type::ats9872, ats_data_domain::time,      2,          ats_data_layout::buffer_interleaved, 2048      , 10      , false }, 10       , 500e3          },
    {"data-ats9872-2ch-intlvd.bin",   {ats_board_type::ats9872, ats_data_domain::time,      2,          ats_data_layout::sample_interleaved, 2048      , 10      , false }, 10       , 500e3          },
};
// clang-format on

int main() {
    try {
        for (auto config : footer_data_file_configs) {
            check_data_file(config);
        }
    } catch (const std::exception &e) {
        std::cerr << "test_atsfooters error: " << e.what();
        return -1;
    }
}
