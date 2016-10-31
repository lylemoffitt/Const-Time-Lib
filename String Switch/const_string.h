//
//  const_string.h
//  CNC
//
//  Created by Lyle Moffitt on 3/7/15.
//  Copyright (c) 2015 Lyle Moffitt. All rights reserved.
//

#ifndef CNC_const_string_h
#define CNC_const_string_h


#include <cstdio>
#include <functional>
#include <string>
#include <utility>

// literal class
//template<class Type, Type...Tps>
template<class ...>
struct const_str ;

template<char...t_chr>
struct const_str<char...>{
    
    const char data[sizeof...(t_chr)];
    
    constexpr  inline
    const_str() : data({t_chr...}) {}
    
    constexpr inline
    std::size_t size() const { return sizeof...(t_chr); }
    
    // constexpr functions signal errors by throwing exceptions from operator ?:
    constexpr inline
    char
    operator[](std::size_t n) const {
        return n < size() ? data[n] : throw std::out_of_range("");
    }
 
    
    constexpr inline
    bool operator==(const const_str & _cs ) const 
    {
        return (size()!=_cs.size()?false: true);
    }
    
    inline
    operator std::string(){
        return std::string(data);
    }
};



template<size_t N>
struct const_str<size_t>{
    
    const char data[N];
    
    constexpr  inline
    const_str(const char(&a)[N]) : data(a) {}
    
    
    constexpr inline static
    std::size_t size() 
    {   return N; 
    }
    
    constexpr inline 
    char
    operator[](std::size_t n) const 
    {   return n < size() ? data[n] : throw std::out_of_range("");
    }
    

    template<class...Tps>
    constexpr inline
    bool operator==(const const_str<Tps...> & _cs ) const 
    {   return (size()!=_cs.size()?false: true);
    }
    
    inline
    operator std::string(){
        return std::string(data);
    }
};

template<int ...> struct Sequence { };
template<int N, int ...S> struct CreateSequence : CreateSequence<N-1, N-1, S...> { };
template<int ...S> struct CreateSequence<0, S...> { typedef Sequence<S...> Type; };

template<typename... C> 
constexpr
const_str<C...>
construct_const_str(size_t _sz, C... c)
{
    return 
}
template<int N, int ...S> 
constexpr 
auto
Unpack(const char (&s)[N], Sequence<S...>) -> const_str<s[S]...>
{
    return const_str<s[S]...>();
}

inline constexpr
auto
operator "" _const(const char * val, size_t sz)
->decltype(Unpack( val, typename CreateSequence<sz>::Type()))
{  
    return Unpack( val, typename CreateSequence<sz>::Type());
}


#endif
