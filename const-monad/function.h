//
//  function.h
//  LyleLib
//
//  Created by Lyle Moffitt on 1/5/17.
//  Copyright © 2017 Lyle Moffitt. All rights reserved.
//

#ifndef function_h
#define function_h


#include <utility>

#include <type_traits>



/** 
 \class function_base

 Concentrates the functionality of the three classes here into one
 base class; differentiate only on the following:
 \li - Call semantics (model after std::invoke)
 \li - Storage needs
 \li - Type declarations (e.g. \c using and \c typedef directives)
 */
template<bool Fn_const, class Class_t, class Ret_t, class ... Param_ts>
struct function_base{};



template< class Fn_ptr_t, class Class_t = void>
class function;

//template< class R, class... Args >
//class function<R(Args...)>

/**
 Unimplemented specialization for function pointer.
 */
template<typename Ret_t, typename ... Param_ts>
struct function<Ret_t (*) (Param_ts...), void>;


template< class Ret_t, class ... Param_ts >
class function<Ret_t(Param_ts...), void>
{
public:

    using type = Ret_t(Param_ts...);

    using ret_type = Ret_t;
    using funct_type = type;
    using param_type = std::tuple<Param_ts...>;
    using self_type = function< Ret_t(Param_ts...) >;

    typedef ret_type (*fn_ptr_type)( Param_ts... );

    /// Void constructor
    constexpr
    function() :
        function_ptr( 0 )
    {
    }

    /// Construct from original function
    constexpr
    function( funct_type  function_original ) :
        function_ptr( function_original )
    {
    }

    /// Defaulted copy-constructor
//    function( const function & ) = default;


    /// Call function
    constexpr inline
    ret_type
    call( Param_ts ... arg_s ) const
    {
        return (*function_ptr)( arg_s... );
    }

    /// Call function
    template<typename ... Arg_ts>
    constexpr inline static
    ret_type
    call( funct_type fn_ptr, Arg_ts... arg_s )
    {
        return fn_ptr( arg_s... );
    }

    /// Call function
    constexpr inline
    ret_type operator()( Param_ts ... arg_s ) const
    {
        return (*function_ptr)( arg_s... );
    }

    /// Determine if function is callable
    constexpr inline
    operator bool() const
    {
        return (function_ptr != 0);
    }
    
private:
    fn_ptr_type function_ptr;
    
};


template<typename Ret_t, typename ... Param_ts>
constexpr
auto get_Fn(Ret_t (*fn_ptr) (Param_ts...))->function<Ret_t(Param_ts...)>
{
    return function<Ret_t(Param_ts...)>(*fn_ptr);
}


namespace test {
    constexpr
    bool t0(){
        return true;
    }
    constexpr
    bool t1(bool val){
        return val;
    }
    template<typename t>
    constexpr
    bool t2(t val){
        return true && val;
    }

    template< typename Arg_0_t>
    constexpr
    bool t3(Arg_0_t arg_0){
        return arg_0;
    }
    template< typename Arg_0_t, typename ... Arg_N_ts >
    constexpr
    typename std::enable_if<((sizeof...(Arg_N_ts))>=1),bool>::type
    t3(Arg_0_t arg_0, Arg_N_ts...arg_N){
        return (arg_0 && t3(arg_N...));
    }

    constexpr
    const bool t4(){
        return true;
    }
    constexpr
    const bool t5(const bool){
        return true;
    }


    static_assert(function<decltype(t0)>(t0)(),"");
    static_assert(function<decltype(t1)>(t1)(true),"");
    static_assert(function<decltype(t2<bool>)>(t2)(true),"");
    static_assert(function<decltype(t3<bool>)>(t3)(true),"");
    static_assert(function<decltype(t3<bool,bool>)>(t3)(true,true),"");
    static_assert(function<decltype(t4)>(t4)(),"");
    static_assert(function<decltype(t5)>(t5)(true),"");


    static_assert(get_Fn(t0)(),"");
    static_assert(get_Fn(t1)(true),"");
    static_assert(get_Fn(t2<bool>)(true),"");
    static_assert(get_Fn(t3<bool>)(true),"");
    static_assert(get_Fn(t3<bool,bool>)(true,true),"");
    static_assert(get_Fn(t4)(),"");
    static_assert(get_Fn(t5)(true),"");


    static_assert(get_Fn(t0).call(),"");
    static_assert(get_Fn(t1).call(true),"");
    static_assert(get_Fn(t2<bool>).call(true),"");
    static_assert(get_Fn(t3<bool>).call(true),"");
    static_assert(get_Fn(t3<bool,bool>).call(true,true),"");



//    static_assert(function<bool(bool,bool)>(t2<bool,bool>)(true,true),"");


}



//template< class R, class T >
///*unspecified*/ mem_fn(R T::* pm);

//template<class Fn_ptr_t>
//class member_function;

