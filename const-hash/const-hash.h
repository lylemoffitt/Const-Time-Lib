//
//  const-hash.h
//  LyleLib
//
//  Created by Lyle Moffitt on 1/3/17.
//  Copyright © 2017 Lyle Moffitt. All rights reserved.
//

#ifndef const_hash_h
#define const_hash_h

#include <cstdint>
#include <cstddef>

namespace LL
{
    typedef uint8_t byte;


    template <class _sz_t = size_t>
    constexpr inline
    _sz_t load_word(byte B0, byte B1, byte B2, byte B3)
    {
        return static_cast<_sz_t>(  (B3<<(8*3))
                                  | (B2<<(8*2))
                                  | (B1<<(8*1))
                                  | (B0<<(8*0)) );
    }
    namespace test{
        static_assert( load_word(0x12,0x34,0x56,0x78)==0x78563412 ,"");
    }

    template <class _sz_t = size_t>
    constexpr inline
    _sz_t load_word(const byte * _B_ptr)
    {
        //    return static_cast<const _Size>( *static_cast<const _Size*>( _B_ptr ) );
        return static_cast<_sz_t>(  (_B_ptr[0]<<(8*0))
                                  | (_B_ptr[1]<<(8*1))
                                  | (_B_ptr[2]<<(8*2))
                                  | (_B_ptr[3]<<(8*3)) );
    }
    namespace test{
        constexpr const byte data_4byte[] = {0x12,0x34,0x56,0x78};
        static_assert( load_word(data_4byte)==0x78563412 ,"");
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

    template <typename _sz_t = size_t>
    struct murmer
    {
        typedef _sz_t size_type;

        constexpr const static size_type __m = 0x5bd1e995;
        constexpr const static size_type __r = 24;


        /* The following functions duplicate this block of the algo

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
         */
        constexpr inline static
        size_type
        K0(const size_type __k)
        {   return __k * __m;
        }
        constexpr inline static
        size_type
        K1(const size_type __k)
        {   return __k ^ (__k >> __r);
        }
        constexpr inline static
        size_type
        K2(const size_type __k)
        {   return __k * __m;
        }
        constexpr inline static
        size_type
        H0(const size_type __h)
        {   return __h * __m;
        }
        constexpr inline static
        size_type
        H1(const size_type __h, const size_type __k)
        {   return __h ^ __k;
        }
        constexpr inline static
        size_type
        HK(const size_type __h, const size_type __k)
        {   return H1(H0(__h),K2(K1(K0(__k))));
        }
        constexpr inline static
        size_type
        L_calc(const size_type __h, byte B0, byte B1, byte B2, byte B3)
        {   return HK(__h,load_word<size_type>(B0,B1,B2,B3));
        }
        constexpr inline static
        size_type
        L_calc(const size_type __h, const byte *_B_ptr)
        {   return HK(__h,load_word<size_type>(_B_ptr));
        }


        /* The following functions duplicate this block of the algo

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
         */
        constexpr inline static
        size_type
        S0(const size_type __h)
        {   return __h * __m;
        }
        constexpr inline static
        size_type
        S_calc(const size_type __h, const byte B0)
        {   return S0(__h ^ B0);
        }
        constexpr inline static
        size_type
        S_calc(const size_type __h, const byte B0, const byte B1)
        {   return S_calc(__h ^ (B1 << 8), B0);
        }
        constexpr inline static
        size_type
        S_calc(const size_type __h, const byte B0, const byte B1, const byte B2)
        {   return S_calc(__h ^ (B2 << 16), B0, B1);
        }



        /* The following functions duplicate this block of the algo

         __h ^= __h >> 13;
         __h *= __m;
         __h ^= __h >> 15;
         */
        constexpr inline static
        size_type
        E0(const size_type __h)
        {   return __h ^ (__h >> 13);
        }
        constexpr inline static
        size_type
        E1(const size_type __h)
        {   return __h * __m;
        }
        constexpr inline static
        size_type
        E2(const size_type __h)
        {   return __h ^ (__h >> 15);
        }

        constexpr inline static
        size_type
        E_calc(const size_type __h)
        {   return E2(E1(E0(__h)));
        }

        constexpr inline static
        size_type
        calc(const byte * _B_ptr, size_type _len, size_type _hsh=0) 
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

        constexpr
        size_type
        operator()(const byte* __key, size_type __len) const
        {   return calc( __key , __len, 0);
        }
    };
    namespace test{
        constexpr const byte
            data_8byte[] = {0x12,0x34,0x56,0x78, 0x87,0x65,0x43,0x21};

        static_assert( 0x52bcf091
                      == murmer<uint32_t>::calc(data_4byte, 4), "");
        static_assert( 0xa06c5f3a
                      == murmer<uint32_t>::calc(data_8byte, 8), "");

    }

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
    recursive_hash(const char * str, size_t sz, size_t val = 0x5bd1e995)
    {
        return size_t
        ( sz>0
         ? recursive_hash( str+1, sz-1, ((val<<4)^(val>>28)^(*str)) )
         : ((val<<4)^(val>>28)^(*str))
         );
    }



    template<typename _key_t, typename _res_t>
    struct const_hash
    {
        typedef _key_t key_type;
        typedef _res_t result_type;

        constexpr
        result_type operator()(const key_type & value) const;
    };





} //namespace LL

#endif /* const_hash_h */
