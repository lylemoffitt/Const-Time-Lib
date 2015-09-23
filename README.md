# Constant-Time Library

---
>A collection of compile-time / constant-time libraries to extend C++. [WIP]

### Motivation

Included here is a collection of small projects that grew out of a need I saw in C++, that strings cannot be the indexes (`case`s) of a `switch` block. While most-other POD types can, the closest you can get is using the pointer addresses. This won't be good enough if you're `switch` parameter is anything other than an r-value reference to one of the strings (a tricky proposition in itself). 

In short, I set out to make something like the following possible.
````.cpp
    string arg = "foo";

    switch(arg)
    {
        case "foo":
            /* ... */
            break;
        
        case "bar":
            /* ... */
            break;
        
        default:
    }
````

### Implementation

At first approach, it would seem that this is a non-problem. The solution is simple, *hashes*! While this should work in theory, because we are simplifying the objects down to POD, `size_t`, numbers; in practice, it's impossible because the hashes aren't calculated at *compile-time*, but at run-time. That being said, using something like `std::unordered_map<std::string,std::function<void(void)>>` is certainly possible, if you're willing to accept the run-time cost *and* have access to the STL. Unfortunately, neither is really an option if you're programming for embedded environments. That being said, algorithmically this is entirely the right approach. So that's what I did.


### String Switch [super-alpha]

Under `String Switch/` you will find my first-draft implementation. It uses a simple hashing function implemented with meta-template programming techniques to ensure compile-time conclusion. It's conveniently accessible as a `constexpr` [operator-literal](http://en.cppreference.com/w/cpp/language/user_literal) function. In short, if you *my* hashing function it will work. However, this isn't compatible with `std::hash<std::string>`, which uses the much more complex [City Hash](https://www.wikiwand.com/en/CityHash) or [Murmer Hash](https://www.wikiwand.com/en/MurmurHash) depending on your platform. This is the jumping off point for the rest of the libraries.


### Const-Function [WIP]

This is a helper library I wrote to try and ease the creation of compile-time versions of the STL's hashing functions. It's basically an attempt to try and generalize the syntax of meta-template programming so that functions can be written in a more **sane** C-like syntax. Only later did I realize that what I really needed was to use [Boost MPL](www.boost.org/doc/libs/1_59_0/libs/mpl/). But that's an adventure for another time.


### Smart-Reference [WIP]

This is a re-implementation of `std::smart_pointer` but for references. It'll necessarily end up being more like `boost::optional`, than I'd initially hoped. Still very much WIP.


### Base64 [WIP]

As an exercise on how to implement string processing functions, I wrote a library to encode/decode base-64 strings as a compile-time operation. I can't really think of why anyone would need this. It's just something I did cuz I'm a nerd.


