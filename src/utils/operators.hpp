//
// Created by morimoto_hibiki on 2026/01/01.
//

#ifndef FRONTAGE_CLIENT_OPERATORS_HPP
#define FRONTAGE_CLIENT_OPERATORS_HPP

#include <AHO/aho.hpp>

template<typename R, typename CI>
constexpr bool in(aho::_Vector<R, vsl::D2, CI> lt, aho::_Vector<R, vsl::D2, CI> rb, aho::_Vector<R, vsl::D2, CI> target) {
    return (lt.value.___AN1 < target.value.___AN1 && target.value.___AN1 < rb.value.___AN1)
           && (lt.value.___AN2 < target.value.___AN2 && target.value.___AN2 < rb.value.___AN2);
}

template<typename R, typename Dim, typename CI>
constexpr bool equales(aho::_Vector<R, Dim, CI> l, aho::_Vector<R, Dim, CI> r) {
    if constexpr (std::same_as<Dim, vsl::D1>) {
        return l.value.___AN1 == r.value.___AN1;
    } else if constexpr (std::same_as<Dim, vsl::D2>) {
        return l.value.___AN1 == r.value.___AN1 && l.value.___AN2 == r.value.___AN2;
    } else {
        return l.value.___AN1 == r.value.___AN1 && l.value.___AN2 == r.value.___AN2 && l.value.___AN3 == r.value.___AN3;
    }
}

template<typename R, typename Dim, typename CI>
constexpr bool equales(aho::_Point<R, Dim, CI> l, aho::_Point<R, Dim, CI> r) {
    if constexpr (std::same_as<Dim, vsl::D1>) {
        return l.value.___AN1 == r.value.___AN1;
    }
    else if constexpr (std::same_as<Dim, vsl::D2>) {
        return l.value.___AN1 == r.value.___AN1 && l.value.___AN2 == r.value.___AN2;
    }
    else {
        return l.value.___AN1 == r.value.___AN1 && l.value.___AN2 == r.value.___AN2 && l.value.___AN3 == r.value.___AN3;
    }
}

#endif //FRONTAGE_CLIENT_OPERATORS_HPP
