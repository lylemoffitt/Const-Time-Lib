//
//  control-expr.h
//  LyleLib
//
//  Created by Lyle Moffitt on 1/3/17.
//  Copyright © 2017 Lyle Moffitt. All rights reserved.
//

#ifndef control_expr_h
#define control_expr_h

template<bool B, class T = void>
struct enable_if
{
};

template<class T>
struct enable_if<true, T>
{   typedef T type;
};



template<bool B, class T, class F>
struct type_if
{   typedef T type;
};

template<class T, class F>
struct type_if<false, T, F>
{   typedef F type;
};





template<unsigned bits,class t0, class ... tN>
struct least_t
{    typedef typename type_if
    < (bits <= (8*sizeof(t0)))
    , t0
    , typename least_t<bits,tN...>::type
    >::type		type;
};

template<unsigned bits,class t0>
struct least_t <bits,t0>
{   typedef typename enable_if
    < (bits <= (8*sizeof(t0)))
    , t0
    >::type		type;
};

#endif /* control_expr_h */
