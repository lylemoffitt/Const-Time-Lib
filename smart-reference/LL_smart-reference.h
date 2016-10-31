//
//  LL_smart-reference.h
//  LyleLib
//
//  Created by Lyle Moffitt on 1/6/15.
//  Copyright (c) 2015 Lyle Moffitt. All rights reserved.
//

#ifndef LyleLib_LL_smart_reference_h
#define LyleLib_LL_smart_reference_h

#include <memory>

//namespace LL{

template <class _Tp>
class shared : std::shared_ptr<_Tp> {
    typedef std::shared_ptr<_Tp> _base;
public:
    constexpr 
    shared():_base(){}
    
    using _base::operator=;
    
    using _base::operator bool;
    
    inline
    _Tp &   get()
    {   return (*this->get());  
    } 
    
    inline
    _Tp &   get() const 
    {   return (*this->get());  
    } 
    
    inline
    _Tp &   operator*()
    {   return (*this);
    }
    
    inline
    _Tp *   operator->()
    {   return (this->get());
    }
    
    
};
    
    
//}// namespace LL

#endif
