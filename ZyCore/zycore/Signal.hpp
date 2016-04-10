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

#ifndef ZYCORE_SIGNAL_HPP
#define ZYCORE_SIGNAL_HPP

#include "zycore/Utils.hpp"

#include <map>
#include <thread>
#include <mutex>

namespace zycore
{

class SignalObject;

using SlotHandle = size_t;

// ============================================================================================== //
// Internal base classes                                                                          //
// ============================================================================================== //

namespace internal 
{
    class SignalBase
    {
        friend SignalObject;
        virtual void onSlotsObjectDestroyed(SlotHandle handle) = 0;
    public:
        virtual ~SignalBase() = default;
    };

    struct SignalObjectBase
    {
        template<typename Object, typename... ArgsT>
        friend class ClassFuncConnection;

        virtual ~SignalObjectBase() = default;
        virtual void onSignalConnected(SignalBase *signal, SlotHandle handle) = 0;
        virtual void onSignalDisconnected(SignalBase *signal, SlotHandle handle) = 0;
    };
} // namespace internal

// ============================================================================================== //
// [ConnectionBase]                                                                               //
// ============================================================================================== //

/**
 * @brief   Abstract base class for connections between signals and slots.
 * @tparam  ArgsT The slot's argument types.
 */
template<typename... ArgsT>
struct ConnectionBase
{
    template<typename...>
    friend class Signal;
public: // Public interface.
    /**
     * @brief   Default constructor.
     */
    ConnectionBase() = default;

    /**
     * @brief   Destructor.
     */
    virtual ~ConnectionBase() = default;

