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

#ifndef _ZYCORE_SINGLETON_HPP_
#define _ZYCORE_SINGLETON_HPP_

#include "zycore/Utils.hpp"

namespace zycore
{

// ============================================================================================== //
// [Singleton]                                                                                    //
// ============================================================================================== //

/**
 * @brief   Singleton template
 * @todo    Should we free the instance on static shutdown rather than leaking it.
 */
template<typename ObjTypeT>
class Singleton : public NonCopyable
{
    static ObjTypeT *m_obj;
protected:
    /**
     * @brief   Default constructor.
     */
    Singleton() = default;
public:
    /**
     * @brief   Destructor.
     */
    virtual ~Singleton() = default;
    /**
     * @brief   Gets the instance.
     * @return  The instance.
     */
    static ObjTypeT& instance();
    /**
     * @brief   Frees the instance.
     */
    static void freeInstance();
    /**
     * @brief   Queries if the singleton has already been instanciated.
     * @return  @c true if instanciated, @c false if not.
     */
    static bool hasInstance();
};

// ============================================================================================== //
// Implementation of inline functions [Singleton]                                                 //
// ============================================================================================== //

template<typename ObjTypeT>
ObjTypeT *Singleton<ObjTypeT>::m_obj = nullptr;

template<typename ObjTypeT>
inline ObjTypeT& Singleton<ObjTypeT>::instance()
{
    if (!m_obj)
    {
        m_obj = new ObjTypeT;
    }

    return *m_obj;
}

template<typename ObjTypeT>
inline void Singleton<ObjTypeT>::freeInstance()
{
    if (m_obj)
    {
        delete m_obj;
        m_obj = nullptr;
    }
}

template<typename ObjTypeT>
inline bool Singleton<ObjTypeT>::hasInstance()
{
    return m_obj != nullptr;
}

// ============================================================================================== //

} // namespace zycore

#endif // _ZYCORE_SINGLETON_HPP_
