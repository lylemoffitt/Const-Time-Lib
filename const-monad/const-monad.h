//
//  const-monad.h
//  LyleLib
//
//  Created by Lyle Moffitt on 1/3/17.
//  Copyright © 2017 Lyle Moffitt. All rights reserved.
//

#ifndef const_monad_h
#define const_monad_h

#include "const-sequence.h"
#include "function.h"

#include <type_traits>
#include <functional>
#include <utility>

#define S( _str_ ) #_str_

namespace ctxe_funct
{
    template< class ... >
    struct fn;

    template< class Ret_t, class ... Param_ts >
    struct fn<Ret_t, Param_ts...>
    {
        using ret_type = Ret_t;
        using param_types = std::tuple<Param_ts...>;

        typedef ret_type (*type)( param_types... );

        template<class ... Arg_ts>
        ret_type
        operator()(Arg_ts...Val_Ns)
        {

        }
    };


    template<typename Val_t>
    struct monad
    {
        Val_t value;

        using type = monad<Val_t>;
        using value_type = Val_t;

        using is_monad = std::integral_constant<bool, true>;

        constexpr
        monad(Val_t _value) : value(_value)
        {
        }


        template<typename newVal_t>
        constexpr inline static
        monad<newVal_t> unit(newVal_t val) 
        {
            return monad<newVal_t>(val) ;
        }

        template<class Fn_t, typename ... Param_ts>
        constexpr inline
        typename std::result_of<Fn_t(value_type,Param_ts...)>::type
        bind( Fn_t func, Param_ts...Arg_s ) const
        {
            static_assert( decltype(func(value,Arg_s...))::is_monad::value
                          ,"Cannot bind to function that does not return a monad.");
            return func(value,Arg_s...);
        }

        template<class Fn_t, typename ... Param_ts>
        constexpr inline
        monad<typename std::result_of<Fn_t(value_type,Param_ts...)>::type>
        lift ( Fn_t func, Param_ts...Arg_s ) const
        {
            return unit(func(value,Arg_s...));
        }

        template<typename Fn_0_t, typename... Fn_N_ts>
        constexpr
        monad< typename std::result_of<Fn_0_t(value_type)>::type >
        apply(Fn_0_t fn_0, Fn_N_ts... fn_Ns) const
        {   return (sizeof...(fn_Ns)==0)
            ? bind(fn_0)
            : apply(fn_Ns...)
            ;
        }


        template<class Fn_t>
        constexpr inline
        typename std::result_of<Fn_t(value_type)>::type
        operator| ( Fn_t func) const
        {
            static_assert( decltype(func(value))::is_monad::value
                          ,"Cannot bind to function that does not return a monad.");
            return func(value);
        }

        constexpr
        operator value_type() const{
            return value;
        }
    };

    template<typename Val_t>
    constexpr
    monad<Val_t> unit(Val_t val)
    {   return monad<Val_t>( val );
    }
//    template<class Fn_t, typename ... Param_ts>
//    constexpr inline
//    typename std::result_of<Fn_t(Param_ts...)>::type
//    bind( Fn_t func, Param_ts...Arg_s )
//    {
//        static_assert( decltype(func(Arg_s...))::is_monad::value
//                      ,"Cannot bind to function that does not return a monad.");
//        return func(Arg_s...);
//    }

    namespace test
    {
        constexpr
        monad<int> divideby2(int i)
        {   return unit(i/2);
        }
        constexpr
        monad<bool> even(int i)
        {   return unit<bool>(i % 2 == 0);
        }
        constexpr
        monad<bool> odd(int i)
        {   return unit<bool>(i % 2 != 0);
        }
        constexpr
        monad<int> mult_by(int i, int x)
        {   return unit<int>(i*x);
        }
        template<class val_t, val_t val_arg>
        constexpr
        val_t add(val_t value){
            return value + val_arg;
        }

        static_assert(unit(22)
                      .bind(divideby2)
                      .bind(divideby2)
                      .bind(divideby2)
                      .bind(even)
                      .value
                      ,"");

        static_assert(unit(22)
                      | divideby2
                      | divideby2
                      | divideby2
                      | even
                      ,"");
        static_assert((unit(22)
                      | divideby2
                      | divideby2
                       ).bind(mult_by, 2)
                      | divideby2
                      | odd
                      ,"");

        static_assert(unit(22)
                      .bind(divideby2)
                      .bind(mult_by,2)
                      .bind(divideby2)
                      .lift(add<int,2>)
                      .bind(even)
                      .value == false
                      ,"");
    }//namespace test


};


template<class Ident, template <class...Params> class Fn >
struct method_t
{
    using name = Ident;

    template< class ... Args >
    using exec = Fn<Args...>;
};

template< typename ... val_t >
struct Data {
    using types = type_list<val_t...>;
};


template< typename _key_t, typename _value_t >
struct KV_pair{
    using key_type = _key_t;
    using value_type = _value_t;
};

template< class Ident, template< class ret_type, class ... param_type > class Fn >
struct method{

    using name = Ident;

    template<class...Args>
    using exec = Fn<Args...>;
};


//template< class ... >
//struct monad;

//template< template <class...> class ... >
//struct monad;





template< typename _val_t, _val_t val >
struct value_monad{

    using value_type = _val_t;

    static constexpr value_type value = val;

    template< template <class...> class Func, class...Args >
    using bind = Func< value_type, Args... >;
};

template< typename val_t, val_t value >
using unit = value_monad<val_t, value>;


template< typename _val_t, template <class...m_args> class ... Methods>
struct type_monad
{

    using type = _val_t;
    using value = _val_t;

    template< typename val_type >
    using unit = type_monad<val_type>;

    template< template <class...> class Func, class...Args >
    using bind = Func< value, Args... >;


    template< template <class...> class Func >
    using lift = type_monad< bind<Func>, Methods... >;

    template< template <class...> class Func >
    using method = type_monad< value, Func, Methods... >;
};



namespace test{

    static_assert(type_monad<int>
                  ::bind<std::is_same, int>
                  ::value
                  ,"");
//    static_assert(type_monad<int>
//                  ::lift<std::is_same>
//                  ,"");

    template<class T, T val>
    using int_const = std::integral_constant<T, val>;

//    static_assert(type_monad<int_const<int,4>>
//                  ::bind<int_const>::
//                  ,"");
};







//variadic constructor
template<class... base_type>
class va_container : public base_type... {
public:
    constexpr
    va_container(const base_type & ... base_init) :
    base_type(base_init)... { }

    constexpr
    va_container(base_type && ... base_init) :
    base_type(std::forward<base_type>(base_init))... { }
};

template <class...> class Method;


template< template <class...> class ... Methods >
struct m_monad
{


};





#endif /* const_monad_h */
