/**
 * This file is part of the zyan core library (zyantific.com).
 * 
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Joel Höner (athre0z)
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software 
 * and associated documentation files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge, publish, distribute, 
 * sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all copies or 
 * substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING 
 * BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef _ZYCORE_UTILS_HPP_
#define _ZYCORE_UTILS_HPP_

#include <exception> // std::terminate
#include <cstdint>
#include <functional>

namespace zycore
{

// ============================================================================================== //
// [BlackBoxConsts]                                                                               //
// ============================================================================================== //

/**  
 * @brief   Black-box constants opaque to the compiler until template instantiation.
 *          
 * Can be used to make @c static_assert statements evaluate only when the containing template
 * is about to be instantiated. Some people use @c sizeof(T)\ !=\ sizeof(T) instead, which 
 * however, other than this version, fails for types that cannot be sizeofed (e.g. functions).
 * Lazy evaluation is accomplished by making the constants dependant on a template argument.
 */
template<typename>
struct BlackBoxConsts
{
    static const bool kFalse = false;
    static const bool kTrue = true;
};

// ============================================================================================== //
// [NonCopyable]                                                                                  //
// ============================================================================================== //

/**
 * @brief   Makes deriving classes non-copyable.
 */
class NonCopyable
{
protected:
    NonCopyable() = default;
    ~NonCopyable() = default;
    NonCopyable(const NonCopyable&) = delete;
    NonCopyable& operator = (const NonCopyable&) = delete;
};

// ============================================================================================== //
// [StaticInitializer]                                                                            //
// ============================================================================================== //

/**
 * @brief Emits user defined callbacks on con and destruction.
 */
class StaticInitializer : public NonCopyable
{
    using Callable = std::function<void()>;
private:
    Callable m_destruct;
public:
    explicit StaticInitializer(Callable construct, Callable destruct = nullptr);
    ~StaticInitializer();
};

// ---------------------------------------------------------------------------------------------- //
// Implementation of inline methods [StaticInitializer]                                           //
// ---------------------------------------------------------------------------------------------- //

inline StaticInitializer::StaticInitializer(Callable construct, Callable destruct)
    : m_destruct(destruct)
{
    if (construct)
    {
        construct();
    }
}
    
inline StaticInitializer::~StaticInitializer()
{
    if (m_destruct)
    {
        m_destruct();
    }
}

// ============================================================================================== //
// [fatalExit]                                                                                    //
// ============================================================================================== //

/**
 * @brief   Terminates the program after a fatal error occurred.
 * @param   why Short description of the error (currently unused).
 */
inline void fatalExit(const char* /*why*/)
{
    std::terminate();
}

// ============================================================================================== //

} // namespace remodel

#endif // _ZYCORE_UTILS_HPP_