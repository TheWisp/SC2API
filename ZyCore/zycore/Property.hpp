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

#ifndef _ZYCORE_PROPERTY_HPP_
#define _ZYCORE_PROPERTY_HPP_

#ifdef ZYCORE_HEADER_ONLY
#   error "This file cannot be used in header-only mode."
#endif // ZYCORE_HEADER_ONLY

#include "zycore/ReflectableObject.hpp"
#include "zycore/Exceptions.hpp"

#include <string>
#include <unordered_map>
#include <sstream>
#include <algorithm>

namespace zycore
{

// ============================================================================================== //
// [PropertyBase]                                                                                 //
// ============================================================================================== //
/**
 * @brief   Abstract base class for properties.
 * Provides raw-data access, type information and value access using strings.
 */
class PropertyBase : public ReflectableObject
{
    std::string m_name;
    ReflectableObject* m_owner;
public:
    /**
     * @brief   Constructor.
     * @param   owner   The owner object of this property. May NOT be @c nullptr.
     * @param   name    The name of the property.
     */
    PropertyBase(ReflectableObject* owner, const std::string& name);
    /**
     * @brief   Destructor.
     */
    virtual ~PropertyBase();
public:
    /**
     * @brief   Sets the property using a string.
     * @param   val The new value as string.
     * @throws  NotImplemented Implementations may throw this exception if they decide not to 
     *                         implement this method. The default implementation does throw this 
     *                         exception.
     */
    virtual void fromString(const std::string& val);
    /**
     * @brief   Obtains the string representation of the property.
     * @return  The property value as string.
     * @throws  NotImplemented Implementations may throw this exception if they decide not
     *                         to implement this method.
     *                           
     * The default implementation returns a string similar to the following:
     * @code
     *      <Object at 0x12345678>
     * @endcode
     */
    virtual std::string toString() const;
    /**
     * @brief   Gets the name of the property.
     * @return  The name of the property.
     */
    virtual const std::string& name() const;
    /**
     * @brief   Gets the type of the property.
     * @return  The property type.
     */
    virtual const std::string& typeName() const = 0;
    /**
     * @brief   Gets the raw pointer to the property's data.
     * @return  A raw pointer.
     */
    virtual const void* rawData() const = 0;
    /**
     * @brief   Gets the length of the raw data of the property.
     * @return  The raw data size.
     */
    virtual size_t rawDataLen() const = 0;
public:
    /**
     * @brief   Gets the owner of the property.
     * @return  The owner.
     */
    const ReflectableObject* owner() const;
    /**
     * @overload
     */
    ReflectableObject* owner();
};

// ============================================================================================== //
// Implementation of inline methods [PropertyBase]                                                //
// ============================================================================================== //

inline const ReflectableObject* PropertyBase::owner() const
{
    return m_owner;
}

inline ReflectableObject* PropertyBase::owner()
{
    return m_owner;
}

// ============================================================================================== //
// [PropertyTemplatedBase&  PropertyImplementation]                                               //
// ============================================================================================== //

template<typename T>
class PropertyTemplatedBase : public PropertyBase
{
public:
    using Setter = std::function<void(const T&)>;
    using Getter = std::function<const T&()>;
protected:
    T& m_value;
    Setter m_setter;
    Getter m_getter;
public:
    /**
     * @brief   Constructor.
     * @param   owner   The owner object of this property. May NOT be @c nullptr.
     * @param   name    The name of the property.
     * @param   member  The variable to represent with this property.
     * @param   getter  The getter called to obtain the value. See @c defaultGetter.
     * @param   setter  The setter called to set the value. See @c defaultSetter.
     */
    PropertyTemplatedBase(ReflectableObject* owner, const std::string& name, T& member, 
        Getter getter, Setter setter);
    /**
     * @brief   Destructor.
     */
    ~PropertyTemplatedBase() override = default;
public:
    const void* rawData() const override { return& m_value;  }
    size_t rawDataLen() const override { return sizeof(T); }
public:
    // TODO: find a better solution here - for objects without assignment
    //       operator defined this will cause errors.
    /**
     * @brief   The default setter.
     * @param   newValue The new value.
     * The default setter just uses the assignment operator to set the value.
     */
    void defaultSetter(const T& newValue);
    /**
     * @brief   The default getter.
     * @return  A constant reference on the value.
     * The default getter just returns a reference to the value.
     */
    const T& defaultGetter() const;
public: // Access to the reflected object using the property.
    /**
     * @brief   Sets a new value for the object reflected by the property.
     * @param   newValue    The new value to set.
     */
    void set(const T& newValue);
    /**
     * @brief   Gets the current value of the object reflected by the property.
     * @return  Current value of the property's object.
     */
    const T& get() const;
};

template<typename T>
class PropertyImplementation
{
    static_assert(BlackBoxConsts<T>::kFalse, 
        "no property specialization found for given type");
};

// ============================================================================================== //
// Implementation of template methods [PropertyTemplatedBase]                                     //
// ============================================================================================== //

template<typename T>
inline PropertyTemplatedBase<T>::PropertyTemplatedBase(
    ReflectableObject* owner, const std::string& name, T& member, Getter getter, Setter setter)
    : PropertyBase(owner, name)
    , m_value(member)
    , m_setter(setter)
    , m_getter(getter)
{
    
}

template<typename T>
inline const T& PropertyTemplatedBase<T>::defaultGetter() const
{
    return m_value;
}

template<typename T>
void PropertyTemplatedBase<T>::set(const T& newValue)
{
    m_setter(newValue);
}

template<typename T>
const T& PropertyTemplatedBase<T>::get() const
{
    return m_getter();
}

template<typename T>
inline void PropertyTemplatedBase<T>::defaultSetter(const T& newValue)
{
    m_value = newValue;
}

// ============================================================================================== //
// [Property]                                                                                     //
// ============================================================================================== //

template<typename T>
class Property : public PropertyImplementation<T>
{
public:
    /**
     * @brief   Constructor.
     * @param   owner   The owner object of this property. May NOT be @c nullptr.
     * @param   name    The name of the property.
     * @param   member  The variable to represent with this property.
     */
    Property(ReflectableObject* owner, const std::string& name, T& member);
    /**
     * @brief   Constructor.
     * @param   owner   The owner object of this property. May NOT be @c nullptr.
     * @param   name    The name of the property.
     * @param   member  The variable to represent with this property.
     * @param   getter  The getter called to obtain the value. See 
     *                  @c defaultGetter.
     */
    Property(ReflectableObject* owner, const std::string& name, T& member, 
        typename PropertyTemplatedBase<T>::Getter getter);
    /**
     * @brief   Constructor.
     * @param   owner   The owner object of this property. May NOT be @c nullptr.
     * @param   name    The name of the property.
     * @param   member  The variable to represent with this property.
     * @param   setter  The setter called to set the value. See 
     *                  @c defaultSetter.
     */
    Property(ReflectableObject* owner, const std::string& name, T& member, 
        typename PropertyTemplatedBase<T>::Setter setter);
    /**
     * @brief   Constructor.
     * @param   owner   The owner object of this property. May NOT be @c nullptr.
     * @param   name    The name of the property.
     * @param   member  The variable to represent with this property.
     * @param   getter  The getter called to obtain the value. See 
     *                  @c defaultGetter.
     * @param   setter  The setter called to set the value. See 
     *                  @c defaultSetter.
     */
    Property(ReflectableObject* owner, const std::string& name, T& member, 
        typename PropertyTemplatedBase<T>::Getter getter, 
        typename PropertyTemplatedBase<T>::Setter setter);
};

// ============================================================================================== //
// Implementation of inline methods [Property]                                                    //
// ============================================================================================== //

template<typename T>
inline Property<T>::Property(ReflectableObject* owner, const std::string& name, T& member)
    : PropertyImplementation<T>(owner, name, member, 
        std::bind(&PropertyTemplatedBase<T>::defaultGetter, this), 
        std::bind(&PropertyTemplatedBase<T>::defaultSetter, this, std::placeholders::_1))
{}

template<typename T>
inline Property<T>::Property(ReflectableObject* owner, const std::string& name, T& member, 
        typename PropertyTemplatedBase<T>::Getter getter)
    : PropertyImplementation<T>(owner, name, member, getter, 
        std::bind(&PropertyTemplatedBase<T>::defaultSetter, this, std::placeholders::_1))
{}

template<typename T>
inline Property<T>::Property(ReflectableObject* owner, const std::string& name, T& member, 
        typename PropertyTemplatedBase<T>::Setter setter)
    : PropertyImplementation<T>(owner, name, member, 
        std::bind(&PropertyTemplatedBase<T>::defaultGetter, this), setter)
{}

template<typename T>
inline Property<T>::Property(ReflectableObject* owner, const std::string& name, T& member, 
        typename PropertyTemplatedBase<T>::Getter getter, 
        typename PropertyTemplatedBase<T>::Setter setter)
    : PropertyImplementation<T>(owner, name, member, getter, setter)
{}

// ============================================================================================== //
// [Basic property types]                                                                         //
// ============================================================================================== //

#define ZYCORE_NUMERIC_PROPERTY(type, converter)                                                   \
    template<>                                                                                     \
    class PropertyImplementation<type> : public PropertyTemplatedBase<type>                        \
    {                                                                                              \
    public:                                                                                        \
        PropertyImplementation(ReflectableObject* owner, const std::string& name,                  \
                type& member, Getter getter, Setter setter)                                        \
            : PropertyTemplatedBase<type>(owner, name, member, getter, setter) {}                  \
    public:                                                                                        \
        void fromString(const std::string& val) override                                           \
        {                                                                                          \
            try                                                                                    \
            {                                                                                      \
                m_setter(converter(val));                                                          \
            } catch (const std::exception&)                                                        \
            {                                                                                      \
                throw InvalidUsage("invalid value provided");                                      \
            }                                                                                      \
        }                                                                                          \
                                                                                                   \
        std::string toString() const override                                                      \
        {                                                                                          \
            return std::to_string(m_getter());                                                     \
        }                                                                                          \
                                                                                                   \
        const std::string& typeName() const override                                               \
        {                                                                                          \
            static const std::string typeName(#type);                                              \
            return typeName;                                                                       \
        }                                                                                          \
    };

ZYCORE_NUMERIC_PROPERTY(int, std::stoi);
ZYCORE_NUMERIC_PROPERTY(unsigned int, std::stoul);
ZYCORE_NUMERIC_PROPERTY(short, (short)std::stoi);
ZYCORE_NUMERIC_PROPERTY(unsigned short, (unsigned short)std::stoul);
ZYCORE_NUMERIC_PROPERTY(long, std::stol);
ZYCORE_NUMERIC_PROPERTY(unsigned long, std::stoul);
ZYCORE_NUMERIC_PROPERTY(long long, std::stoll);
ZYCORE_NUMERIC_PROPERTY(unsigned long long, std::stoull);
ZYCORE_NUMERIC_PROPERTY(float, std::stof);
ZYCORE_NUMERIC_PROPERTY(double, std::stod);
ZYCORE_NUMERIC_PROPERTY(long double, std::stold);
ZYCORE_NUMERIC_PROPERTY(unsigned char, (unsigned char)std::stoul);

#undef ZYCORE_NUMERIC_PROPERTY

template<>
class PropertyImplementation<bool> : public PropertyTemplatedBase<bool>
{
public:
    PropertyImplementation(ReflectableObject* owner, const std::string& name, bool& member, 
        Getter getter, Setter setter) : PropertyTemplatedBase<bool>(owner, name, member, getter,
        setter) {}
public:
    void fromString(const std::string& val) override
    {
        m_setter(!(val == "false" || val == "0"));
    }

