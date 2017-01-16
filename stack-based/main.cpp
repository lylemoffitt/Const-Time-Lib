//
//  main.cpp
//  stack-based
//
//  Created by Lyle Moffitt on 1/16/17.
//  Copyright © 2017 Lyle Moffitt. All rights reserved.
//

#include <iostream>

template<class _data_t>
class ring_buffer
{
public:
    using self_type     = ring_buffer<_data_t>;
    using data_type     = _data_t;
    using size_type     = uint16_t;
    using offset_type   = int16_t;
    using data_ptr_type = data_type*;

    typedef void (*read_function)(data_type);
    typedef data_type (*write_function)(data_type);

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
        return ((write_ptr+number)-read_ptr)%number;
    }
    inline
    bool is_full()const{
        return length() == (size()-1);
    }
    inline
    bool is_empty()const{
        return length() == 0;
    }

    inline
    data_type & read(){
        if(read_ptr == end()){ read_ptr = begin(); }
        data_type & ret = *read_ptr;
        if(++read_ptr == end()){ read_ptr = begin(); }
        return ret;
    }
    inline
    self_type & pop(data_type & out){
        out = read();
        return *this;
    }

    inline
    data_type & write(const data_type & dat){
        if(write_ptr == end()){ write_ptr = begin(); }
        (*write_ptr) = dat;
        data_type & ret = *write_ptr;
        if(++write_ptr == end()){ write_ptr = begin(); }
        return ret;
    }
    inline
    self_type & push(const data_type & in){
        write(in);
        return *this;
    }

    inline
    self_type & set_reader(offset_type offset){
        read_ptr    = &data_begin[(number+offset)%number];
        return *this;
    }
    inline
    self_type & set_writer(offset_type offset){
        write_ptr   = &data_begin[(number+offset)%number];
        return *this;
    }
    inline
    self_type & move_reader(offset_type offset){
        set_reader( (read_ptr+offset) - begin() );
        return *this;
    }
    inline
    self_type & move_writer(offset_type offset){
        set_writer( (write_ptr+offset) - begin() );
        return *this;
    }

    inline
    self_type & memset(data_type value){
        for (data_type* ptr = begin(); ptr!=end(); ptr++) {
            (*ptr) = value;
        }
        return *this;
    }



    class cursor
    {
        friend ring_buffer<_data_t>;
    public:
        using buffer_type   = ring_buffer<_data_t>;
        using self_type     = buffer_type::cursor;
        using data_type     = buffer_type::data_type;
        using size_type     = buffer_type::size_type;
        using offset_type   = buffer_type::offset_type;
        using data_ptr_type = buffer_type::data_ptr_type;

    private:
        const buffer_type &     buffer_ref;
        data_type *             read_ptr;
        data_type *             write_ptr;

        cursor(const buffer_type & buff):
        buffer_ref(buff),
        read_ptr(buff.read_ptr),
        write_ptr(buff.write_ptr)
        {}
        cursor(const buffer_type & buff, data_type* r_ptr, data_type* w_ptr):
        buffer_ref(buff),
        read_ptr(r_ptr),
        write_ptr(w_ptr)
        {}
        inline
        data_type * begin() const{
            return buffer_ref.begin();
        }
        inline
        data_type * end() const{
            return buffer_ref.end();
        }
    public:
        inline
        self_type & pop(){
            if(++read_ptr == end()){ read_ptr = begin(); }
            return *this;
        }

        inline
        self_type & push(){
            if(++write_ptr == end()){ write_ptr = begin(); }
            return *this;
        }
        inline
        self_type& each(read_function callback){
            for (data_type* ptr = read_ptr; ptr!=write_ptr; ptr++) {
                if( ptr == end() ){ ptr = begin(); }
                (*callback)(*ptr);
            }
            return *this;
        }
        inline
        self_type& each(write_function callback){
            for (data_type* ptr = read_ptr; ptr!=write_ptr; ptr++) {
                if( ptr == end() ){ ptr = begin(); }
                (*ptr) = (*callback)(*ptr);
            }
            return *this;
        }
        inline
        self_type& memset(data_type value){
            for (data_type* ptr = read_ptr; ptr!=write_ptr; ptr++) {
                if( ptr == end() ){ ptr = begin(); }
                (*ptr) = value;
            }
            return *this;
        }
    };

    inline
    cursor view_live(){
        return cursor(*this,read_ptr,write_ptr);
    }
    inline
    cursor view_dead(){
        return cursor(*this,write_ptr,read_ptr);
    }
    inline
    cursor view_all(){
        return cursor(*this,begin(),end());
    }
};




int main(int argc, const char * argv[]) {
    int raw[10];
    ring_buffer<int> ring(raw);

    int temp;

    ring
    .memset(0)              //init all with 0
    .push(1).push(2).push(3)//write in 1,2,3
    .pop(temp);             //read out 1 into `temp`

    std::cout << "\nFull buffer\t";
    ring
    .view_all()
    .each([](int val){std::cout << val <<" ";});
    // Full buffer	1 2 3 0 0 0 0 0 0 0

    std::cout << "\nDead subset\t";
    ring
    .view_dead()
    .pop().pop()
    .each([](int val){std::cout << val <<" ";});
    // Dead subset	0 0 0 0 0 1

    std::cout << "\nLive subset\t";
    ring
    .view_live()
    .each([](int val){std::cout << val <<" ";});
    // Live subset	2 3
    
    
    std::cout << std::endl;
    return 0;
}
