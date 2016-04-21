#pragma once
#include <zycore/Signal.hpp>
#include <zycore/Optional.hpp>
#include <zycore/Singleton.hpp>
#include <zycore/SignalObject.hpp>

#if SC2API_EXPORTS
#define SC2API_API __declspec (dllexport)
#else
#define SC2API_API __declspec (dllimport)
#endif

#pragma warning(disable: 4251)  //Currently, user DLL is expected to compile with same options as SC2API (/MD, /MDd)

/// <summary>
/// Type and thread safe signal implementation.
/// For more information, see Signals & Slots at wikipedia:
/// http://en.wikipedia.org/wiki/Signals_and_slots
/// </summary>
/// <param name="ArgsT">
/// The slot's argument types.
/// </param>
template<typename... ArgsT>
using Signal = zycore::Signal<ArgsT...>;

/// <summary>
/// Class representing "nullable" objects which may not (yet) have a value assigned.
/// </summary>
/// <param name="T">The value type of the optional.</param>
template<typename T>
using Optional = zycore::Optional<T>;

/// <summary>
/// Singleton template
/// </summary>
/// <param name="T">The type of singleton class</param>
template<typename T>
using Singleton = zycore::Singleton<T>;

/// <summary>
/// Base class for all objects whose methods should be able to serve as slots to be connected to a signal.
/// </summary>
using SignalObject = zycore::SignalObject;