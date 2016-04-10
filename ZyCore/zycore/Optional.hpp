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

#ifndef _ZYCORE_OPTIONAL_HPP_
#define _ZYCORE_OPTIONAL_HPP_

#include "zycore/Utils.hpp"
#include "zycore/TypeTraits.hpp"

namespace zycore
{

// ============================================================================================== //
// [Optional]                                                                                     //
// ============================================================================================== //

// ---------------------------------------------------------------------------------------------- //
// Marker types                                                                                   //
// ---------------------------------------------------------------------------------------------- //

/**
 * @brief   Marker type hinting something is empty.
 */
static const struct Empty { Empty() {} } kEmpty;

/**
 * @brief   Marker type hinting in-place construction.
 */
static const struct InPlace { InPlace() {} } kInPlace;

namespace internal
{

// ---------------------------------------------------------------------------------------------- //
// [OptionalImplBase]                                                                             //
// ---------------------------------------------------------------------------------------------- //

template<typename T, typename=void>
class OptionalImplBase
{
    bool m_hasValue;
    std::aligned_storage_t<sizeof(T), std::alignment_of<T>::value> m_data;

    T* ptr()             { return reinterpret_cast<T*>(&m_data); }
    const T* ptr() const { return reinterpret_cast<const T*>(&m_data); }

    void destroyValue()
    {
        if (m_hasValue)
        {
            ptr()->~T();
            m_hasValue = false;
        }
    }
protected: // Copy and move semantics (used by OptionalImpl)
    template<typename TT = T, std::enable_if_t<IsMovable<TT>::value, int> = 0>
    void moveConstruct(OptionalImplBase<T>&& other)
    {
        m_hasValue = other.m_hasValue;
        if (other.m_hasValue) 
        {
            new (ptr()) T{std::move(*other.ptr())};
            other.destroyValue();
        }
    }

    template<typename TT = T, std::enable_if_t<IsMovable<TT>::value, int> = 0>
    void moveAssign(OptionalImplBase<T>&& other)
    {
        if (other.m_hasValue)
        {
            if (m_hasValue)
            {
                *ptr() = std::move(*other.ptr());
            }
            else
            {
                new (ptr()) T{std::move(*other.ptr())};
            }

            other.destroyValue();
        }
        else
        {
            destroyValue();
        }

        m_hasValue = other.m_hasValue;
    }

    template<typename TT = T, std::enable_if_t<IsCopyable<TT>::value, int> = 0>
    void copyConstruct(const OptionalImplBase<T>& other)
    {
        m_hasValue = other.m_hasValue;
        if (other.m_hasValue) 
        {
            new (ptr()) T{*other.ptr()};
        }
    }

    template<typename TT = T, std::enable_if_t<IsCopyable<TT>::value, int> = 0>
    void copyAssign(const OptionalImplBase<T>& other)
    {
        if (other.m_hasValue)
        {
            if (m_hasValue)
            {
                *ptr() = *other.ptr();
            }
            else
            {
                new (ptr()) T{*other.ptr()};
            }
        }
        else
        {
            destroyValue();
        }

        m_hasValue = other.m_hasValue;
    }

    template<typename TT = T, std::enable_if_t<IsMovable<TT>::value, int> = 0>
    void moveConstruct(T&& obj)
    {
        m_hasValue = true;
        new (ptr()) T{std::forward<T>(obj)};
    }

    template<typename TT = T, std::enable_if_t<IsMovable<TT>::value, int> = 0>
    void moveAssign(T&& obj)
    {
        destroyValue();
        new (ptr()) T{std::forward<T>(obj)};
        m_hasValue = true;
    }

    template<typename TT = T, std::enable_if_t<IsCopyable<TT>::value, int> = 0>
    void copyConstruct(const T& obj)
    {
        m_hasValue = true;
        new (ptr()) T{obj};
    }

    template<typename TT = T, std::enable_if_t<IsCopyable<TT>::value, int> = 0>
    void copyAssign(const T& obj)
    {
        destroyValue();
        new (ptr()) T{obj};
        m_hasValue = true;
    }
public: // Member functions
    /**
     * @brief   Type of the value.
     */
    using ValueType = T;

    /**
     * @brief   Default constructor, creats an empty optional.
     */
    OptionalImplBase() : OptionalImplBase{kEmpty} {}

