//
//  main.cpp
//  const-string
//
//  Created by Lyle Moffitt on 12/16/14.
//  Copyright (c) 2014 Lyle Moffitt. All rights reserved.
//

//#define MURMER_DEV //Uncomment to try murmer hash [WIP]

#include "const-string.h"
#include <iostream>
#include <string>


int main(int argc, const char * argv[])
{

    using namespace LL;

    std::string str = "hello";

#ifdef MURMER_DEV
    std::hash<std::string> hs;
    switch (hs(str)) {
#else
    switch( recursive_hash(str.c_str(), str.length()) ){
#endif
        case "hello"_hash:
            std::cout << "MATCH\n";
            break;
            
        default:
            std::cout << "FAIL\n";
            break;
    }
    
    return 0;
}

