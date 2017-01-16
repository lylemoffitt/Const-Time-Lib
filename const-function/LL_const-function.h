//
//  LL_const-function.h
//  LyleLib
//
//  Created by Lyle Moffitt on 12/16/14.
//  Copyright (c) 2014 Lyle Moffitt. All rights reserved.
//

#ifndef LyleLib_LL_const_function_h
#define LyleLib_LL_const_function_h

#include <utility>
#include <functional>






template<class condition_f, class >
struct for_const
{
    
};




template<class...Tp>
class tuple_ray : public std::tuple<Tp...> {
    
    typedef typename std::tuple_size<std::tuple<Tp...>>  size_type;
    
    template <std::size_t N>
    using type = typename std::tuple_element<N, std::tuple<Tp...>>::type;
    
public:
    
    constexpr
    tuple_ray(const Tp &...Tp_arg): std::tuple<Tp...> (Tp_arg...){}
    
    constexpr
    tuple_ray(Tp &&...Tp_arg): std::tuple<Tp...> (Tp_arg...){}
    
    
    constexpr inline 
    size_t    size()  const   { return size_type::value; }
    
    constexpr inline 
    operator  size_t()const   { return size_type::value; }
    
    constexpr inline 
    auto operator[](size_t I) const -> decltype(std::get<I>(*this))
    {   return std::get<I>(*this); 
    }
    
    template< class...Tups  >
    constexpr inline
    auto operator + (Tups ... tup_arg) const -> decltype(std::tuple_cat((*this), tup_arg...))
    {   return std::tuple_cat((*this), tup_arg...);
    }
    
    
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


#if 0
template <class funct_t>
struct va_funct{
    typedef decltype(std::declval<funct_t()>()()) return_type;
    
    
    template <class arg_t0, class...arg_tN> 
    constexpr 
    const auto
    operator()(const arg_t0 & arg_0, const arg_tN &... arg_N) const
    {
        return funct_t()(arg_0,(*this)(arg_N...)) ;
    }
    
    template <class arg_t0, class arg_t1>
    constexpr 
    const auto
    operator()(const arg_t0 & arg_0, const arg_t1 & arg_1) const
    {
        return funct_t()(arg_0,arg_1) ;
    }
    
    template<class...arg_t>
    constexpr
    const auto
    eval(return_type * store, arg_t...args)const
    {
        return (*store)=(*this)(args...);
    }
};
#endif








#endif
