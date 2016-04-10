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

#ifndef _ZYCORE_RESULT_HPP_
#define _ZYCORE_RESULT_HPP_

#include "zycore/Optional.hpp"

namespace zycore
{

// ============================================================================================== //
// [Result]                                                                                       //
// ============================================================================================== //

// ---------------------------------------------------------------------------------------------- //
// Marker types                                                                                   //
// ---------------------------------------------------------------------------------------------- //

static const struct Error { Error() {} } kError;

// ---------------------------------------------------------------------------------------------- //
// Helper classes                                                                                 //
// ---------------------------------------------------------------------------------------------- //

namespace internal
{

template<typename ResultT>
class ResultResultValueHolder
{
protected:
    Optional<ResultT> m_result;

    template<typename... ArgsT>
    explicit ResultResultValueHolder(ArgsT&&... args)
        : m_result(std::forward<ArgsT>(args)...)
    {}
};

template<>
class ResultResultValueHolder<void>
{
protected:
    ResultResultValueHolder(InPlace) {}
    ResultResultValueHolder(Empty) {}
};

template<typename ResultT>
class ResultErrorValueHolder
{
protected:
    Optional<ResultT> m_error;

    template<typename... ArgsT>
    explicit ResultErrorValueHolder(ArgsT&&... args)
        : m_error(std::forward<ArgsT>(args)...)
    {}
};

template<>
class ResultErrorValueHolder<void>
{
protected:
    ResultErrorValueHolder(InPlace) {}
    ResultErrorValueHolder(Empty) {}
};

} // namespace internal

// ---------------------------------------------------------------------------------------------- //
// [Result]                                                                                       //
// ---------------------------------------------------------------------------------------------- //

/**
 * @brief   Class representing either failure or success, optionally holding either an operation
 *          result or an error descriptor.
 * @tparam  ResultT Type of the result.
 * @tparam  ErrorT  Type of the error.
 */
template<typename ResultT = void, typename ErrorT = void>
class Result final
    : public internal::ResultResultValueHolder<ResultT>
    , public internal::ResultErrorValueHolder<ErrorT>
{
public:
    /**
     * @brief   Constructor creating a result in error state.
     * @tparam  ArgsT   Argument types for in-place construction.
     * @param   error   @c kEmpty constant.
     * @param   args    Arguments passed to the constructor of the error value.
     */
    template<typename... ArgsT>
    Result(Error, ArgsT&&... args) 
        : internal::ResultResultValueHolder<ResultT>(kEmpty)
        , internal::ResultErrorValueHolder<ErrorT>(kInPlace, std::forward<ArgsT>(args)...)
    {}

    /**
     * @brief   Constructor creating a result in success state.
     * @tparam  ArgsT   Argument types for in-place construction
     * @param   args    Arguments passed to the constructor of the result value.
     */
    template<typename... ArgsT>
    Result(ArgsT&&... args)
        : internal::ResultResultValueHolder<ResultT>(kInPlace, std::forward<ArgsT>(args)...)
        , internal::ResultErrorValueHolder<ErrorT>(kEmpty)
    {}
public:
    /**
     * @brief   Queries if the operation that lead to the result succeeded.
     * @return  @c true if the operation succeded and thus this result has a result value but no
     *          error value, @c false if not.
     */
    // Implementation capturing Result<ResultT, ErrorT> and Result<ResultT, void>
    template<typename ResultTT = ResultT, std::enable_if_t<!std::is_void<ResultTT>::value, int> = 0>
    bool succeeded() const
    {
        return this->m_result.hasValue();
    }
    
    /**
     * @copydoc succeeded()
     */
    // Implementation capturing Result<void, ErrorT>. Case Result<void, void> is handled by 
    // a specialization of Result itself.
    template<typename ResultTT = ResultT, 
        std::enable_if_t<std::is_void<ResultTT>::value && !std::is_void<ErrorT>::value, int> = 0>
    bool succeeded() const
    {
        return !this->m_error.hasValue();
    };

    /**
     * @brief   Convenience operator equivalent to @c succeeded.
     * @copydetauls succeeded
     */
    operator bool () const
    {
        return succeeded();
    }

    /**
     * @brief   Queries if the operation that lead to the result failed.
     * @return  @c true if the operation failed and thus this result has an error value but no
     *          result value, @c false if not.
     */
    bool failed() const
    {
        return !succeeded();
    }

    /**
     * @brief   Obtains the result value.
     * @return  The result value.
     * @warning Attepmt to obtain the result value of results in error state is undefined 
     *          behaviour. Doing so will terminate the application.
     */
    template<typename ResultTT = ResultT, std::enable_if_t<!std::is_void<ResultTT>::value, int> = 0>
    ResultTT& result()
    {
        if (!succeeded()) fatalExit("attempt to obtain result of result in error state");
        return this->m_result.value();
    }

    /**
     * @brief   Obtains the error value.
     * @return  An ErrorT&amp;
     */
    template<typename ErrorTT = ErrorT, std::enable_if_t<!std::is_void<ErrorTT>::value, int> = 0>
    ErrorTT& error()
    {
        if (!failed()) fatalExit("attempt to obtain error of result in success state");
        return this->m_error.value();
    }

    /**
     * @brief   Convenience operator equivalent to @result.
     * @copydetails result
     */
    template<typename ResultTT = ResultT, std::enable_if_t<!std::is_void<ResultTT>::value, int> = 0>
    explicit operator ResultTT& ()
    {
        return result();
    }
};

// ---------------------------------------------------------------------------------------------- //
// [Result<void, void>]                                                                           //
// ---------------------------------------------------------------------------------------------- //

/**
 * @brief   @copydoc Result
 */
template<>
class Result<void, void> final
{
    bool m_succeeded;
public:
    /**  
     * @copydoc Result::Result(Error, ...)
     */
    explicit Result(Error) 
        : m_succeeded(false) 
    {}

    /**  
     * @copydoc Result::Result(...)
     */
    Result() 
        : m_succeeded(true) 
    {}
public:
    /**  
     * @copydoc Result::succeeded
     */
    bool succeeded() const { return m_succeeded;  }

    /**  
     * @copydoc Result::failed
     */
    bool failed   () const { return !succeeded(); }

    /**  
     * @copydoc Result::operator bool
     */
    operator bool () const { return succeeded();  }
};

// ============================================================================================== //

} // namespace zycore

#endif // _ZYCORE_RESULT_HPP_