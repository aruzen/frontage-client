//
// Created by morimoto_hibiki on 2026/01/01.
//

#ifndef FRONTAGE_CLIENT_OPERATORS_HPP
#define FRONTAGE_CLIENT_OPERATORS_HPP

#include <AHO/aho.hpp>

template<typename R, typename CI>
bool in(aho::_Vector<R, vsl::D2, CI> lt, aho::_Vector<R, vsl::D2, CI> rb, aho::_Vector<R, vsl::D2, CI> target) {
    return (lt.value.___AN1 < target.value.___AN1 && target.value.___AN1 < rb.value.___AN1)
           && (lt.value.___AN2 < target.value.___AN2 && target.value.___AN2 < rb.value.___AN2);
}

#endif //FRONTAGE_CLIENT_OPERATORS_HPP
