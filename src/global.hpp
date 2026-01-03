//
// Created by morimoto_hibiki on 2025/12/27.
//

#ifndef FRONTAGE_CLIENT_GLOBAL_HPP
#define FRONTAGE_CLIENT_GLOBAL_HPP

#include <AHO/aho.hpp>

struct GlobalContext {
    vsl::Viewport viewport;
    vsl::Scissor scissor;
    vsl::PipelineLayout base_layout;
};


#endif //FRONTAGE_CLIENT_GLOBAL_HPP
