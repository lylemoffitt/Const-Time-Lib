//
//  main.cpp
//  stack-based
//
//  Created by Lyle Moffitt on 1/16/17.
//  Copyright © 2017 Lyle Moffitt. All rights reserved.
//

#include <iostream>

//#include "ring_buffer.h"

template<class type>
void swap(type & lhs, type & rhs){
    type temp = lhs;
    lhs = rhs;
    rhs = temp;
}

template<class _data_t>
class ring_buffer
{
public:
    using self_type     = ring_buffer<_data_t>;
    using data_type     = _data_t;
    using size_type     = uint16_t;
    using offset_type   = int16_t;
    using data_ptr_type = data_type*;

private:
    const data_type * data_begin;
    const data_type * data_end;

    const size_type number;

    data_type * read_ptr;
    data_type * write_ptr;

    inline
    data_type * begin() const{
        return (data_type*)data_begin;
    }
    inline
    data_type * end() const{
        return (data_type*)data_end;
    }
    inline
    data_type * ptr_incr( data_ptr_type & ptr ) const{
        if(++ptr == end()){ ptr = begin(); }
        return ptr;
    }
    inline
    data_type * ptr_next( const data_ptr_type & ptr ) const{
        data_type * ret_ptr = ptr+1;
        if(ret_ptr == end()){ ret_ptr = begin(); }
        return ret_ptr;
    }
    inline
    data_type * ptr_wrap( const data_ptr_type & ptr ) const{
        return begin() + ( ptr_offset(ptr) % size() );
    }

    inline
    size_type ptr_offset( const data_ptr_type & ptr ) const{
        return ptr - begin();
    }

    ring_buffer(const data_type * b_ptr , const data_type * e_ptr   ,
                data_type * r_ptr , data_type * w_ptr   ):
    data_begin(b_ptr),
    data_end(e_ptr),
    number(e_ptr-b_ptr),
    read_ptr(r_ptr),
    write_ptr(w_ptr)
    {}

    inline
    self_type copy(data_type* r_ptr, data_type* w_ptr){
        return ring_buffer(data_begin,data_end,r_ptr,w_ptr);
    }
public:
    // cannot be defualt constructed
    ring_buffer() = delete;

    // Contstruct using a pre-allocated array
    template<size_type _num>
    ring_buffer( data_type (&array) [_num] ):
    data_begin(array),
    data_end(&array[_num]),
    number(_num),
    read_ptr((data_type*)&data_begin[0]),
    write_ptr((data_type*)&data_begin[0])
    {}

    inline
    const size_type size() const {
        return number;
    }
    inline
    size_type length() const{
        return ( (write_ptr+number) - read_ptr) % number;
    }
    inline
    bool is_full()const{
        return length() == size()-1;
    }
    inline
    bool is_empty()const{
        return read_ptr == write_ptr;
    }

    inline
    data_type & write(const data_type & dat){
        data_type & ret = *write_ptr = dat;
        if(ptr_incr(write_ptr) == read_ptr){ ptr_incr(read_ptr); }
        return ret;
    }
    inline
    data_type & read(){
        data_type & ret = *read_ptr;
        ptr_incr(read_ptr);
        return ret;
    }
    inline
    data_type & peek() const{
        return *read_ptr;
    }

    inline
    self_type & push(const data_type & in){
        write(in);
        return *this;
    }
    inline
    self_type & pop(data_type & out){
        out = read();
        return *this;
    }
    inline
    self_type & pop(){
        ptr_incr(read_ptr);
        return *this;
    }

    template<class fn_type>
    inline
    self_type& each(fn_type callback){
        for (data_type* ptr = read_ptr; ptr!=write_ptr; ptr_incr(ptr)) {
            callback( (const data_type &) *ptr );
        }
        return *this;
    }
    template<class fn_type>
    inline
    self_type& map(fn_type callback){
        for (data_type* ptr = read_ptr; ptr!=write_ptr; ptr_incr(ptr)) {
            *ptr = callback( (const data_type &) *ptr );
        }
        return *this;
    }

