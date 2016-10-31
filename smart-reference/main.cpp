//
//  main.cpp
//  smart-reference
//
//  Created by Lyle Moffitt on 1/6/15.
//  Copyright (c) 2015 Lyle Moffitt. All rights reserved.
//

#include <iostream>
#include "LL_smart-reference.h"

#include <string>

struct move_only
{
    move_only & operator=(const move_only&) = delete;
    move_only & operator=(move_only&&)      = default;
};

struct normal
{
};

struct test
{
    shared<move_only>   s_obj;
    shared<normal>      s_norm;
};


int main(int argc, const char * argv[])
{
    test t1,t2;
    t1=t2;
     
    std::shared_ptr<std::string>  s_str;
    
    t1.s_norm.
    
    // insert code here...
    std::cout << "Hello, World!\n";
    return 0;
}

