/**
 * This file is part of the zyan core library (zyantific.com).
 * 
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Joel Höner (athre0z)
 * Copyright (c) 2015 Florian Bernd
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

#ifndef _ZYCORE_EXCEPTIONS_HPP_
#define _ZYCORE_EXCEPTIONS_HPP_

#include "Config.hpp"
#include <stdexcept>
#ifdef ZYCORE_WINDOWS
#   include <Windows.h>
#else
#   include <errno.h>
#   include <iostream>
#endif

// ============================================================================================== //
// Macro definitions                                                                              //
// ============================================================================================== //

#ifdef ZYCORE_DEBUG
    #define ZYCORE_RAISE(class, ...)                                                               \
        throw class(__VA_ARGS__, __FILE__, __LINE__);
    #define ZYCORE_FATAL_EXIT(class, ...)                                                          \
        class(__VA_ARGS__, __FILE__, __LINE__).fatalExit();
#else
    #define ZYCORE_RAISE(class, ...)                                                               \
        throw class(__VA_ARGS__, "", 0);
    #define ZYCORE_FATAL_EXIT(class, ...)                                                          \
        class(__VA_ARGS__, "", 0).fatalExit();
#endif

namespace zycore
{

// ============================================================================================== //
// [BaseException]                                                                                //
// ============================================================================================== //

/**
 * @brief   Base class for all exceptions.
 */
class BaseException : public std::runtime_error
{
private:
    const std::string   m_message;
    const std::string   m_fileName;
    const unsigned int  m_lineNumber;
public:
    /**
     * @brief   Constructor.
     * @param   message The exception message.
     */
    explicit BaseException(const std::string& message)
        : BaseException(message, "", 0) { };
    /**
     * @brief   Constructor.
     * @param   message     The exception message.
     * @param   fileName    The name of the file where the exception occured.
     * @param   lineNumber  The line number where the exception occured.
     */
    BaseException(const std::string& message, const std::string& fileName, 
        unsigned int lineNumber)
        : std::runtime_error(message)
        , m_message(message)
        , m_fileName(fileName)
        , m_lineNumber(lineNumber) { };
public:
    BaseException& operator = (const BaseException&) = delete;
public:
    /**
     * @brief   Instantly shows the exception message and terminates the program. This is useful
     *          if a fatal error occured during deconstruction of an object.
     */
    void fatalExit() const;
public:
    /**
     * @brief   Returns the exception message.
     * @return  The exception message.
     */
    std::string getMessage() const;
    /**
     * @brief   Returns the name of the file where the exception occured.
     * @return  The file name.
     */
    std::string getFileName() const;
    /**
     * @brief   Indicates the line number where the exception occured.
     * @return  The line number.
     */
    unsigned int getLineNumber() const;
};

inline void BaseException::fatalExit() const
{
    // TODO: Format message to include filename and linenumber
#ifdef ZYCORE_WINDOWS
    MessageBoxA(nullptr, m_message.c_str(), "Error", MB_ICONERROR);
#else
    std::cerr << m_message;
#endif
    std::terminate();
}

inline std::string BaseException::getMessage() const
{
    return m_message;
}

inline std::string BaseException::getFileName() const
{
    return m_fileName;
}

inline unsigned int BaseException::getLineNumber() const
{
    return m_lineNumber;
}

// ============================================================================================== //
// [ExceptionTemplate]                                                                            //
// ============================================================================================== //

/**
 * @brief   Templated base class for runtime exceptions of any kind.
 * @tparam  MarkerT The marker type used for disambiguation.
 *                  
 * As forwarding constructors for specialized runtime-error exception types can get really tedious,
 * we create them using template aliases. By allocating a new type using the class/struct 
 * keyword and feeding it into the base template, the types become distinguishable in exception
 * handling.
 */
