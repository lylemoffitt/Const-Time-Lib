//
//  main.cpp
//  const-function
//
//  Created by Lyle Moffitt on 12/16/14.
//  Copyright (c) 2014 Lyle Moffitt. All rights reserved.
//

#include <iostream>
#include <functional>
#include "LL_const-function.h"

int main(int argc, const char * argv[])
{
    int var;
    switch (var) {
        case std::bit_or<int>()(2,0): break;
        case std::initializer_list<int>({2,3}).begin()[1]: break;
            
        case va_funct<std::plus<int>>()(1e4,3,4,5,'a'): break;
            
        default:
            break;
    }

    // insert code here...
    std::cout << "Hello, World!\n";
    return 0;
}