template<class Object_t, class Class_t, class Ret_t, class ... Param_ts>
class function<Ret_t (Class_t::*)(Param_ts...)const, Object_t>
{
    typedef Ret_t (Class_t::*class_fn_ptr_type)( Param_ts... ) const;
public:

    using type = Ret_t (Class_t::*)(Param_ts...) const;

    using object_type   = Object_t;
    using class_type    = Class_t;
    using ret_type      = Ret_t;
    using funct_type    = type;
    using param_type    = std::tuple<Param_ts...>;
    using self_type     = function<type,Class_t>;

    template <class n_class_t>
    using new_self_type = function<type,n_class_t>;

    template <class new_obj_t>
    using new_class_type
    = typename std::remove_cv
    < typename std::remove_pointer<new_obj_t>::type
    >::type;

    template <class new_obj_t>
    using is_obj_same_class
    = std::is_same
    < class_type
    , typename std::remove_cv
    < typename std::remove_pointer<new_obj_t>::type
    >::type
    >;

    constexpr
    function() :
    cls_obj_ptr(0),
    cls_fun_ptr(0)
    {}

    constexpr
    function(const class_type * obj, const funct_type fn) :
    cls_obj_ptr(obj),
    cls_fun_ptr(fn)
    {}


    template<class _class_t>
    constexpr inline
    typename std::enable_if
    < is_obj_same_class<_class_t>::value
    , function< type,new_class_type<_class_t> >
    >::type
    with(const _class_t * obj) const {
        return function
        < type
        , new_class_type<_class_t>
        >(obj,cls_fun_ptr);
    }


    constexpr inline
    ret_type
    call (const class_type *obj, Param_ts ... param_args) const
    {
        return ((*obj).*cls_fun_ptr)(param_args...);
    }

    constexpr inline
    ret_type
    call (Param_ts ... param_args) const
    {
        return ((*cls_obj_ptr).*cls_fun_ptr)(param_args...);
    }

    constexpr inline
    ret_type
    operator() (Param_ts ... param_args) const
    {
        return ((*cls_obj_ptr).*cls_fun_ptr)(param_args...);
    }

    constexpr inline
    operator bool() const
    {
        return (cls_obj_ptr!=0 && cls_fun_ptr!=0);
    }

private:
    const class_type *      cls_obj_ptr;
    const class_fn_ptr_type cls_fun_ptr;

    static_assert(is_obj_same_class<object_type>::value
                  ,"Object type must be same as class type.");
};


template<class Object_t, class Class_t, class Ret_t, class ... Param_ts>
class function<Ret_t (Class_t::*)(Param_ts...), Object_t>
{
    typedef Ret_t (Class_t::*class_fn_ptr_type)( Param_ts... );
public:

    using type = Ret_t (Class_t::*)(Param_ts...);

    using object_type   = Object_t;
    using class_type    = Class_t;
    using ret_type      = Ret_t;
    using funct_type    = type;
    using param_type    = std::tuple<Param_ts...>;
    using self_type     = function<type,object_type>;

    template <class n_class_t>
    using new_self_type = function<type,n_class_t>;

    template <class new_obj_t>
    using new_class_type
    = typename std::remove_cv
    < typename std::remove_pointer<new_obj_t>::type
    >::type;

    template <class new_obj_t>
    using is_obj_same_class
    = std::is_same
    < class_type
    , typename std::remove_cv
        < typename std::remove_pointer<new_obj_t>::type
        >::type
    >;

    constexpr
    function() :
    cls_obj_ptr(0),
    cls_fun_ptr(0)
    {}

    constexpr
    function(const class_type * obj, const funct_type fn) :
    cls_obj_ptr(obj),
    cls_fun_ptr(fn)
    {}


    template<class _class_t>
    constexpr inline
    typename std::enable_if
    < is_obj_same_class<_class_t>::value
    , function< type, new_class_type<_class_t> >
    >::type
    with(const _class_t * obj) const {
        return function
        < type
        , new_class_type<_class_t>
        >(obj,cls_fun_ptr);
    }


    constexpr inline
    ret_type
    call (const class_type *obj, Param_ts ... param_args) const
    {
        return ((*obj).*cls_fun_ptr)(param_args...);
    }

    constexpr inline
    ret_type
    call (Param_ts ... param_args) const
    {
        return ((*cls_obj_ptr).*cls_fun_ptr)(param_args...);
    }

    constexpr inline
    ret_type
    operator() (Param_ts ... param_args) const
    {
        return call(param_args...);
    }

    constexpr inline
    operator bool() const
    {
        return (cls_obj_ptr!=0 && cls_fun_ptr!=0);
    }

private:
    const class_type *      cls_obj_ptr;
    const class_fn_ptr_type cls_fun_ptr;

    static_assert(is_obj_same_class<object_type>::value
                  ,"Object type must be same as class type.");

};

