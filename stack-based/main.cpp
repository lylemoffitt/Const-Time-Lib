//
//  main.cpp
//  stack-based
//
//  Created by Lyle Moffitt on 1/16/17.
//  Copyright © 2017 Lyle Moffitt. All rights reserved.
//

#include <iostream>

#define TESTING
#include "ring_buffer.h"

#define TEST_SIZE 10
#define TEST_RANDOM_DATA    // vs Linear Data
//#define TEST_RANDOM_TRIALS  // vs Linear (quadratic) Trials
#define TEST_REPEAT 1

void print(const int & val){
    printf("%2d, ",val);
}
void pad(const int & val){
    printf("%2c  ",' ');
}

template<size_t sz,class fn>
void gen_buffer (int off, int len, fn call){
    int data[sz];
    for(size_t i=0; i<sz; ++i){
        data[i]=0;
    }
    auto buff = make_ring(data);

    for(int i=1; i<=off; ++i){
#ifdef TEST_RANDOM_DATA
        buff.push(rand()%TEST_SIZE-rand()%TEST_SIZE).pop();
#else
        buff.push(-i).pop();
#endif
    }
    for(int j=1; j<=len; ++j){
#ifdef TEST_RANDOM_DATA
        buff.push(rand()%TEST_SIZE);
#else
        buff.push(j);
#endif
    }
    call( data, (ring_buffer<int>&)buff );
};



int main(int argc, const char * argv[]) {
    srand((unsigned)time(nullptr));

    // create a fixed size array
    int raw[10];

    ring_buffer<int> ring(raw);
//    auto ring = make_ring(raw); // This works too

    int temp = 0;

    printf("\n%16s:\t","Init buffer");
    ring
    .fill(0)                    //init all with 0
    .push(1).push(2).push(3)    //write in 1,2,3 & drop first 0,0,0
    .each(print)
    .extent().each(pad).extent()
    .print_data();

    printf("\n%16s:\t","Align data");
    ring
    .align()
    .each(print)
    .extent().each(pad).extent()
    .print_data();

    printf("\n%16s:\t","Pop-Pop");
    ring
    .pop().pop()
    .each(print)
    .extent().each(pad).extent()
    .print_data();

    printf("\n%16s:\t","Trim zeros");
    ring
    .trim([](const int & val){ return val==0; })
    .each(print)
    .extent().each(pad).extent()
    .print_data();

    printf("\n%16s:\t","Double each");
    ring
    .push(4)
    .map([=](const int & val){ return val*2; })
    .each(print)
    .extent().each(pad).extent()
    .print_data();

    printf("\n%16s:\t","Juggle first");
    ring
    .pop(temp)      //pop into a temp variable
    .push(temp)     //push temp to end of buffer
    .each(print)
    .extent().each(pad).extent()
    .print_data();

    printf("\n%16s:\t","Fill odds");
    temp = 2;
    ring
    .fill([&](){ return ++temp%2 ? temp : ++temp; })
    .each(print)
    .extent().each(pad).extent()
    .print_data();


    auto alias_test = [](int off, int len, bool do_align){
        gen_buffer<TEST_SIZE>(off, len, [&]( int (data)[], ring_buffer<int> & buff){
            if( do_align ){
                printf("\n%11s[%d,%d]:\t","Aligned",off,len);
                buff
                .align()
                .each(print)
                .extent().each(pad).extent()
                .print_data();

            }else{
                printf("\n%11s[%d,%d]:\t","Original",off,len);
                buff
                .each(print)
                .extent().each(pad).extent()
                .print_data();

            }
        });
    };

    for(int repeats = TEST_REPEAT; repeats!=0; repeats--)
#ifdef TEST_RANDOM_TRIALS
    for(int once=1, off=rand()%10; once; once=0)
#else
    for(int off = 0; off<TEST_SIZE; ++off)
#endif
    {
        std::cout << std::endl;
        for(int len=0; len<TEST_SIZE; ++len){
            alias_test(off,len,false);
        }
        for(int len=0; len<TEST_SIZE; ++len){
            alias_test(off,len,true);
        }
    }

    for(int repeats = TEST_REPEAT; repeats!=0; repeats--){
#ifdef TEST_RANDOM_TRIALS
        int off = rand()%TEST_SIZE, len = rand()%TEST_SIZE;
#else
        for(int off = 0, len = 0; off<TEST_SIZE; (++len==TEST_SIZE)?(len=0,++off):(0))
#endif
        gen_buffer<TEST_SIZE>(off, len, [&]( int (data)[], ring_buffer<int> & buff){
            std::cout << std::endl;
            printf("\n%11s[%d,%d]:\t","Random buffer",off,len);
            buff
            .each(print)
            .extent().each(pad).extent()
            .print_data();

            printf("\n%11s[%d,%d]:\t","Sorted buffer",off,len);
            buff
            .sort([](const int & lhs, const int & rhs){
                return lhs<rhs;
//                return abs(lhs)<abs(rhs);
            })
            .each(print)
            .extent().each(pad).extent()
            .print_data();
      });
    }


    std::cout << std::endl;
    return 0;
}
