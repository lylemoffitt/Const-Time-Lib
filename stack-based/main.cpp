//
//  main.cpp
//  stack-based
//
//  Created by Lyle Moffitt on 1/16/17.
//  Copyright © 2017 Lyle Moffitt. All rights reserved.
//

#include <iostream>
#include <cassert>

//#include "ring_buffer.h"



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
        assert(begin() <= ptr && ptr < end());
        if(++ptr == end()){ ptr = begin(); }
        return ptr;
    }
    inline
    data_type * ptr_next( const data_ptr_type & ptr ) const{
        assert(begin() <= ptr && ptr < end());
        data_type * ret_ptr = ptr+1;
        if(ret_ptr == end()){ ret_ptr = begin(); }
        return ret_ptr;
    }
    inline
    data_type * ptr_wrap( const data_ptr_type & ptr ) const{
        assert(begin() <= ptr && ptr < end());
        return begin() + ( ptr_offset(ptr) % size() );
    }

    inline
    size_type ptr_offset( const data_ptr_type & ptr ) const{
        assert(begin() <= ptr && ptr < end());
        return ptr - begin();
    }

    ring_buffer(const data_type * b_ptr , const data_type * e_ptr   ,
                data_type * r_ptr       , data_type * w_ptr         ):
    data_begin(b_ptr),
    data_end(e_ptr),
    number(e_ptr-b_ptr),
    read_ptr(r_ptr),
    write_ptr(w_ptr)
    {
        assert(data_end>data_begin
               && "data_end before data_begin");
        assert(data_begin < read_ptr && read_ptr < data_end
               && "read_ptr not in bounds");
        assert(data_begin < write_ptr && write_ptr < data_end
               && "write_ptr not in bounds");
    }

    inline
    self_type copy(data_type* r_ptr, data_type* w_ptr){
        return ring_buffer(data_begin,data_end,r_ptr,w_ptr);
    }

    inline
    void swap(data_type & lhs, data_type & rhs){
        data_type temp = lhs;
        lhs = rhs;
        rhs = temp;
    }