    std::string toString() const override
    {
        return m_getter() ? "true" : "false";
    }

    const std::string& typeName() const override
    {
        static const std::string typeName("bool");
        return typeName;
    }
};

template<>
class PropertyImplementation<std::string> 
    : public PropertyTemplatedBase<std::string>
{
public:
    PropertyImplementation(ReflectableObject* owner, const std::string& name, 
            std::string& member, Getter getter, Setter setter) 
        : PropertyTemplatedBase<std::string>(owner, name, member, getter, setter) {}
public:
    void fromString(const std::string& val) override
    {
        m_setter(val);
    }

    std::string toString() const override
    {
        return m_getter();
    }

    const std::string& typeName() const override
    {
        static const std::string typeName("std::string");
        return typeName;
    }
};

// ============================================================================================== //
// Macro magic for easy creation of enum properties                                               //
// ============================================================================================== //

namespace internal
{
    template<typename T>
    std::unordered_map<T, std::string> enumValuesToString(const std::string& enumValues)
    {
        std::istringstream                 ss(enumValues);
        std::unordered_map<T, std::string> map;
        std::string                        curItem;
        std::underlying_type_t<T>          enumVal = 0;

        while (std::getline(ss, curItem, ','))
        {
            curItem.erase(std::remove(curItem.begin(), curItem.end(), ' '));
            map[static_cast<T>(enumVal++)] = curItem;
        }
        return map;
    }