    inline
    self_type & fill(data_type value){
        while(is_full() != true){
            push(value);
        }
        return *this;
    }
    template<class fn_type>
    inline
    self_type & fill(fn_type generator){
        while(is_full() != true){
            push(generator());
        }
        return *this;
    }
    template<class fn_type>
    inline
    self_type & trim(fn_type callback){
        while ( callback(peek()) ) {
            pop();
        }
        return *this;
    }
    template<class fn_type>
    inline
    self_type & sort(fn_type compare){
        for (data_type* lhs_ptr = read_ptr;
             lhs_ptr != write_ptr;
             ptr_incr(lhs_ptr))
        {
            for (data_type* rhs_ptr = ptr_next(lhs_ptr);
                 rhs_ptr != write_ptr;
                 ptr_incr(rhs_ptr))
            {
                if( not compare(*lhs_ptr,*rhs_ptr) ){
                    swap<data_type>(*lhs_ptr,*rhs_ptr);
                }
            }
        }
        return *this;
    }


    void sub_align (data_type* pivot, data_type* begin, data_type* end){
        //            data_type*  pivot = read_ptr;
        offset_type off_sz  = pivot-begin;
        offset_type ext_sz  = end - pivot;
        offset_type range   = end - begin;

        for(offset_type step=1; step<=off_sz; ++step){
            swap( *(pivot-step), *(end-step) );
        }
        if( range%ext_sz ){
            sub_align( begin+(range%ext_sz), begin, begin+ext_sz );
        }

    };

    inline
    self_type & align(){
//        sub_align(read_ptr, begin(), end());

        for( data_type *p_ptr=read_ptr, *b_ptr=begin(), *e_ptr=end();; )
        {
            const offset_type off_sz  = p_ptr - b_ptr;
            const offset_type ext_sz  = e_ptr - p_ptr;
            const offset_type range   = e_ptr - b_ptr;

            for(offset_type step=1; step<=off_sz; ++step){
                swap( *(p_ptr-step), *(e_ptr-step) );
            }
            if( range%ext_sz ){
                p_ptr = b_ptr+(range%ext_sz);
                b_ptr = b_ptr;
                e_ptr = b_ptr+ext_sz;
            }else{
                break;
            }
        }
        write_ptr   = begin()+length();
        read_ptr    = begin();
        return *this;
    }
    inline
    self_type dup(){
        return copy(read_ptr, write_ptr);
    }
    inline
    self_type extent(){
        return copy(write_ptr,read_ptr);
    }

    inline
    self_type & clear(){
        read_ptr = write_ptr = begin();
        return *this;
    }
};

template<class data_t, uint16_t count>
auto make_ring(data_t (&array) [count] )->ring_buffer<data_t>
{
    return ring_buffer<data_t> (array);
}


void print(const int & val){
    printf("%2d, ",val);
}

//template<uint16_t N>
void print_data(int (array) [], ring_buffer<int> & buff){
    const auto  len     = buff.length();
    const auto  pad     = (buff.size()-len)*4;
    const auto  size    = buff.size();
    const int * rd_ptr  = &buff.peek();
    const uint16_t  rd_off  = rd_ptr-array;
    const int * wr_ptr  = array + ((len+rd_off)%size) ;
    const uint16_t  wr_off  = ((len+rd_off)%size) - (wr_ptr==rd_ptr ? 0 : 1);
    const bool empty = buff.is_empty();

    int d_width = 4;
//    buff.each([&](const int& val){
//        int digits = (int)std::to_string(val).length();
//        if( digits > d_width ){ d_width++; }
//    });
    auto fmt_str = [&](uint16_t i){
        auto str = std::to_string(array[i]);
        char sep = ',';
        if( i==rd_off && !empty ){
            str.insert(0,"(");  //if(len>1){ str.append("_"); }
            sep = '_';
        }
        if( i==wr_off+1 ){
            str.insert(0,":"); str.append(":");
        }
        if( i==wr_off && !empty ){
            str.append(")");    //if(len>1){ str.insert(0,"_"); }
            sep = '_';
        }
        if( wr_off > rd_off && (i>rd_off && i<wr_off) ){
//            str.insert(0,"_"); str.append("_");
            sep = '_';
        }
        if( rd_off > wr_off && (i>rd_off || i<wr_off) ){
//            str.insert(0,"_"); str.append("_");
            sep = '_';
        }
        if( isdigit(str.back()) ){ str.append(" "); }
        int len = d_width-(int)str.length();
        if( len > 0){
            str.insert(0, len, ' ');
        }
        if( i+1 < size ){ str.append(1,sep); }
        return str;
    };
    printf("%*c Data [%2d] = [",pad,' ',len);
//    printf("\nData [%2d] = [",len);
    for(uint16_t i=0; i!=size; ++i){
        printf( "%s", fmt_str(i).c_str() );
    }
    printf(" ]");
}