public:
    // cannot be defualt constructed
    ring_buffer() = delete;

    /** Contstruct using a pre-allocated array
     \example
        int array[20];
        ring_buffer buf(array);
     */
    template<size_type num>
    ring_buffer( data_type (&array) [num] ):
    data_begin(array),
    data_end(&array[num]),
    number(num),
    read_ptr((data_type*)&data_begin[0]),
    write_ptr((data_type*)&data_begin[0])
    {
        static_assert(num>=2,"Array must store at least 2 items");
    }

    /** Total size; same as backing array
     */
    inline
    const size_type size() const {
        return number;
    }
    /** Number of elements stored in buffer
     */
    inline
    size_type length() const{
        return ( (write_ptr+number) - read_ptr) % number;
    }
    /** The buffer is full, further writes will drop elements
     */
    inline
    bool is_full()const{
        return length() == size()-1;
    }
    /** The buffer is empty
     */
    inline
    bool is_empty()const{
        return read_ptr == write_ptr;
    }
    /** Add (push) data to end of buffer; drop last if full
     */
    inline
    data_type & write(const data_type & dat){
        assert( write_ptr!=end() );
        data_type & ret = *write_ptr = dat;
        if(ptr_incr(write_ptr) == read_ptr){ ptr_incr(read_ptr); }
        return ret;
    }
    /** Read (pop) data from buffer; destructive
     */
    inline
    data_type & read(){
        assert( read_ptr!=end() );
        data_type & ret = *read_ptr;
        ptr_incr(read_ptr);
        return ret;
    }
    /** Check last element of buffer; non-destructive
     */
    inline
    data_type & peek() const{
        return *read_ptr;
    }
    /** Add data to back of buffer
     \sa write
     \param in variable to read datum from
     \example
        int data = 10;
        buffer.push(data).push(data);
     */
    inline
    self_type & push(const data_type & in){
        write(in);
        return *this;
    }
    /** Remove data from front of buffer
     \sa read
     \param out variable to read datum to
     \example
     int d1,d2 ;
     buffer.pop(d1).pop(d2);
     */
    inline
    self_type & pop(data_type & out){
        out = read();
        return *this;
    }
    /** Remove data from front of buffer (and drop it)
     */
    inline
    self_type & pop(){
        ptr_incr(read_ptr);
        return *this;
    }
    /** Call a function on each value
     \param callback 
        A function of type \code void (const data_type &) \endcode
     \example
        # Call with defined function
        void func(const int & value){ ... };
        buff.each(func);
        # Call with lambda
        buff.each([](const int & value){ ... });
     */
    template<class fn_type>
    inline
    self_type& each(fn_type callback){
        for (data_type* ptr = read_ptr; ptr!=write_ptr; ptr_incr(ptr)) {
            callback( (const data_type &) *ptr );
        }
        return *this;
    }
    /** Map a function on each value (write-back)
     \param callback
        A function of type \code data_type (const data_type &) \endcode
     \example
         # Call with defined function
         void func(const int & value){ return ... ; };
         buff.map(func);
         # Call with lambda
         buff.map([](const int & value){ return ... ; });
     */
    template<class fn_type>
    inline
    self_type& map(fn_type callback){
        for (data_type* ptr = read_ptr; ptr!=write_ptr; ptr_incr(ptr)) {
            *ptr = callback( (const data_type &) *ptr );
        }
        return *this;
    }
    /** Fill will constant value
     */
    inline
    self_type & fill(data_type value){
        while(is_full() != true){
            push(value);
        }
        return *this;
    }
    /** Fill with generated value
     \param generator
         A function of type \code data_type (void) \endcode
     \example
         # Call with defined function
         int func(){ return ... ; };
         buff.fill(func);
         # Call with lambda
         buff.fill([](){ return ... ; });
     */
    template<class fn_type>
    inline
    self_type & fill(fn_type generator){
        while(is_full() != true){
            push(generator());
        }
        return *this;
    }
    /** Remove matching values
     Pop values for which the callback returns true; stops at first false
     \param callback
        A function of type \code bool (const data_type &) \endcode
     */
    template<class fn_type>
    inline
    self_type & trim(fn_type callback){
        while ( callback(peek()) ) {
            pop();
        }
        return *this;
    }
    /** Sort contained data according to strict-weak-ordering comparator
     \param compare
        A function of type \code bool (const data_type &,const data_type &) \endcode.
        This is a function that compares two values, and returns true if the
        first should be \em before the second.
     \example
         # Call with defined function
         bool less(const int & lhs,const int & rhs){ return lhs<rhs; };
         buff.sort(less);
         # Call with lambda
         buff.sort([](const int & lhs,const int & rhs){ return lhs<rhs; });
     */
    template<class fn_type>
    inline
    self_type & sort(fn_type compare){
        for (data_type* lhs_ptr = read_ptr;
             lhs_ptr != write_ptr;
             ptr_incr(lhs_ptr))
        {
            bool already_sorted = true;
            for (data_type* rhs_ptr = ptr_next(lhs_ptr);
                 rhs_ptr != write_ptr;
                 ptr_incr(rhs_ptr))
            {
                if( not compare(*lhs_ptr,*rhs_ptr) ){
                    swap(*lhs_ptr,*rhs_ptr);
                    already_sorted = false;
                }
            }
            if( already_sorted ){
                break;
            }
        }
        return *this;
    }

    /** Align ring with backing array
     Moves data so that the indexes of the elements match the indexes of the 
     array they are stored in.
     */
    inline
    self_type & align(){
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
    /** Shallow duplicate
     \warning Modifying the buffer from this copy may result in dropped data.
     */
    inline
    self_type dup(){
        return copy(read_ptr, write_ptr);
    }
    /** Buffer of non-valid data
     \example
     \code
                 ^\pop             push\v
     Data [ 4] = [ (1 _  2 _  3 _  4)_ :0:,  0 ,  0 ,  0 ,  0 , -1  ]
                    \_ valid data _/    \_     extent data      _/
     \endcode
     */
    inline
    self_type extent(){
        return copy(write_ptr,read_ptr);
    }
    /** Make the buffer empty
     */
    inline
    self_type & clear(){
        read_ptr = write_ptr = begin();
        return *this;
    }
};

/** Make a ring buffer
 \param array Staticly sized array, e.g. \c int[10]
 Utility function to generate a \c ring_buffer , deduced from the parameter
 info.
 \example 
    int data [20]
    auto ring = make_ring(data);
 */
template<class data_t, uint16_t count>
auto make_ring(data_t (&array) [count] )->ring_buffer<data_t>
{
    return ring_buffer<data_t> (array);
}

/* -------------------------------------------------------------------------- */
/*  End of library. Below here is testing code.
 */
/* -------------------------------------------------------------------------- */

#define TEST_SIZE 10
//#define TEST_RANDOM
#define TEST_REPEAT 1

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
#ifdef TEST_RANDOM
        buff.push(rand()%TEST_SIZE-rand()%TEST_SIZE).pop();
#else
        buff.push(-i).pop();
#endif
    }
    for(int j=1; j<=len; ++j){
#ifdef TEST_RANDOM
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
        gen_buffer<TEST_SIZE>(off, len, [&]( int (data)[], ring_buffer<int> & buff){
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

    for(int repeats = TEST_REPEAT; repeats!=0; repeats--)
#ifdef TEST_RANDOM
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
#ifdef TEST_RANDOM
        int off = rand()%TEST_SIZE, len = rand()%TEST_SIZE;
#else
        for(int off = 0, len = 0; off<TEST_SIZE; (++len==TEST_SIZE)?(len=0,++off):(0))
#endif
        gen_buffer<TEST_SIZE>(off, len, [&]( int (data)[], ring_buffer<int> & buff){
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
