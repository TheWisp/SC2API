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

#ifndef _ZYCORE_REFLECTABLEOBJECT_HPP_
#define _ZYCORE_REFLECTABLEOBJECT_HPP_

#ifdef ZYCORE_HEADER_ONLY
#   error "This file cannot be used in header-only mode."
#endif // ZYCORE_HEADER_ONLY

#include "zycore/SignalObject.hpp"
#include "zycore/Optional.hpp"

#include <mutex>

namespace zycore
{

class PropertyBase;

// ============================================================================================== //
// [ReflectableObject]                                                                            //
// ============================================================================================== //

class ReflectableObject : public SignalObject
{
    std::unique_ptr<std::string> m_objectName;
    std::vector<PropertyBase*> m_propertyList;
    mutable std::recursive_mutex m_propertyListLock;
public: // Public interface.
    /**
     * @brief   Constructor.
     */
    ReflectableObject() = default;
    /**
     * @brief   Destructor.
     * @remarks This routine is thread-safe.
     */
    virtual ~ReflectableObject() = default;
    /**
     * @brief   Gets the object name.
     * @return  The object name if one is set.
     */
    Optional<const std::string&> objectName() const;
    /**
     * @brief   Sets the object name.
     * @param   name The new object name. May not be empty.
     */
    void setObjectName(const std::string& name);
public: // Reflection.
    /**
     * @brief   Obtains the list of properties of this object.
     * @return  The desired list.
     * @remarks This routine is thread-safe.
     */
    std::vector<PropertyBase*>& properties();
    /**
     * @overload
     */
    const std::vector<PropertyBase*>& properties() const;
private: // Internal interface.
    friend PropertyBase;
    /**
     * @brief   Registers a given property to this object.
     * @param   prop    The property to register.
     */
    void registerProperty(PropertyBase *prop);
    /**
     * @brief   Unregisters a given property from this object.
     * @param   prop    The property to unregister.
     */
    void unregisterProperty(PropertyBase *prop);
};

// ============================================================================================== //
// Implementation of inline methods [ReflectableObject]                                           //
// ============================================================================================== //

inline std::vector<PropertyBase*>& ReflectableObject::properties()
{
    std::lock_guard<std::recursive_mutex> lock(m_propertyListLock);
    return m_propertyList;
}

inline const std::vector<PropertyBase*>& ReflectableObject::properties() const
{
    std::lock_guard<std::recursive_mutex> lock(m_propertyListLock);
    return m_propertyList;
}
// ============================================================================================== //

} // namespace zycore

#endif // _ZYCORE_REFLECTABLEOBJECT_HPP_