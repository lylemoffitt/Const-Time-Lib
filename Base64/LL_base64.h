//
//  LL_base64.h
//  LyleLib
//
//  Created by Lyle Moffitt on 3/17/15.
//  Copyright (c) 2015 Lyle Moffitt. All rights reserved.
//

#ifndef __LyleLib__LL_base64__
#define __LyleLib__LL_base64__


#include "assert_or_throw.h"

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <cassert>
#include <locale>
#include <valarray>
#include <typeinfo>

class b64_digit_t
{
    struct{
        uint8_t     base    :6;
        uint8_t             :1;
        bool        pad     :1;
    };
public:
    bool inline is_pad(){ return pad; }
    
    template<class uint_type>
    inline
    b64_digit_t(uint_type num, bool is_padding = false)
    { 
        static_assert(std::is_unsigned<uint_type>::value,
                      "Value cannot be initialized with non-unsigned type.");
        static_assert(077==63, "Octal digits not recognized");
        base    = static_cast<uint8_t>(077 & num); 
        pad     = is_padding;
    }
    inline
    b64_digit_t(char _b64)
    { 
        if(false){
        }else if(_b64>='A'&&_b64<='Z'){ 
            base=(_b64-'A'+00);     pad=false;   
        }else if(_b64>='a'&&_b64<='z'){ 
            base=(_b64-'a'+26);     pad=false;
        }else if(_b64>='0'&&_b64<='9'){ 
            base=(_b64-'0'+52);     pad=false;
        }else if(_b64=='+'){   
            base=62;                pad=false;
        }else if(_b64=='/'){   
            base=63;                pad=false;
        }else if(_b64=='='){   
            base=00;                pad=true;
        }else {
            assert_or_throw(false, std::invalid_argument,
                            "Invalid base64 digit.");
        }
    }
    inline
    operator char()
    {
        if(pad){             return  '=';
        }else if(base<=25){  return  ('A'+(base-00)); 
        }else if(base<=51){  return  ('a'+(base-26)); 
        }else if(base<=61){  return  ('0'+(base-52)); 
        }else if(base==62){  return  '+';             
        }else if(base==63){  return  '/';             
        }else{
            assert_or_throw(false, std::logic_error,
                            "Invalid base64 value.");
        }
    }
    inline
    operator uint8_t()
    {   assert_or_throw(pad==false, std::bad_cast, 
                        "Padding cannot be cast to value.");
        return base;
    }
};

std::string static
to_b64( void * _dat, size_t _sz )
{
    std::string ret;
    uint8_t * _raw (static_cast<uint8_t*>(_dat));
    for(size_t ii=0; ii<_sz; ii+=3)
    {   
        switch(_sz-ii)
        {   case 0: return ret;
            case 1: return ret += "=";
            case 2: return ret += "==";
            default: break;
        }
        uint32_t _b24 (*((uint32_t*)(_raw+ii)));
        ret+=(b64_digit_t(2341234));
        
        
        uint8_t octal[4] = 
        {   uint8_t( (_b24>>0)  &077 ), 
            uint8_t( (_b24>>6)  &077 ), 
            uint8_t( (_b24>>12) &077 ), 
            uint8_t( (_b24>>18) &077 ) 
        }; 
        for(uint8_t i=0; i<4; ++i)
        {   
            ret+=char(b64_digit_t(octal[i]));
        }
    }
    return ret;
}

std::vector<uint8_t> static
from_b64(const std::string & _b64 )
{
    std::vector<uint8_t> ret;
    uint32_t sum;
    for( size_t i=0, sz=_b64.size(); i<sz; )
    {
        uint8_t val;
        
        b64_digit_t _d(_b64[i]);
        if (_d.is_pad()) {
            break;
        }
        val = _d;
        
        sum += val<<(i%3);
        
        if(i%3==2){
            ret.push_back((sum>>0) &0xff);
            ret.push_back((sum>>8) &0xff);
            ret.push_back((sum>>16)&0xff);
        }
    }
    
    return ret;
}

class base64
{   
    std::valarray<uint8_t> base;
    
public:
    
    class base64_ref
    {
        uint8_t * b0, * b1;
        uint8_t offset;
    public:
        
        base64_ref()=delete;
        
        base64_ref(uint8_t _off, 
                   uint8_t * _b0, uint8_t * _b1=nullptr): b0(_b0), b1(_b1)
        {
            assert_or_throw(_b0!=nullptr,std::invalid_argument,
                            "First argument must not be nullptr.");
        }
        
        uint8_t inline operator = (uint8_t val){
            uint16_t tmp = (*b0 | (b1==nullptr?0:(*b1)<<8));
            tmp &= ~(077<<offset);
            tmp |= (val<<offset);
            *b0 = tmp&0xff;
            if(b1!=nullptr){
                *b0 = (tmp>>8)&0xff;
            }
            return val;
        }
        
        char inline operator =(char digit){
            (*this) = uint8_t(b64_digit_t(digit));
            return digit;
        }
        
        inline operator char(){
            uint16_t tmp = (*b0 | (b1==nullptr?0:(*b1)<<8));
            tmp >>= offset;
            return (tmp&0xff);
        }
        
    };
    
    
};


#endif /* defined(__LyleLib__LL_base64__) */