template<size_t sz,class fn>
void gen_buffer (int off, int len, fn call){
    int data[sz];
    for(size_t i=0; i<sz; ++i){
        data[i]=0;
    }
    auto buff = make_ring(data);

    for(int i=1; i<=off; ++i){
        buff.push(-i).pop();
    }
    for(int j=1; j<=len; ++j){
        buff.push(j);
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
    .each(print);
    print_data(raw,ring);

    printf("\n%16s:\t","Align data");
    ring
//    .align()
    .each(print);
    print_data(raw,ring);

    printf("\n%16s:\t","Pop-Pop");
    ring
    .pop().pop()
    .each(print);
    print_data(raw,ring);

    printf("\n%16s:\t","Trim zeros");
    ring
    .trim([](const int & val){ return val==0; })
    .each(print);
    print_data(raw,ring);

    printf("\n%16s:\t","Double each");
    ring
    .push(4)
    .map([=](const int & val){ return val*2; })
    .each(print);
    print_data(raw,ring);

    printf("\n%16s:\t","Juggle first");
    ring
    .pop(temp)      //pop into a temp variable
    .push(temp)     //push temp to end of buffer
    .each(print);
    print_data(raw,ring);

    printf("\n%16s:\t","Fill odds");
    temp = 2;
    ring
    .fill([&](){ return ++temp%2 ? temp : ++temp; })
    .each(print);
    print_data(raw,ring);


    auto alias_test = [](int off, int len, bool do_align){
        gen_buffer<10>(off, len, [&]( int (data)[], ring_buffer<int> & buff){
            if( do_align ){
                printf("\n%11s[%d,%d]:\t","Aligned",off,len);
                buff
                .align()
                .each(print)
                //            .align()
                ;
                print_data(data,buff);
            }else{
                printf("\n%11s[%d,%d]:\t","Original",off,len);
                buff
                .each(print)
                ;
                print_data(data,buff);
            }
        });
    };

    for(int once=1, off=rand()%10; once; once=0){
        std::cout << std::endl;
        for(int len=0; len<10; ++len){
            alias_test(off,len,false);
        }
        for(int len=0; len<10; ++len){
            alias_test(off,len,true);
        }
    }

    for(int i=0; i<3; ++i){
        int off = rand()%10, len = rand()%10;
        gen_buffer<10>(off, len, [&]( int (data)[], ring_buffer<int> & buff){
            for( int i = rand()%20; i<10; ++i){
                if( rand()%2 ){
                    buff.push(rand()%10 - rand()%10);
                }else{
                    buff.pop();
                }
            }
            std::cout << std::endl;
            printf("\n%11s[%d,%d]:\t","Random buffer",off,len);
            buff
            .each(print)
            ;
            print_data(data,buff);
            printf("\n%11s[%d,%d]:\t","Sorted buffer",off,len);
            buff
            .sort([](const int & lhs, const int & rhs){
                return lhs<rhs;
            })
            .each(print)
            ;
            print_data(data,buff);
        });
    }


    std::cout << std::endl;
    return 0;
}
