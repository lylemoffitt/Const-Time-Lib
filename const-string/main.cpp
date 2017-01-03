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


using namespace std;
int main(int argc, const char * argv[])
{

    
    string str = "hello";
    
    hash<string> hs;
    
    switch (hs(str)) {
        case "hello"_hash:
            cout << "MATCH\n";
            break;
            
        default:
            cout << "FAIL\n";
            break;
    }
    
    return 0;
}