//template<typename Cls_t, typename Ret_t, typename ... Param_ts>
//constexpr
//auto get_Mem(const Cls_t * cls_ptr,  const Ret_t (Cls_t::*fn_ptr)(Param_ts...))
//->  member_function<Cls_t, Ret_t, Param_ts...>
//{
//    return member_function<Cls_t, Ret_t, Param_ts...>(cls_ptr,fn_ptr);
//}

template<typename Cls_t, typename Ret_t, typename ... Param_ts>
constexpr
auto get_Fn(Ret_t (Cls_t::*fn_ptr)(Param_ts...))
->  function<Ret_t (Cls_t::*)(Param_ts...),Cls_t>
{
    return function<Ret_t (Cls_t::*)(Param_ts...),Cls_t>(0,fn_ptr);
}

//template<typename Cls_t, typename Ret_t, typename ... Param_ts>
//constexpr
//auto get_Fn(Ret_t (Cls_t::*fn_ptr)(Param_ts...) const)
//->  member_function<Cls_t, Ret_t, Param_ts...>
//{
//    return member_function<Cls_t, Ret_t, Param_ts...>(nullptr,fn_ptr);
//}

template<typename Cls_t, typename Ret_t, typename ... Param_ts>
constexpr
auto get_Fn(Ret_t (Cls_t::*fn_ptr)(Param_ts...) const)
->  function<Ret_t (Cls_t::*)(Param_ts...)const,Cls_t>
{
    return function<Ret_t (Cls_t::*)(Param_ts...)const,Cls_t>(0,fn_ptr);
}
namespace test
{
    template<class val_t>
    struct cls_t
    {
        using value_type = val_t;
        value_type value;

        constexpr
        cls_t(value_type val):value(val)
        {}

        static constexpr
        value_type fn0(){
            return true;
        }
        value_type fn1(){
            return true;
        }
        constexpr
        value_type fn2() const{
            return true;
        }
        constexpr
        value_type fn3(value_type arg) const{
            return arg;
        }
        constexpr
        value_type fn4() const{
            return value;
        }
        constexpr
        value_type fn5(const value_type arg) const{
            return arg;
        }
    };

    using non_cls = cls_t<int>;

    using cls = cls_t<bool>;

    constexpr cls obj0(false);
    constexpr cls obj1(true);
//    cls obj0(false);
//    cls obj1(true);

    constexpr non_cls bad_obj(true);

    static_assert(std::is_same
                  < bool()
                  , decltype(cls::fn0)
                  >::value,"");

    static_assert(get_Fn(cls::fn0)(),"");
    static_assert(function<decltype(cls::fn0)>(cls::fn0)(),"");

    static_assert(std::is_same
                  < bool(cls::*)()
                  , decltype(&cls::fn1)
                  >::value,"");
    static_assert(std::is_same
                  < bool(cls::*)()
                  , function<decltype(&cls::fn1),cls>::type
                  >::value,"");

    static_assert(get_Fn(&cls::fn1).with(&obj0),"");
    static_assert(get_Fn(&cls::fn1).with(&obj1),"");

    //this line should fail
//    static_assert(get_Fn(&cls::fn1).with(&bad_obj),"");


    static_assert(std::is_same
                  < bool(cls::*)() const
                  , decltype(&cls::fn2)
                  >::value,"");
    static_assert(std::is_same
                  < bool(cls::*)() const
                  , function<decltype(&cls::fn2),cls>::type
                  >::value,"");
    // NOTE: const-member functions do not counst as "const" for
    // purposes of `std::remove_cv`
    static_assert(std::is_same
                  < bool(cls::*)() const
                  , std::remove_cv<decltype(&cls::fn2)>::type
                  >::value == true /* WTF */,"");
    // NOTE: const-member functions do not counst as "const" for
    // purposes of `std::is_const`
    static_assert(std::is_const
                  <decltype(&cls::fn2)
                  >::value == false /* WTF */,"");

    static_assert(get_Fn(&cls::fn2).call(&obj0),"");
    static_assert(get_Fn(&cls::fn2).call(&obj1),"");


    static_assert(get_Fn(&cls::fn3).call(&obj0,true),"");
    static_assert(get_Fn(&cls::fn3).call(&obj1,true),"");


    static_assert(get_Fn(&cls::fn4).call(&obj0) == false,"");
    static_assert(get_Fn(&cls::fn4).call(&obj1) == true,"");

    static_assert(get_Fn(&cls::fn5).call(&obj0,true),"");
    static_assert(get_Fn(&cls::fn5).call(&obj1,true),"");

//    static_assert(get_Fn(&cls::fn4).call(&obj0,true),"");
//    static_assert(get_Fn(&cls::fn4).call(&obj1,true),"");

//    static_assert(function<decltype(cls::fn1)>(cls::fn0),"");

//    static_assert(get_Mem(&obj, &cls::fn1)(),"");
//    static_assert(function<decltype(cls::fn1)>(cls::fn1)(),"");
}








#endif /* function_h */
