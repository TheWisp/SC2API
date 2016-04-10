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

#ifndef _ZYCORE_SIGNALOBJECT_HPP_
#define _ZYCORE_SIGNALOBJECT_HPP_

#ifdef ZYCORE_HEADER_ONLY
#   error "This file cannot be used in header-only mode."
#endif // ZYCORE_HEADER_ONLY

#include "zycore/Signal.hpp"

#include <vector>
#include <mutex>
#include <memory>

namespace zycore
{

// ============================================================================================== //
// [SignalObject]                                                                                 //
// ============================================================================================== //

/**
 * @brief   Base class for all objects whose methods should be able to serve
 * 			as slots to be connected to a signal.
 * @ingroup sigslot
 */
class SignalObject 
    : public internal::SignalObjectBase
    , public NonCopyable
{
    std::recursive_mutex m_objectMutex;
    std::vector<std::tuple<SlotHandle, internal::SignalBase*>> m_connectedSignals;
public: // Con- and destruction.
    /**
     * @brief   Default constructor.
     */
    SignalObject() = default;
    /**
     * @brief   Destructor.
     */
    virtual ~SignalObject();
public: // Signals.
    /**
     * @brief   Signal emitted after object destruction, prior the destruction
     *          of signal & slot connections.
     */
    Signal<> sigDestroy;
private:
    /**
     * @brief   Destroys this object and disconnects the slots from all signals.
     */
    void destroy();
    /**
     * @brief   Called when a signal is disconnected from one of our slots.
     * @param   signal The signal we are connected to.
     * @param   handle The slot handle which has been assigned to the object.
     */
    void onSignalConnected(internal::SignalBase* signal, SlotHandle handle) override;
    /**
     * @brief   Called when a signal is disconnected from one of our slots.
     * @param   signal The signal to disconnect from.
     * @param   handle The slot handle of the connection to be removed.
     */
    void onSignalDisconnected(internal::SignalBase* signal, SlotHandle handle) override;
};

// ============================================================================================== //

} // namespace zycore

#endif // _ZYCORE_SIGNALOBJECT_HPP_
