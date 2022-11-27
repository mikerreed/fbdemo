/*
 *  Copyright Pentrek Inc, 2022
 */

#ifndef _pentrek_utf_h_
#define _pentrek_utf_h_

#include "include/pentrek_types.h"
#include <string>

namespace pentrek {

extern std::string string_printf(const char format[], ...);

class UTF {
public:
    static bool isSurrogate(unsigned c) {
        assert((uint16_t)c == c);
        // surrogates are D8 and DC
        //    1101 10   and  1101 11
        // so our mask is 1111 1, looking for 1101 1
        return (c & 0xF800) == 0xD800;
    }
    
    static bool isHiSurrogate(unsigned c) {
        return (c & 0xFC00) == 0xD800;
    }
    
    static bool isLoSurrogate(unsigned c) {
        return (c & 0xFC00) == 0xDC00;
    }
    
    // if .first == 0 then
    //     .second is the answer
    // else
    //     .first + .second become "hi" and "lo" of the answer
    static std::pair<uint16_t, uint16_t> toU16(uint32_t);

    static void Tests();
};
    
} // namespace

#endif
