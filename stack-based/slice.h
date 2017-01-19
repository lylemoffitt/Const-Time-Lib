//
//  slice.h
//  LyleLib
//
//  Created by Lyle Moffitt on 1/16/17.
//  Copyright © 2017 Lyle Moffitt. All rights reserved.
//

#ifndef slice_h
#define slice_h

template<class _sz_t>
struct unsign;

template<> struct unsign<uint8_t>{
    using type = int8_t;
};
template<> struct unsign<uint16_t>{
    using type = int16_t;
};
template<> struct unsign<uint32_t>{
    using type = int32_t;
};

template<class _data_t, class _sz_t>
struct bound
{
    using data_type     = _data_t;
    using ptr_type      = data_type*;
    using size_type     = _sz_t;
    using self_type     = bound<data_type,size_type>;

    const ptr_type      lower;
    const ptr_type      upper;
    const size_type     range;

    bound() = delete;
    bound(const ptr_type low, const ptr_type high):
        lower(low),
        upper(high),
        range(high-low)
    {}
};

template<class _data_t, class _sz_t>
class bounded_pointer
{
public:
    using data_type     = _data_t;
    using size_type     = _sz_t;
    using offset_type   = typename unsign<size_type>::type;
    using data_ptr_type = data_type*;
    using bound_type    = bound<data_type, size_type>;
    using self_type     = bounded_pointer<data_type,size_type>;

private:
    const bound_type &      bnd_ref;
    data_type *             bnd_ptr;

    bounded_pointer(const bound_type & bound):
        bnd_ref(bound),
        bnd_ptr(bound.lower())
    {}

    inline
    data_type * begin() const{
        return bnd_ref.lower;
    }
    inline
    data_type * end() const{
        return bnd_ref.upper;
    }
    inline
    const size_type size() const{
        return bnd_ref.range;
    }

    inline
    self_type & set(offset_type offset){
        bnd_ptr   = begin() + ( (size()+offset) % size() );
        return *this;
    }
    inline
    self_type & move(offset_type offset){
        set( (bnd_ptr+offset) - begin() );
        return *this;
    }
public:

    inline
    self_type & next(){
        if( ++bnd_ptr == end() ){ bnd_ptr = begin(); }
    }
    inline
    self_type & read(data_type & out){
        out = *bnd_ptr;
        return *this;
    }
    inline
    self_type & write(const data_type & in){
        *bnd_ptr = in;
        return *this;
    }


    inline
    offset_type length(const self_type & other){
        return ( (other.bnd_ptr + size()) - bnd_ptr ) % size();
    }
    
};


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
        return ( (write_ptr+number) - read_ptr) % number;
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
    data_type & write(const data_type & dat){
        //        if(write_ptr == end()){ write_ptr = begin(); }
        (*write_ptr) = dat;
        data_type & ret = *write_ptr;
        if(++write_ptr == end()){ write_ptr = begin(); }
        return ret;
    }
    inline
    data_type & read(){
        //        if(read_ptr == end()){ read_ptr = begin(); }
        data_type & ret = *read_ptr;
        if(++read_ptr == end()){ read_ptr = begin(); }
        return ret;
    }
    inline
    data_type & peek(){
        //        if(read_ptr == end()){ read_ptr = begin(); }
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
    self_type & fill(data_type value){
        for (data_type* ptr = begin(); ptr!=end(); ptr++) {
            (*ptr) = value;
        }
        return *this;
    }



    class view
    {
        friend ring_buffer<_data_t>;
    public:
        using buffer_type   = ring_buffer<_data_t>;
        using self_type     = buffer_type::view;
        using data_type     = buffer_type::data_type;
        using size_type     = buffer_type::size_type;
        using offset_type   = buffer_type::offset_type;
        using data_ptr_type = buffer_type::data_ptr_type;

    private:
        const buffer_type &     buffer_ref;
        data_type *             read_ptr;
        data_type *             write_ptr;

        view(const buffer_type & buff):
        buffer_ref(buff),
        read_ptr(buff.read_ptr),
        write_ptr(buff.write_ptr)
        {}
        view(const buffer_type & buff, data_type* r_ptr, data_type* w_ptr):
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
        self_type& fill(data_type value){
            for (data_type* ptr = read_ptr; ptr!=write_ptr; ptr++) {
                if( ptr == end() ){ ptr = begin(); }
                (*ptr) = value;
            }
            return *this;
        }
    };

    inline
    view view_live(){
        return view(*this,read_ptr,write_ptr);
    }
    inline
    view view_dead(){
        return view(*this,write_ptr,read_ptr);
    }
    inline
    view view_all(){
        return view(*this,begin(),end());
    }
};


#endif /* slice_h */
