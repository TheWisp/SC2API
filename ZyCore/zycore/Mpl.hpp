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

#ifndef _ZYCORE_TMP_HPP_
#define _ZYCORE_TMP_HPP_

#include <type_traits>

namespace zycore
{
namespace mpl
{

/**   
 * @brief   End type indicating that there are no more elements in a container.
 */
class End {};

// ============================================================================================== //
// [Stack]                                                                                        //
// ============================================================================================== //

/**
 * @brief   Stack of types.
 * @tparam  ElementsT   Initial content of the stack, top element up front.
 */
template<typename... ElementsT>
class Stack final
{
private:
    /**
     * @brief   Push implementation.
     * @tparam  ItemT   The new item to push.
     */
    template<typename ItemT>
    struct PushImpl
    {
        using NewStack = Stack<ItemT, ElementsT...>;
    };

    /**
     * @brief   Pop implementation capturing on empty stacks.
     * @tparam  StackT  Type stack type.
     */
    template<typename StackT>
    struct PopImpl
    {
        using NewStack = StackT;
        using Item = End;
    };

    /**
     * @brief   Pop implementation capturing on stacks with elements.
     * @tparam  StackT  Type stack type.
     */
    template<template<typename...> class StackT, typename... ContentT, typename TopItemT>
    struct PopImpl<StackT<TopItemT, ContentT...>>
    {
        using NewStack = StackT<ContentT...>;
        using Item = TopItemT;
    };
public:
    template<typename ItemT>
    using Push = typename PushImpl<ItemT>::NewStack;

    using Pop = typename PopImpl<Stack<ElementsT...>>::NewStack;
    using Top = typename PopImpl<Stack<ElementsT...>>::Item;

    static const std::size_t kSize = sizeof...(ElementsT);
    static const bool kEmpty = kSize == 0;
};

// ============================================================================================== //
// [SliceView]                                                                                    //
// ============================================================================================== //

namespace internal
{
    
template<
    typename InContainerT,
    typename OutContainerT,
    std::size_t startT, 
    std::size_t endT,
    std::size_t curT,
    typename=void>
struct SliceViewImpl2
    : SliceViewImpl2<
        typename InContainerT::PopFront,
        std::conditional_t<
            curT >= startT && curT <= endT,
            typename OutContainerT::template PushFront<typename InContainerT::Top>,
            OutContainerT>,
        startT,
        endT,
        curT + 1>
{};

template<
    typename InContainerT,
    typename OutContainerT,
    std::size_t startT, 
    std::size_t endT,
    std::size_t curT>
struct SliceViewImpl2<
    InContainerT, 
    OutContainerT, 
    startT, endT, curT, 
    std::enable_if_t<InContainerT::kEmpty>>
{
    using Projection = OutContainerT;
};

template<typename ContainerT, std::size_t startT, std::size_t endT>
struct SliceViewImpl;

template<
    template<typename...> class ContainerT, 
    typename... ElementsT, 
    std::size_t startT, 
    std::size_t endT>
struct SliceViewImpl<ContainerT<ElementsT...>, startT, endT>
    : SliceViewImpl2<ContainerT<ElementsT...>, ContainerT<>, startT, endT, 0> {};

} // namespace internal

/**
 * @brief   Slices an MPL container.
 * @tparam  ContainerT  The container to be sliced.
 * @tparam  startT      The index to start slicing.
 * @tparam  endT        The index to stop slicing.
 */
template<typename ContainerT, std::size_t startT, std::size_t endT>
using SliceView = typename internal::SliceViewImpl<ContainerT, startT, endT>::Projection;

// ============================================================================================== //
// [Vector]                                                                                       //
// ============================================================================================== //

template<typename... ContentT>
class Vector
{
    template<typename ItemT>
    struct PushBackImpl
    {
        using NewContainer = Vector<ContentT..., ItemT>;
    };

    template<typename ItemT>
    struct PushFrontImpl
    {
        using NewContainer = Vector<ItemT, ContentT...>;
    };

    template<typename ContainerT>
    struct PopFrontImpl
    {
        using NewContainer = ContainerT;
        using Item = End;
    };

    template<typename... ElementsT, typename FrontItemT>
    struct PopFrontImpl<Vector<FrontItemT, ElementsT...>>
    {
        using NewContainer = Vector<ElementsT...>;
        using Item = FrontItemT;
    };
public:
    static const std::size_t kSize = sizeof...(ContentT);
    static const bool kEmpty = kSize == 0;

    template<typename ItemT>
    using PushFront = typename PushFrontImpl<ItemT>::NewContainer;
    template<typename ItemT>
    using PushBack = typename PushBackImpl<ItemT>::NewContainer;

    using PopFront = typename PopFrontImpl<Vector<ContentT...>>::NewContainer;
    using Top = typename PopFrontImpl<Vector<ContentT...>>::Item;

    using PopBack = SliceView<Vector<ContentT...>, 0, kSize - 1>;
    using Bottom = typename SliceView<Vector<ContentT...>, kSize - 1, kSize>::Top;
};

// ============================================================================================== //

} // namespace mpl
} // namespace zycore

#endif // _ZYCORE_TMP_HPP_
