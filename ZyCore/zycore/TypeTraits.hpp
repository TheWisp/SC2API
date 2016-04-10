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

#ifndef _ZYCORE_TYPETRAITS_HPP_
#define _ZYCORE_TYPETRAITS_HPP_

#include "zycore/Types.hpp"
#include "zycore/Mpl.hpp"

#include <type_traits>

namespace zycore
{

// ============================================================================================== //
// [CloneConst]                                                                                   //
// ============================================================================================== //

/**
 * @brief   Clones the const qualifier from one type to another.
 * @tparam  SrcT    Source type.
 * @tparam  DstT    Destination type.
 */
template<typename SrcT, typename DstT>
using CloneConst = std::conditional_t<
    std::is_const<SrcT>::value, 
    std::add_const_t<DstT>,
    std::remove_const_t<DstT>
>;

// ============================================================================================== //
// [CloneVolatile]                                                                                //
// ============================================================================================== //

/**
 * @brief   Clones the volatile qualifier from one type to another.
 * @tparam  SrcT    Source type.
 * @tparam  DstT    Destination type.
 */
template<typename SrcT, typename DstT>
using CloneVolatile = std::conditional_t<
    std::is_volatile<SrcT>::value, 
    std::add_volatile_t<DstT>,
    std::remove_volatile_t<DstT>
>;

// ============================================================================================== //
// [CloneCv]                                                                                      //
// ============================================================================================== //

/**
 * @brief   Clones the const and volatile  qualifiers from one type to another.
 * @tparam  SrcT    Source type.
 * @tparam  DstT    Destination type.
 */
template<typename SrcT, typename DstT>
using CloneCv = typename CloneVolatile<SrcT, CloneConst<SrcT, DstT>>::Type;

// ============================================================================================== //
// [InheritIfFlags]                                                                               //
// ============================================================================================== //

namespace internal
{

template<typename T, bool doInheritT>
struct InheritIfImpl {};

template<typename T>
struct InheritIfImpl<T, true> : T {};

} // namespace internal

/**
 * @brief   Inherits @c T if @c flagsT\ &\ flagConditionT evaluates to @c true.
 * @tparam  flagsT          The flags.
 * @tparam  flagConditionT  The flags that need to be @c true to inherit.
 * @tparam  T               The class to inherit if the required flags are set.
 */
template<Flags flagsT, Flags flagConditionT, typename T>
using InheritIfFlags 
    = internal::InheritIfImpl<T, (flagsT & flagConditionT) == flagConditionT>;

// ============================================================================================== //
// [IsMovable]                                                                                    //
// ============================================================================================== //

/**
 * @brief   Determines if a type is move assign and constructable.
 * @tparam  T   The type to check.
 */
template<typename T>
struct IsMovable
    : std::integral_constant<
        bool, 
        std::is_move_assignable<T>::value && std::is_move_constructible<T>::value
    > 
{};

// ============================================================================================== //
// [IsCopyable]                                                                                   //
// ============================================================================================== //

/**
 * @brief   Determines if a type is copy assign and constructable.
 * @tparam  T   The type to check.
 */
template<typename T>
struct IsCopyable
    : std::integral_constant<
        bool,
        std::is_copy_assignable<T>::value && std::is_copy_constructible<T>::value
    >
{};

// ============================================================================================== //
// [IsAnyOf]                                                                                      //
// ============================================================================================== //

namespace internal
{
    struct IsAnyOfImplTrue
    {
        static const bool kValue = true;
    };

    template<typename ComperandT, typename OthersT>
    struct IsAnyOfImpl
    {
        static const bool kValue = std::conditional_t<
            std::is_same<ComperandT, OthersT::Top>::value,
            IsAnyOfImplTrue,
            IsAnyOfImpl<ComperandT, OthersT::PopFront>
        >::kValue;
    };

