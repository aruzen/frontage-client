//
// Created by morimoto_hibiki on 2026/01/04.
//

#ifndef FRONTAGE_CLIENT_DEFINE_HPP
#define FRONTAGE_CLIENT_DEFINE_HPP

#ifdef _MSC_VER
#define PATH_NORMALIZE(P) P
#elifdef __APPLE_CC__
#define PATH_NORMALIZE(P) "../" P
#endif

#endif //FRONTAGE_CLIENT_DEFINE_HPP