template <typename T>
class ExceptionTemplate : public BaseException
{
public:
    /**
     * @brief   Constructor.
     * @param   message The exception message.
     */
    explicit ExceptionTemplate(const std::string& message) 
        : BaseException(message) { };
    /**
     * @brief   Constructor.
     * @param   message     The exception message.
     * @param   fileName    The name of the file where the exception occured.
     * @param   lineNumber  The line number where the exception occured.
     */
    ExceptionTemplate(const std::string& message, const std::string& fileName, 
        unsigned int lineNumber)
        : BaseException(message, fileName, lineNumber) { };
};

#define ZYCORE_EXCEPTION_TYPE_FROM_TEMPLATE(template_, newType)                                    \
    class __##newType {};                                                                          \
    using newType = template_<__##newType>;

// ============================================================================================== //
// Exception types used all over the project                                                      //
// ============================================================================================== //

ZYCORE_EXCEPTION_TYPE_FROM_TEMPLATE(ExceptionTemplate, InvalidUsage  );
ZYCORE_EXCEPTION_TYPE_FROM_TEMPLATE(ExceptionTemplate, NotImplemented);
ZYCORE_EXCEPTION_TYPE_FROM_TEMPLATE(ExceptionTemplate, OutOfBounds   );

// ============================================================================================== //
// [OSException]                                                                                  //
// ============================================================================================== //

#ifdef ZYCORE_WINDOWS
typedef uint32_t ErrorCode;
#else
typedef int ErrorCode;
#endif

/**
 * @brief   Exception for operating system errors.
 */
class OSException : public BaseException
{
private:
    const std::string   m_functionName;
    const ErrorCode     m_errorCode;
private:
    static std::string formatExceptionMessage(const std::string& functionName,
        ErrorCode errorCode);
public:
    /**
     * @brief   Constructor.
     * @param   functionName    The name of the function that caused the exception.
     */
#ifdef ZYCORE_WINDOWS
    explicit OSException(const std::string& functionName)
        : OSException(functionName, GetLastError()) { };
#else
    explicit OSException(const std::string& functionName)
        : OSException(functionName, errno) { };
#endif
    /**
     * @brief   Constructor.
     * @param   functionName    The name of the function that caused the exception.
     * @param   fileName        The name of the file where the exception occured.
     * @param   lineNumber      The line number where the exception occured.
     */
#ifdef ZYCORE_WINDOWS
    OSException(const std::string& functionName, const std::string& fileName, 
        unsigned int lineNumber):
        OSException(functionName, GetLastError(), fileName, lineNumber) { };
#else
    OSException(const std::string& functionName, const std::string& fileName, 
        unsigned int lineNumber):
        OSException(functionName, errno, fileName, lineNumber) { };
#endif
    /**
     * @brief   Constructor.
     * @param   functionName    The name of the function that caused the exception.
     * @param   errorCode       The system error code.
     */
    OSException(const std::string& functionName, ErrorCode errorCode)
        : OSException(functionName, errorCode, "", 0) { };
    /**
     * @brief   Constructor.
     * @param   functionName    The name of the function that caused the exception.
     * @param   errorCode       The system error code.
     * @param   fileName        The name of the file where the exception occured.
     * @param   lineNumber      The line number where the exception occured.                          
     */
    OSException(const std::string& functionName, ErrorCode errorCode, 
        const std::string &fileName, unsigned int lineNumber)
        : BaseException(formatExceptionMessage(functionName, errorCode), fileName, lineNumber)
        , m_functionName(functionName)
        , m_errorCode(errorCode) { };
public:
    /**
     * @brief   Returns the name of the function that caused the exception.
     * @return  The function name.
     */
    std::string getFunctionName() const;
    /**
     * @brief   Returns the system error code for this exception.
     * @return  The system error code.
     */
    ErrorCode getErrorCode() const;
};

inline std::string OSException::formatExceptionMessage(const std::string& /*functionName*/, 
    ErrorCode /*errorCode*/)
{
    // TODO: Format exception message   
    return std::string();
}

inline std::string OSException::getFunctionName() const 
{
    return m_functionName;
}

inline ErrorCode OSException::getErrorCode() const 
{
    return m_errorCode;
}

// ============================================================================================== //

} // namespace zycore

#endif // _ZYCORE_EXCEPTIONS_HPP_