    /**
     * @brief   Calls the connected slot.
     * @param   args The arguments.
     */
    virtual void call(ArgsT... args) const = 0;
private: // Private interface.
    /**
     * @brief   Called when connection is destroyed.
     * @param   handle  The slot handle.
     */
    virtual void onDestroy(SlotHandle handle) = 0;
};

// ============================================================================================== //
// [FuncConnection]                                                                               //
// ============================================================================================== //

/**
 * @brief   Connection between static slot and signal.
 * @tparam  ArgsT The slot's argument types.
 */
template<typename... ArgsT>
class FuncConnection
    : public ConnectionBase<ArgsT...>
{
public:
    using Function = std::function<void(ArgsT... args)>;
    /**
     * @brief   Constructor.
     * @param   func The slot to be connected.
     */
    explicit FuncConnection(Function func);
public: // Implementation of public, virtual member-functions.
    /**
     * @brief   Calls the slot.
     * @param   args  Arguments.
     */
    virtual void call(ArgsT... args) const override;
private:
    Function m_func;
private: // Implementation of private interface.
    virtual void onDestroy(SlotHandle /*handle*/) override {}
};

// ============================================================================================== //
// [LifetimedConnection]                                                                          //
// ============================================================================================== //

/**
 * @brief   Connection automatically loosened when either the signal or the lifetime-giving objects
 *          is destoyed.
 * @tparam  ArgsT   The slot's member function argument types.
 */
template<typename... ArgsT>
class LifetimedConnection
    : public ConnectionBase<ArgsT...>
    , public NonCopyable
{
public:
    using Function = std::function<void(ArgsT...)>;
private:
    Function m_func;
    internal::SignalBase* m_signal;
    internal::SignalObjectBase* m_lifetimeObject;
public: // Constructor
    /**
     * @brief   Constructor.
     * @param   obj     The object whose lifetime to bind to.
     * @param   func    The callback function.
     * @param   sig     The signal to connect to.
     * @param   handle  The slot handle.
     */
    LifetimedConnection(internal::SignalObjectBase *obj, Function func, 
        internal::SignalBase* sig, SlotHandle handle);
public: // Implementation of public interface.
    void call(ArgsT... args) const override;
private: // Implementation of private interface.
    void onDestroy(SlotHandle handle) override;
};

// ============================================================================================== //
// [internal::MemberFuncBinding]                                                                  //
// ============================================================================================== //

namespace internal
{

/**
 * @brief   Binds @c this to a member-function without the need of placeholders.
 */
template<typename ObjectT, typename... ArgsT>
class MemberFuncBinding
{
    using Member = void (ObjectT::*)(ArgsT...);
    ObjectT* m_obj;
    Member m_member;
public:
    MemberFuncBinding(ObjectT* obj, Member memberFunc);
    void operator() (ArgsT... args);
};

} // namespace internal

// ============================================================================================== //
// [Signal]                                                                                       //
// ============================================================================================== //

/**
 * @brief   Type and thread safe signal implementation.
 * @tparam  ArgsT The slot's argument types.
 *
 * For more information, see Signals & Slots at wikipedia:
 * http://en.wikipedia.org/wiki/Signals_and_slots
 */
template<typename... ArgsT>
class Signal 
    : public internal::SignalBase
    , public NonCopyable
{
    // Typedefs and private member-variables  
    using ConnectionBase = ConnectionBase<ArgsT...>;
    std::map<SlotHandle, ConnectionBase*> m_slots;
    SlotHandle m_IdCtr;
    mutable std::recursive_mutex m_mutex;
public: // Con- & Destructor.
    /**
     * @brief   Default constructor.
     */
    Signal();

    /**
     * @brief   Destructor.
     */
    virtual ~Signal();
public: // Public interface.
    /**
     * @brief   Adds a given connection to the internal list.
     * @param   connection The connection to be added. The pointer will be 
     *                     stolen.
     */
    SlotHandle connect(FuncConnection<ArgsT...>* connection);

    /**
     * @brief   Connects a static slot to the signal.
     * @param   func The function/lambda to connect.
     */
    SlotHandle connect(typename FuncConnection<ArgsT...>::Function func);

    /**
     * @brief   Disconnects an existing connetion by it's handle
     * @param   handle  The connection handle.
     * @return  `true` on success, `false` if the connection didn't exist.
     */
    bool disconnect(SlotHandle handle);

    /**
     * @brief   Emits the signal and calls all connected slots.
     * @param   args  Arguments to be passed to the slots.
     */
    void emit(ArgsT... args) const;

    /**
     * @brief   Shorthand for @c emit.
     */
    void operator () (ArgsT... args) const;

    /**
     * @brief   Shorthand for @c connect.
     * @param   func  The function (slot) to connect with the signal.
     * @return  This instance.
     */
    Signal& operator += (typename FuncConnection<ArgsT...>::Function func);

    /**
     * @brief   Adds a given connection to the internal list.
     * @param   connection The connection to be added. Ownership is transfered.
     */
    template<typename Object>
    SlotHandle connect(LifetimedConnection<Object, ArgsT...>* connection)
    {
        std::lock_guard<std::recursive_mutex> lock(m_mutex);
        m_slots.emplace(m_IdCtr, connection);
        return m_IdCtr++;
    }

    /**
     * @brief   Connects a function to the signal binding the connection lifetime to a 
     *          signal object.
     * @param   object The object to be connected.
     * @param	member The member function.
     *                 
     * The connection is automatically released as soon as either the signal or the
     * lifetime-giver is destroyed.
     */
    SlotHandle connect(SignalObject* lifetimeGiver, std::function<void(ArgsT...)> func)
    {
        std::lock_guard<std::recursive_mutex> lock(m_mutex);
        SlotHandle handle = m_IdCtr++;
        m_slots.emplace(handle, new LifetimedConnection<ArgsT...>(
            lifetimeGiver, func, this, handle));
        return handle;
    }

    /**
     * @brief   Connects a member-function slot to the signal.
     * @param   object The object to be connected.
     * @param	member The member function.
     *                 
     * The connection is automatically released as soon as either the signal or the object
     * with the slot is destroyed.
     */
    template<typename ObjectT>
    SlotHandle connect(ObjectT* object, void(ObjectT::*member)(ArgsT...))
    {
        static_assert(std::is_base_of<SignalObject, ObjectT>::value,
            "type has to be derived from SignalObject");
        return connect(object, internal::MemberFuncBinding<ObjectT, ArgsT...>(object, member));
    }
private: // Interface for SignalObject.
    /**
     * @brief   Callback used by slots when they are destroyed.
     */
    void onSlotsObjectDestroyed(SlotHandle handle) override;
};

// ============================================================================================== //
// Implementation of inline methods [FuncConnection]                                              //
// ============================================================================================== //

template<typename... ArgsT>
inline FuncConnection<ArgsT...>::FuncConnection(Function func)
    : m_func(func)
{}

template<typename... ArgsT>
inline void FuncConnection<ArgsT...>::call(ArgsT... args) const
{
    m_func(args...);
}

// ============================================================================================== //
// Implementation of inline methods [LifetimedConnection]                                         //
// ============================================================================================== //

template<typename... ArgsT>
inline LifetimedConnection<ArgsT...>::LifetimedConnection(internal::SignalObjectBase *lifetimeObj, 
        Function func, internal::SignalBase* sig, SlotHandle handle)
    : m_func(func)
    , m_signal(sig)
    , m_lifetimeObject(lifetimeObj)
{
    lifetimeObj->onSignalConnected(sig, handle);
}

template<typename... ArgsT>
inline void LifetimedConnection<ArgsT...>::call(ArgsT... args) const
{
    m_func(args...);
}

template<typename... ArgsT>
inline void LifetimedConnection<ArgsT...>::onDestroy(SlotHandle handle)
{
    m_lifetimeObject->onSignalDisconnected(m_signal, handle);
}

// ============================================================================================== //
// Implementation of inline functions [internal::MemberFuncBinding]                               //
// ============================================================================================== //

namespace internal
{

template<typename ObjectT, typename... ArgsT>
inline MemberFuncBinding<ObjectT, ArgsT...>::MemberFuncBinding(ObjectT* obj, Member memberFunc)
    : m_obj(obj)
    , m_member(memberFunc)
{}

template<typename ObjectT, typename... ArgsT>
inline void MemberFuncBinding<ObjectT, ArgsT...>::operator() (ArgsT... args)
{
    (m_obj->*m_member)(args...);
}

} // namespace internal

// ============================================================================================== //
// Implementation of inline functions [Signal]                                                    //
// ============================================================================================== //

template<typename... ArgsT> 
inline Signal<ArgsT...>::Signal()
    : m_IdCtr(1)
{}

template<typename... ArgsT>
inline Signal<ArgsT...>::~Signal()
{
    std::lock_guard<std::recursive_mutex> lock(m_mutex);
    for (auto i = m_slots.begin(); i != m_slots.end(); ++i)
    {
        i->second->onDestroy(i->first);
        delete i->second;
    }
}

template<typename... ArgsT>
inline SlotHandle Signal<ArgsT...>::connect(FuncConnection<ArgsT...>* connection)
{
    std::lock_guard<std::recursive_mutex> lock(m_mutex);
    m_slots.emplace(m_IdCtr, connection);
    return m_IdCtr++;
}

template<typename... ArgsT>
inline bool Signal<ArgsT...>::disconnect(SlotHandle handle)
{
    std::lock_guard<std::recursive_mutex> lock(m_mutex);
    auto it = m_slots.find(handle);
    if (it == m_slots.end()) return false;
    it->second->onDestroy(it->first);
    delete it->second;
    m_slots.erase(it);
    return true;
}

template<typename... ArgsT>
inline SlotHandle Signal<ArgsT...>::connect(typename FuncConnection<ArgsT...>::Function func)
{
    std::lock_guard<std::recursive_mutex> lock(m_mutex);
    auto connection = new FuncConnection<ArgsT...>(func);
    m_slots.emplace(m_IdCtr, connection);
    return m_IdCtr++;
}

template<typename... ArgsT>
inline void Signal<ArgsT...>::emit(ArgsT... args) const
{
    std::lock_guard<std::recursive_mutex> lock(m_mutex);
    for (const auto &cur : m_slots)
    {
        cur.second->call(args...);
    }
}

template<typename... ArgsT>
inline void Signal<ArgsT...>::operator()(ArgsT... args) const
{
    emit(args...);
}

template<typename... ArgsT>
inline Signal<ArgsT...>& Signal<ArgsT...>::operator += (
    typename FuncConnection<ArgsT...>::Function func)
{
    connect(func);
    return *this;
}

template<typename... ArgsT>
inline void Signal<ArgsT...>::onSlotsObjectDestroyed(SlotHandle handle)
{
    std::lock_guard<std::recursive_mutex> lock(m_mutex);
    for (auto i = m_slots.begin(); i != m_slots.end();)
    {
        if (i->first == handle)
        {
            delete i->second;
            i = m_slots.erase(i);
        }
        else
        {
            ++i;
        }
    }
}

// ============================================================================================== //

} // namespace zycore

#endif // ZYCORE_SIGNAL_HPP