    template<typename ComperandT>
    struct IsAnyOfImpl<ComperandT, mpl::Vector<>>
    {
        static const bool kValue = false;
    };
}

/**
 * @brief   Determines if a type is one of one or more other types.
 * @tparam  ComperandT  The type whose existence in the list should be checked.
 * @tparam  OthersT     The list of types to check @c ComperandT against.
 */
template<typename ComperandT, typename... OthersT>
using IsAnyOf = internal::IsAnyOfImpl<ComperandT, mpl::Vector<OthersT...>>;

// ============================================================================================== //
// [AnalyzeQualifiers]                                                                            //
// ============================================================================================== //

namespace internal
{

// Final layer implementation
template<typename T, typename LayerStackT>
struct AnalyzeQualifiersFinalImpl
{
    using QualifierStack = LayerStackT;
    using BaseType = T;
    static const std::size_t kDepth = LayerStackT::kSize;
};

// Plain type (+ CV)
template<typename T, typename LayerStackT>
struct AnalyzeQualifiersImpl
    : AnalyzeQualifiersFinalImpl<T, LayerStackT> {};

template<typename T, typename LayerStackT>
struct AnalyzeQualifiersImpl<const T, LayerStackT> 
    : AnalyzeQualifiersFinalImpl<const T, LayerStackT> {};

template<typename T, typename LayerStackT>
struct AnalyzeQualifiersImpl<volatile T, LayerStackT> 
    : AnalyzeQualifiersFinalImpl<volatile T, LayerStackT> {};

template<typename T, typename LayerStackT>
struct AnalyzeQualifiersImpl<const volatile T, LayerStackT> 
    : AnalyzeQualifiersFinalImpl<const volatile T, LayerStackT> {};

#define ZYCORE_ANALYZEQUALIFIERS_SPEC(spec)                                                        \
    template<typename T, typename LayerStackT>                                                     \
    struct AnalyzeQualifiersImpl<T spec, LayerStackT>                                              \
        : AnalyzeQualifiersImpl<T, typename LayerStackT::template Push<int spec>> {};

// Pointer layers (+ CV)
ZYCORE_ANALYZEQUALIFIERS_SPEC(*)
ZYCORE_ANALYZEQUALIFIERS_SPEC(* const)
ZYCORE_ANALYZEQUALIFIERS_SPEC(* volatile)
ZYCORE_ANALYZEQUALIFIERS_SPEC(* const volatile)

// lvalue-reference layers
ZYCORE_ANALYZEQUALIFIERS_SPEC(&)

// rvalue-reference layers
ZYCORE_ANALYZEQUALIFIERS_SPEC(&&)

// Unknown-size arrays
ZYCORE_ANALYZEQUALIFIERS_SPEC([])

#undef ZYCORE_ANALYZEQUALIFIERS_SPEC

// Known-size arrays
template<typename T, typename LayerStackT, std::size_t N>
struct AnalyzeQualifiersImpl<T[N], LayerStackT>
    : AnalyzeQualifiersImpl<T, typename LayerStackT::template Push<int[N]>> {};

} // namespace internal

/**
 * @brief   Analyzes a type's qualifiers
 * @tparam  T   The subject of the analysis.
 *              
 * All qualifier-levels are processed and pushed onto a stack (@c QualifierStack) by applying them
 * to a holder-type (@c int). Pointer layers may also come with additional CV-qualifications. 
 * The base-type can be retrieved from @c BaseType, the amount of qualification layers is stored 
 * into the @c kDepth constant.
 * 
 * Example:
 * @code
 *      using Result = AnalyzeQualifiers<float const (*volatile &) [42]>;
 *      std::cout << "BaseType: " << typeid(Result::BaseType).name() << std::endl;
 *      std::cout << "QualifierStack: " << typeid(Result::QualifierStack).name() << std::endl;
 *      std::cout << "Depth: " << static_cast<Result::DepthType>(Result::kDepth) << std::endl;
 *      
 *      // Output:
 *      // BaseType: float
 *      // QualifierStack: struct mpl::Stack<int &,int * volatile,int const[42]>
 *      // Depth: 3
 * @endcode
 */
template<typename T>
using AnalyzeQualifiers = internal::AnalyzeQualifiersImpl<T, mpl::Stack<>>;

// ============================================================================================== //
// [CloneQualifiers]                                                                              //
// ============================================================================================== //

namespace internal
{

// Plain types (+ CV)
template<typename DstT, typename SrcT>
struct CloneQualifierImpl
{
    using Type = DstT;
};

template<typename DstT, typename SrcT>
struct CloneQualifierImpl<DstT, const SrcT>
{
    using Type = const DstT;
};

template<typename DstT, typename SrcT>
struct CloneQualifierImpl<DstT, volatile SrcT>
{
    using Type = volatile DstT;
};

template<typename DstT, typename SrcT>
struct CloneQualifierImpl<DstT, const volatile SrcT>
{
    using Type = const volatile DstT;
};

#define ZYCORE_CLONEQUALIFIER_SPEC(spec)                                                           \
    template<typename DstT, typename SrcT>                                                         \
    struct CloneQualifierImpl<DstT, SrcT spec>                                                     \
    {                                                                                              \
        using Type = DstT spec;                                                                    \
    };

// Pointer layers (+ CV)
ZYCORE_CLONEQUALIFIER_SPEC(*);
ZYCORE_CLONEQUALIFIER_SPEC(* const);
ZYCORE_CLONEQUALIFIER_SPEC(* volatile);
ZYCORE_CLONEQUALIFIER_SPEC(* const volatile);

// lvalue-qualifiers
ZYCORE_CLONEQUALIFIER_SPEC(&)

// rvalue-qualifiers
ZYCORE_CLONEQUALIFIER_SPEC(&&)

// Unknown-size array qualifiers
ZYCORE_CLONEQUALIFIER_SPEC([]);

#undef ZYCORE_CLONEQUALIFIER_SPEC

// Known-size array qualifiers
template<typename DstT, typename SrcT, std::size_t N>
struct CloneQualifierImpl<DstT, SrcT[N]>
{
    using Type = DstT[N];
};

} // namespace internal

/**
 * @brief   Clones the first layer of qualifiers from one type to another.
 * @tparam  DstT    The destination type the qualifiers are applied to.
 * @tparam  SrcT    The source type the qualifiers are cloned from.
 */
template<typename DstT, typename SrcT>
using CloneQualifier = typename internal::CloneQualifierImpl<DstT, SrcT>::Type;

// ============================================================================================== //
// [ApplyQualifierStack]                                                                          //
// ============================================================================================== //

namespace internal
{
    
// Implementation capturing on an exhausted stack.
template<typename DstT, typename QualifierStackT, typename=void>
struct ApplyQualifierStackImpl
{
    using Type = DstT;
};

// Implementation capturing if there are elements left to process
template<typename DstT, typename QualifierStackT>
struct ApplyQualifierStackImpl<DstT, QualifierStackT, std::enable_if_t<!QualifierStackT::kEmpty>>
    : ApplyQualifierStackImpl<
        CloneQualifier<DstT, typename QualifierStackT::Top>,
        typename QualifierStackT::Pop
    >
{};

} // namespace internal

/**
 * @brief   Applies a stack of qualifiers to a type.
 * @tparam  DstT            The destination type the qualifiers are applied to.
 * @tparam  BaseTypeT       Source type for CV-qualifiers on the target base-type.
 * @tparam  QualifierStackT A stack of additional qualifiers applied to the type.
 */
template<typename DstT, typename BaseTypeT, typename QualifierStackT>
using ApplyQualifierStack 
    = typename internal::ApplyQualifierStackImpl<
        CloneQualifier<
            std::remove_const_t<
                std::remove_volatile_t<
                    typename AnalyzeQualifiers<DstT>::BaseType
                    >
                >,
            BaseTypeT
        >,
    QualifierStackT
>::Type;

// ============================================================================================== //

} // namespace zycore

#endif // _ZYCORE_TYPETRAITS_HPP_
