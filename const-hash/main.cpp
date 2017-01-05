//
//  main.cpp
//  const-hash
//
//  Created by Lyle Moffitt on 1/3/17.
//  Copyright © 2017 Lyle Moffitt. All rights reserved.
//

#include "const-hash.h"

#include <iostream>
#include <cassert>

using sz_t = uint32_t;

int main(int argc, const char * argv[]) {
    // insert code here...

    using namespace std;

    constexpr const uint8_t data_4[] = {0x12,0x34,0x56,0x78};
    constexpr const uint8_t data_8[] = {0x12,0x34,0x56,0x78, 0x87,0x65,0x43,0x21};

    auto std_lw_32bit = __loadword<sz_t>(data_4);
    auto my_lw_32bit = LL::load_word<sz_t>(data_4);

    auto murmer_4_byte = LL::murmer<sz_t>::calc(data_4,4);

    assert( std_lw_32bit == my_lw_32bit );
    assert( __loadword<sz_t>(data_4) == LL::load_word<sz_t>(data_4) );

    assert( __murmur2_or_cityhash<sz_t>()(data_4,4)
           == LL::murmer<sz_t>::calc(data_4,4)      );

    auto murmer_8_byte = LL::murmer<sz_t>::calc(data_8,8);

    assert( __murmur2_or_cityhash<sz_t>()(data_8,8)
           == LL::murmer<sz_t>::calc(data_8,8)      );

    std::cout << "Hello, World!\n";
    return 0;
}
