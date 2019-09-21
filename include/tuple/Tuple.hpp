#pragma once

#include "TupleElement.hpp"
#include "Meta/ValueList.hpp"
#include "Meta/TypeList.hpp"

namespace IDragnev
{
    template <typename... Types>
    class Tuple;

    namespace Detail
    {
        template <typename TupleT>
        struct TupleSize { };

        template <typename... Types>
        struct TupleSize<Tuple<Types...>> : Meta::CTValue<std::size_t, sizeof...(Types)> { };

        template <typename T, typename = std::void_t<>>
        struct IsTuple : std::false_type { };

        template <typename T>
        struct IsTuple<T, std::void_t<decltype(TupleSize<std::decay_t<T>>::value)>> :
            std::true_type { };
    }
    
    template <typename T>
    inline constexpr auto tupleSize = Detail::TupleSize<std::decay_t<T>>::value;
    
    template <typename T>
    inline constexpr bool isTuple = Detail::IsTuple<T>::value;

    template <std::size_t Index,
              typename TupleT,
              std::size_t Size = tupleSize<TupleT>
    > inline constexpr
    decltype(auto) get(TupleT&& tuple) noexcept
    {
        return Detail::getValue<Size - Index - 1>(std::forward<TupleT>(tuple));
    }

    template <>
    class Tuple<> { };

    template <typename Head, typename... Tail>
    class Tuple<Head, Tail...>
        : private Detail::TupleElement<sizeof...(Tail), Head>,
          private Tuple<Tail...>
    {
    private:
        using HeadElement = Detail::TupleElement<sizeof...(Tail), Head>;
        using TailTuple = Tuple<Tail...>;

        template <typename... Args>
        using EnableIfMatchesTailLength = std::enable_if_t<sizeof...(Args) == sizeof...(Tail)>;

    public:
        Tuple() = default;
        Tuple(Tuple&& source) = default;
        Tuple(const Tuple& source) = default;
        ~Tuple() = default;

        template <typename VHead,
                  typename... VTail,
                  typename = EnableIfMatchesTailLength<VTail...>
        > constexpr Tuple(VHead&& head, VTail&&... tail);

        template <typename VHead,
                  typename... VTail,
                  typename = EnableIfMatchesTailLength<VTail...>
        > constexpr Tuple(const Tuple<VHead, VTail...>& source);

        template <typename VHead,
                  typename... VTail,
                  typename = EnableIfMatchesTailLength<VTail...>
        > constexpr Tuple(Tuple<VHead, VTail...>&& source);

        Tuple& operator=(Tuple&& rhs) = default;
        Tuple& operator=(const Tuple& rhs) = default;

        template <typename VHead,
                  typename... VTail,
                  typename = EnableIfMatchesTailLength<VTail...>
        > constexpr Tuple& operator=(const Tuple<VHead, VTail...>& source);

        template <typename VHead,
                  typename... VTail,
                  typename = EnableIfMatchesTailLength<VTail...>
        > constexpr Tuple& operator=(Tuple<VHead, VTail...>&& source);

        constexpr Head& getHead() & noexcept;
        constexpr Head&& getHead() && noexcept;
        constexpr const Head& getHead() const& noexcept;

        constexpr TailTuple& getTail() & noexcept;
        constexpr TailTuple&& getTail() && noexcept;
        constexpr const TailTuple& getTail() const& noexcept;

        template <std::size_t Index, typename TupleT, std::size_t Size>
        friend constexpr decltype(auto) get(TupleT&& tuple) noexcept;
    };
}

#include "TupleImpl.hpp"
#include "TupleUtilities.hpp"