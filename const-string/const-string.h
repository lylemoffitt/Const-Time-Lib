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


namespace LL {


    // literal class
    template<class Type, Type...Tps>
    class const_str ;

    template<char...t_chr>
    class const_str<char,t_chr...>
    {
        const std::size_t sz;
        const char data[sizeof...(t_chr)];
    public:

        using type = const_str<char,t_chr...>;

        constexpr
        const_str() : sz(sizeof...(t_chr)-1), data{t_chr...}
        {
        }

        // constexpr functions signal errors by throwing exceptions from operator ?:
        constexpr
        char operator[](std::size_t n) const
        {   return n < sz ? data[n] : throw std::out_of_range("");
        }

        constexpr
        std::size_t size() const { return sz; }

        constexpr
        bool operator==(const const_str & _cs ) const
        {   return (sz!=_cs.sz?false: true);
        }
        inline
        operator std::string(){
            return std::string(data);
        }
    };

    template<size_t N>
    class const_str<size_t,N>
    {

        const std::size_t sz;
        const char data[N];
    public:

        using type = const_str<size_t,N>;

        constexpr
        const_str(const char(&a)[N]) : sz(N-1), data(a) {}

        constexpr
        char operator[](std::size_t n) const
        {   return n < sz ? data[n] : throw std::out_of_range("");
        }

        constexpr
        std::size_t size() const
        {   return sz;
        }

        constexpr
        bool operator==(const const_str & _cs ) const
        {   return (sz!=_cs.sz?false: true);
        }

        template<class...Tps>
        constexpr inline
        bool operator==(const const_str<Tps...> & _cs ) const
        {   return (size()!=_cs.size()?false: true);
        }

        inline
        operator std::string()
        {   return std::string(data);
        }
    };



    template<class Dat_t, template<class,Dat_t...> class Ret_t, Dat_t..._data_vs>
    constexpr
    Ret_t<Dat_t,_data_vs...>
    recursive_load(const Dat_t * _D_ptr, size_t len)
    {
        return (len==0)
        ? Ret_t<Dat_t,_data_vs...>()
        : recursive_load
        < Dat_t, Ret_t< Dat_t, static_cast<Dat_t>(_D_ptr[0]), _data_vs... >
        , static_cast<Dat_t>(_D_ptr[0]), _data_vs...
        >(static_cast<const Dat_t*>(&_D_ptr[1]), len-1);
    }

//    static_assert(recursive_load<char, const_str>("h",1).size()>0,"");

//    template<class T,
//    constexpr
//    auto
//    make_const_str(const char * str, size_t sz)
//    {
//
//    }



    template<int ...>
    struct Sequence { };

    template<int N, int ...S>
    struct CreateSequence : CreateSequence<N-1, N-1, S...>
    {
    };

    template<int ... S>
    struct CreateSequence<0, S ...>
    {
        typedef Sequence<S...> Type;
    };

    template<int N, int ...S>
    constexpr
    auto Unpack(const char (&s)[N], Sequence<S...>) -> const_str<char,s[S]...>
    {
        return const_str<char,s[S]...>();
    }


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

    constexpr const inline
    size_t
    operator""_hash(const char * str, size_t sz)
    {
#ifdef MURMER_DEV
        return murmer<size_t>()( (const byte *)(const size_t)(str) , sz);
#else
        return recursive_hash(str, sz);//murmer(str, sz)();
#endif
    }


    template<char...chrs>
    constexpr inline
    typename const_str<char,chrs...>::type
    operator"" _c_str()
    {
        return const_str<char,chrs...>();
    }


//    constexpr inline
//    auto
//    operator""_c_str(const char * str, size_t sz)
////        -> const_str<size_t,sz>
//    {
////        return const_str<size_t,2>(" ");
////        return Unpack(str);
//        return const_str<char,"hello">();
//    }
//    static_assert(_c_str.size()==5,"");


} // namespace LL


template<


#endif // LL_const_string_h
