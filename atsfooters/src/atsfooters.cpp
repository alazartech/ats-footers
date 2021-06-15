#include "atsfooters.hpp"

#include <iostream>
#include <vector>

#include "atsfooters_internal.hpp"

ats_footer_type get_ats_footer_type(ats_board_type board_type) {
    switch (board_type) {
    case ats_board_type::ats850:
    case ats_board_type::ats310:
    case ats_board_type::ats330:
    case ats_board_type::ats855:
    case ats_board_type::ats315:
    case ats_board_type::ats335:
    case ats_board_type::ats460:
    case ats_board_type::ats860:
    case ats_board_type::ats660:
    case ats_board_type::ats665:
    case ats_board_type::ats9462:
    case ats_board_type::ats9434:
    case ats_board_type::ats9870:
    case ats_board_type::ats9350:
    case ats_board_type::ats9325:
    case ats_board_type::ats9440:
    case ats_board_type::ats9410:
    case ats_board_type::ats9351:
    case ats_board_type::ats9310:
    case ats_board_type::ats9461:
    case ats_board_type::ats9850:
    case ats_board_type::ats9625:
    case ats_board_type::atg6500:
    case ats_board_type::ats9626:
    case ats_board_type::ats9360:
    case ats_board_type::axi9870:
    case ats_board_type::ats9370:
    case ats_board_type::atu7825:
    case ats_board_type::ats9373:
    case ats_board_type::ats9416:
    case ats_board_type::ats9637:
    case ats_board_type::ats9120:
    case ats_board_type::ats9371:
    case ats_board_type::ats9130:
        return ats_footer_type::type_0;
    case ats_board_type::ats9352:
        return ats_footer_type::type_1;
    case ats_board_type::ats9453:
    case ats_board_type::ats9146:
    case ats_board_type::ats9000:
    case ats_board_type::atst371:
    case ats_board_type::ats9437:
    case ats_board_type::ats9618:
    case ats_board_type::ats9358:
    case ats_board_type::forest:
        return ats_footer_type::type_0;
    case ats_board_type::ats9353:
        return ats_footer_type::type_1;
    case ats_board_type::ats9872:
    case ats_board_type::ats9470:
    case ats_board_type::ats9628:
        return ats_footer_type::type_0;
    default:
        throw std::runtime_error("Error: invalid board type");
    }
}

void ats_parse_footers(span<char> data, ats_footer_configuration configuration,
                       span<ats_footer_type_0> footers) {
    // This is a "scratchpad". In order to avoid the overhead of memory
    // allocation and deallocation, the variable is made static. It is
    // thread-local to avoid race conditions.
    static thread_local std::vector<ats_footer_internal> internals;
    internals.resize(footers.size());

    const auto locs
        = get_internal_footer_locations(configuration, footers.size());
    parse_internal_footers(data, locs,
                           span(internals.data(), internals.size()));
    for (size_t i = 0; i < footers.size(); i++)
        parse_footer(&internals[i], &footers[i]);
}

void ats_parse_footers(span<char> data, ats_footer_configuration configuration,
                       span<ats_footer_type_1> footers) {
    // This is a "scratchpad". In order to avoid the overhead of memory
    // allocation and deallocation, the variable is made static. It is
    // thread-local to avoid race conditions.
    static thread_local std::vector<ats_footer_internal> internals;
    internals.resize(footers.size());

    const auto locs
        = get_internal_footer_locations(configuration, footers.size());
    parse_internal_footers(data, locs,
                           span(internals.data(), internals.size()));
    for (size_t i = 0; i < footers.size(); i++)
        parse_footer(&internals[i], &footers[i]);
}

int c_ats_parse_footers_type_0(char *data, size_t data_size_bytes,
                               ats_footer_configuration configuration,
                               ats_footer_type_0 *footers, size_t footer_count,
                               char *error_message,
                               size_t error_message_max_size) {
    try {
        ats_parse_footers(span<char>((char *)data, data_size_bytes),
                          configuration,
                          span<ats_footer_type_0>(footers, footer_count));

        return 0;
    } catch (const std::exception &e) {
        if (error_message) {
            strncpy(error_message, e.what(), error_message_max_size);
        }
        return -1;
    }
}

int c_ats_parse_footers_type_1(char *data, size_t data_size_bytes,
                               ats_footer_configuration configuration,
                               ats_footer_type_1 *footers, size_t footer_count,
                               char *error_message,
                               size_t error_message_max_size) {
    try {
        ats_parse_footers(span<char>((char *)data, data_size_bytes),
                          configuration,
                          span<ats_footer_type_1>(footers, footer_count));
        return 0;
    } catch (const std::exception &e) {
        if (error_message) {
            strncpy(error_message, e.what(), error_message_max_size);
        }
        return -1;
    }
}