    template<typename K, typename V>
    std::unordered_map<V, K> mapReverseKeyValue(const std::unordered_map<K, V>& in)
    {
        std::unordered_map<V, K> out;
        for (const auto& curPair : in)
        {
            out[curPair.second] = curPair.first;
        }
        return out;
    }

}

#define ZYCORE_DECLARE_EXISTING_ENUM_PROPERTY(enumName, ...)                                       \
    template<>                                                                                     \
    class PropertyImplementation<enumName> : public PropertyTemplatedBase<enumName>                \
    {                                                                                              \
        const std::unordered_map<enumName, std::string> m_valToNameMap                             \
            = internal::enumValuesToString<enumName>(#__VA_ARGS__);                                \
        const std::unordered_map<std::string, enumName> m_nameToValMap                             \
            = internal::mapReverseKeyValue(m_valToNameMap);                                        \
    public:                                                                                        \
        PropertyImplementation(AMUiObject* owner, const std::string& name, enumName& member,       \
            Getter getter, Setter setter) : PropertyTemplatedBase<enumName>(owner, name,           \
            member, getter, setter) {}                                                             \
    public:                                                                                        \
        void fromString(const std::string& val) override                                           \
        {                                                                                          \
            auto valIt = m_nameToValMap.find(val);                                                 \
            if (valIt == m_nameToValMap.end())                                                     \
                throw InvalidUsage("invalid enum value");                                          \
            m_setter(valIt->second);                                                               \
        }                                                                                          \
                                                                                                   \
        std::string toString() const override                                                      \
        {                                                                                          \
            auto nameIt = m_valToNameMap.find(m_getter());                                         \
            assert(nameIt != m_valToNameMap.end());                                                \
            return nameIt->second;                                                                 \
        }                                                                                          \
                                                                                                   \
        const std::string& typeName() const override                                               \
        {                                                                                          \
            static const std::string typeName(#enumName);                                          \
            return typeName;                                                                       \
        }                                                                                          \
    };

#define ZYCORE_DECLARE_ENUM_PROPERTY(enumName, ...)                                                \
    enum enumName { __VA_ARGS__ };                                                                 \
    ZYCORE_DECLARE_EXISTING_ENUM_PROPERTY(enumName, __VA_ARGS__);

// ============================================================================================== //

} // namespace zycore

#endif // _ZYCORE_PROPERTY_HPP_