    /**
     * @brief   Constructor creating an empty optional.
     * @param   empty @c kEmpty constant.
     */
    OptionalImplBase(Empty) : m_hasValue{false} {}

    /**
     * @brief   Constructor.
     * @tparam  ArgsT   The argument types used for in-place construction.
     * @param   inplace @c kInPlace constant.
     * @param   args    Arguments passed to the constructor for in-olace construction.
     */
    template<typename... ArgsT>
    OptionalImplBase(InPlace, ArgsT... args)
        : m_hasValue{true}
    {
        new (ptr()) T{args...};
    }

    /**
     * @brief   Destructor.
     */
    ~OptionalImplBase()
    {
        destroyValue();
    }
public: // Observers
    /**
     * @brief   Query if this object has a value assigned.
     * @return  true if the optional has a value, false if not.
     */
    bool hasValue() const { return m_hasValue; }

    /**
     * @brief   Convenience operator equivalent to @hasValue.
     * @copydetails hasValue
     */
    operator bool () const { return hasValue(); }

    /**
     * @brief   Gets the value.
     * @return  The value.
     * @warning When called on optional without value, the behaviour is undefined.
     */
    T& value()
    {
        if (!m_hasValue) fatalExit("tried to retrieve value of Optional without value");
        return *ptr();
    }

    /**
     * @brief   Gets the constant value.
     * @copydetails value
     */
    const T& value() const
    {
        if (!m_hasValue) fatalExit("tried to retrieve value of Optional without value");
        return *ptr();
    }

    /**
     * @brief   Releases the value from the optional, leaving behind an empty optional.
     * @return  The value.
     * @warning When called on optional without value, the behaviour is undefined.
     */
    template<typename TT = T, std::enable_if_t<IsMovable<TT>::value, int> = 0>
    T release()
    {
        auto tmp = std::move(value());
        destroyValue();
        return tmp;
    }
};

// ---------------------------------------------------------------------------------------------- //
// [OptionalImplBase] for references (lvalue and rvalue)                                          //
// ---------------------------------------------------------------------------------------------- //

template<typename T>
class OptionalImplBase<T, std::enable_if_t<std::is_reference<T>::value>>
{
    std::remove_reference_t<T>* m_value;
public:
    /**
     * @copydoc OptionalImplBase<T>::OptionalImplBase()
     */
    OptionalImplBase()
        : OptionalImplBase(kEmpty)
    {}

    /**
     * @copydoc OptionalImplBase<T>::OptionalImplBase(Empty)
     */
    OptionalImplBase(Empty)
        : m_value(nullptr)
    {}

    /**
     * @brief   Constructs the optional with a reference-value.
     */
    OptionalImplBase(T ref)
        : m_value(&ref)
    {}

    /**
     * @copydoc OptionalImplBase<T>::~OptionalImplBase
     */
    ~OptionalImplBase() = default;

    /**
     * @copydoc OptionalImplBase<T>::hasValue
     */
    bool hasValue() const { return m_value != nullptr; }

    /**
     * @copydoc OptionalImplBase<T>::operator bool
     */
    operator bool () const { return hasValue(); }

    /**
     * @copydoc OptionalImplBase<T>::value
     */
    T value()
    {
        if (!m_value) fatalExit("tried to retrieve value of Optional without value");
        return *m_value;
    }
    
