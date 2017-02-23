/*
 * source.hpp
 *
 *  Created on: Feb 22, 2017
 *      Author: derick
 */

#ifndef SOURCE_HPP_
#define SOURCE_HPP_

#include <cstdint>

namespace xcc {

struct source_location {
    char*                                           filename;
    uint32_t                                        line_number;
    uint32_t                                        column_number;
};

struct source_span {
    source_location                                 first;
    source_location                                 last;
};

inline void init_source_span(source_span& span, source_location& from) {
    span.first = from;
    span.last  = from;
}

inline void advance_column(source_location& loc, uint32_t len) {
    loc.column_number += len;
}

inline void advance_line(source_location& loc) {
    loc.line_number++;
    loc.column_number = 1;
}

inline void set_line(source_location& loc, uint32_t line_number) {
    loc.line_number = line_number;
}


}


#endif /* SOURCE_HPP_ */
