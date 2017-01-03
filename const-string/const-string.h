//
//  const-string_h.h
//  LyleLib
//
//  Created by Lyle Moffitt on 12/16/14.
//  Copyright (c) 2014 Lyle Moffitt. All rights reserved.
//

#ifndef LL_const_string_h
#define LL_const_string_h

#include <cstdio>
#include <functional>
#include <string>
#include <utility>

// literal class
template<class Type, Type...Tps>
class const_str ;

template<char...t_chr>
class const_str<char,t_chr...>{
    
    const std::size_t sz;
    const char data[sizeof...(t_chr)];
public:
    
    constexpr
    const_str() : sz(sizeof...(t_chr)-1), data({t_chr...}) {}
    
    
    // constexpr functions signal errors by throwing exceptions from operator ?:
    constexpr 
    char
    operator[](std::size_t n) const {
        return n < sz ? data[n] : throw std::out_of_range("");
    }
    
    constexpr std::size_t size() const { return sz; }
    
    constexpr bool operator==(const const_str & _cs ) const 
    {
        return (sz!=_cs.sz?false: true);
    }
};



template<size_t N>
class const_str<size_t,N>{
    
    const std::size_t sz;
    const char data[N];
public:
    
    constexpr 
    const_str(const char(&a)[N]) : sz(N-1), data(a) {}
    
    constexpr 
    char
    operator[](std::size_t n) const 
    {   return n < sz ? data[n] : throw std::out_of_range("");
    }
    
    constexpr std::size_t size() const 
    {   return sz; 
    }
    
    constexpr bool operator==(const const_str & _cs ) const 
    {   return (sz!=_cs.sz?false: true);
    }
};


typedef uint8_t byte;


template <class _Size>
constexpr inline
_Size
load_word(byte B0, byte B1, byte B2, byte B3)
{
    return static_cast<_Size>((B3<<(8*3))|(B2<<(8*2))|(B1<<(8*1))|(B0<<(8*0)));
}

template <class _Size>
constexpr inline
_Size
load_word(const byte * _B_ptr)
{
//    return static_cast<const _Size>( *static_cast<const _Size*>( _B_ptr ) );
    return static_cast<_Size>(  (_B_ptr[0]<<(8*0)) 
                              | (_B_ptr[1]<<(8*1)) 
                              | (_B_ptr[2]<<(8*2)) 
                              | (_B_ptr[3]<<(8*3)) );
}

//template <class ptr_t,size_t _sz>
//ptr_t adv_ptr() {
//    return static_cast<ptr_t>
//    ( static_cast< typename std::conditional
//        <std::is_const<ptr_t>::value, const byte *, byte *>::type>
//        (
//        )+_sz
//     );
//}

template <class _sz>
struct murmer 
{
    typedef _sz _Size;
    
    constexpr const static _Size __m = 0x5bd1e995;
    constexpr const static _Size __r = 24;
    

    constexpr inline _Size        
    K0(_Size __k) const
    {   return __k *= __m; 
    }
    constexpr inline _Size        
    K1(_Size __k) const
    {   return __k ^= __k >> __r; 
    }
    constexpr inline _Size        
    K2(_Size __k) const
    {   return __k *= __m; 
    }
    constexpr inline _Size        
    H0(_Size __h) const
    {   return __h *= __m; 
    }
    constexpr inline _Size        
    H1(_Size __h, _Size __k) const
    {   return __h ^= __k;
    }
    constexpr inline _Size        
    HK(_Size __h, _Size __k) const
    {   return H1(H0(__h),K2(K1(K0(__k)))); 
    }
    constexpr inline _Size        
    L_calc(_Size __h, byte B0, byte B1, byte B2, byte B3) const
    {   return HK(__h,load_word<_Size>(B0,B1,B2,B3)); 
    }  
    constexpr inline _Size        
    L_calc(_Size __h, const byte *_B_ptr) const
    {   return HK(__h,load_word<_Size>(_B_ptr)); 
    }  



    constexpr inline _Size        
    S0(_Size __h) const
    {   return __h *= __m; 
    }
    constexpr inline _Size        
    S_calc(_Size __h, const byte B0) const
    {   return S0(__h ^= B0); 
    }
    constexpr inline _Size        
    S_calc(_Size __h, const byte B0, const byte B1) const
    {   return S_calc(__h ^= B1 << 8, B0); 
    }
    constexpr inline _Size        
    S_calc(_Size __h, const byte B0, const byte B1, const byte B2) const
    {   return S_calc(__h ^= B2 << 16, B0, B1); 
    }



    constexpr inline _Size        
    E0(_Size __h) const
    {   return __h ^= __h >> 13; 
    }
    constexpr inline _Size        
    E1(_Size __h) const
    {   return __h *= __m; 
    }
    constexpr inline _Size        
    E2(_Size __h) const
    {   return __h ^= __h >> 15; 
    }
    
    constexpr inline _Size        
    E_calc(_Size __h) const
    {   return E2(E1(E0(__h))); 
    }
    
    constexpr inline _Size        
    calc(const byte * _B_ptr, _Size _len, _Size _hsh=0) const
    {   return 
        (
         _hsh==0? calc( _B_ptr+4, _len-4, L_calc( _len, _B_ptr ) ) :
         _len>=4? calc( _B_ptr+4, _len-4, L_calc( _hsh, _B_ptr ) ) :
         _len==3? calc( _B_ptr+3, _len-3, S_calc( _hsh, _B_ptr[0], _B_ptr[1], _B_ptr[2] ) ) :
         _len==2? calc( _B_ptr+2, _len-2, S_calc( _hsh, _B_ptr[0], _B_ptr[1] ) ) :
         _len==1? calc( _B_ptr+1, _len-1, S_calc( _hsh, _B_ptr[0] ) ) :
         E_calc( _hsh ) 
        );
    }
    
    constexpr _Size
    operator()(const byte* __key, _Size __len) const
    {   return calc( __key , __len, 0);
    }
};

#if 0
// murmur2
template <class _Size>
_Size
__murmur2_or_cityhash<_Size, 32>::operator()(const void* __key, _Size __len)
{
    const _Size __m = 0x5bd1e995;
    const _Size __r = 24;
    _Size __h = __len;
    const unsigned char* __data = static_cast<const unsigned char*>(__key);
    for (; __len >= 4; __data += 4, __len -= 4)
    {
        _Size __k = __loadword<_Size>(__data);
        __k *= __m;
        __k ^= __k >> __r;
        __k *= __m;
        __h *= __m;
        __h ^= __k;
    }
    switch (__len)
    {
        case 3:
            __h ^= __data[2] << 16;
        case 2:
            __h ^= __data[1] << 8;
        case 1:
            __h ^= __data[0];
            __h *= __m;
    }
    __h ^= __h >> 13;
    __h *= __m;
    __h ^= __h >> 15;
    return __h;
}

#endif


constexpr const inline
size_t 
recursive_hash(size_t val, const char * str, size_t sz)
{
    return size_t
    ( sz>0 ? recursive_hash( ((val<<4)^(val>>28)^(*str)), str+1, sz-1)
     : ((val<<4)^(val>>28)^(*str))
    );
}





constexpr const inline
size_t 
operator""_hash(const char * str, size_t sz)
{
#ifdef MURMER_DEV
    return murmer<size_t>()( (const byte *)(const size_t)(str) , sz);
#else
    return recursive_hash(0x5bd1e995, str, sz);//murmer(str, sz)();
#endif
}




#endif // LL_const_string_h