    /**
     * @copydoc OptionalImplBase<T>::release
     */
    T release()
    {
        return value();
    }
};

// ---------------------------------------------------------------------------------------------- //
// [OptionalImpl] for non-copyable, non-movable types                                             //
// ---------------------------------------------------------------------------------------------- //

#define ZYCORE_OPTIONAL_FWD_EMPTY_CTORS                                                            \
    OptionalImpl() : OptionalImplBase<T>(kEmpty) /* MSVC12 requires parantheses here */ {}         \
    OptionalImpl(Empty) : OptionalImplBase<T>(kEmpty) /* ^ */ {}

#define ZYCORE_OPTIONAL_FWD_INPLACE_CTOR                                                           \
    template<typename... ArgsT>                                                                    \
    OptionalImpl(InPlace, ArgsT... args) : OptionalImplBase<T>(kInPlace, args...) /* ^ */ {}

#define ZYCORE_OPTIONAL_IMPL_MOVE_CTORS                                                            \
    OptionalImpl(OptionalImpl&& other)                                                             \
        { this->moveConstruct(std::forward<OptionalImpl>(other)); }                                \
    OptionalImpl& operator = (OptionalImpl&& other)                                                \
        { this->moveAssign(std::forward<OptionalImpl>(other)); return *this; }                     \
    OptionalImpl(T&& other)                                                                        \
        { this->moveConstruct(std::forward<T>(other)); }                                           \
    OptionalImpl& operator = (T&& other)                                                           \
        { this->moveAssign(std::forward<T>(other)); return *this; }

#define ZYCORE_OPTIONAL_IMPL_COPY_CTORS                                                            \
    OptionalImpl(const OptionalImpl& other)                                                        \
        { this->copyConstruct(other); }                                                            \
    OptionalImpl& operator = (const OptionalImpl& other)                                           \
        { this->copyAssign(other); return *this; }                                                 \
    OptionalImpl(const T& other)                                                                   \
        { this->copyConstruct(other); }                                                            \
    OptionalImpl& operator = (const T& other)                                                      \
        { this->copyAssign(other); return *this; }

template<typename T, typename=void>
struct OptionalImpl final
    : OptionalImplBase<T>
{
    ZYCORE_OPTIONAL_FWD_EMPTY_CTORS
    ZYCORE_OPTIONAL_FWD_INPLACE_CTOR
};

// ---------------------------------------------------------------------------------------------- //
// [OptionalImpl] for non-copyable, movable types                                                 //
// ---------------------------------------------------------------------------------------------- //

template<typename T>
struct OptionalImpl<T, std::enable_if_t<!IsCopyable<T>::value && IsMovable<T>::value>>
     : OptionalImplBase<T>
{
    ZYCORE_OPTIONAL_FWD_EMPTY_CTORS
    ZYCORE_OPTIONAL_FWD_INPLACE_CTOR
    ZYCORE_OPTIONAL_IMPL_MOVE_CTORS
};

// ---------------------------------------------------------------------------------------------- //
// [OptionalImpl] for copyable, non-movable types                                                 //
// ---------------------------------------------------------------------------------------------- //

template<typename T>
struct OptionalImpl<T, std::enable_if_t<IsCopyable<T>::value && !IsMovable<T>::value>>
     : OptionalImplBase<T>
{
    ZYCORE_OPTIONAL_FWD_EMPTY_CTORS
    ZYCORE_OPTIONAL_FWD_INPLACE_CTOR
    ZYCORE_OPTIONAL_IMPL_COPY_CTORS
};

// ---------------------------------------------------------------------------------------------- //
// [OptionalImpl] for copyable, movable types                                                     //
// ---------------------------------------------------------------------------------------------- //

template<typename T>
struct OptionalImpl<T, std::enable_if_t<IsCopyable<T>::value && IsMovable<T>::value>>
     : OptionalImplBase<T>
{
    ZYCORE_OPTIONAL_FWD_EMPTY_CTORS
    ZYCORE_OPTIONAL_FWD_INPLACE_CTOR
    ZYCORE_OPTIONAL_IMPL_MOVE_CTORS
    ZYCORE_OPTIONAL_IMPL_COPY_CTORS
};

// ---------------------------------------------------------------------------------------------- //
// [OptionalImpl] for references (lvalue and rvalue)                                              //
// ---------------------------------------------------------------------------------------------- //

template<typename T>
struct OptionalImpl<T, std::enable_if_t<std::is_reference<T>::value>>
     : OptionalImplBase<T>
{
    ZYCORE_OPTIONAL_FWD_EMPTY_CTORS
    OptionalImpl(T ref) : OptionalImplBase<T>(ref) /* MSVC12 requires parantheses here */ {}
};

#undef ZYCORE_OPTIONAL_FWD_EMPTY_CTORS
#undef ZYCORE_OPTIONAL_FWD_INPLACE_CTOR
#undef ZYCORE_OPTIONAL_IMPL_COPY_CTORS
#undef ZYCORE_OPTIONAL_IMPL_MOVE_CTORS

} // namespace internal

// ---------------------------------------------------------------------------------------------- //
// [Optional]                                                                                     //
// ---------------------------------------------------------------------------------------------- //

/**
 * @brief   Class representing "nullable" objects which may not (yet) have a value assigned.
 * @tparam  The value type of the optional.
 */
template<typename T>
using Optional = internal::OptionalImpl<T>;

// ============================================================================================== //

} // namespace zycore

#endif // _ZYCORE_OPTIONAL_HPP_
