//
// Created by morimoto_hibiki on 2025/12/11.
//

#ifndef FRONTAGE_CLIENT_CARD_HPP
#define FRONTAGE_CLIENT_CARD_HPP

#include <optional>
#include <VSL/vsl.hpp>
#include <AHO/aho.hpp>
#include "../resource/image.hpp"
#include <map>

class CardResource {
    vsl::GraphicResource resource;
    Image name;
    Image illustration;
    Image description;
};

struct Card {
    static std::map<int, Image> numbers_repo;
    static std::map<std::string, CardResource> resource_repo;
    static bool init_numbers_repo();
    static bool register_resource(std::string name, std::string illust_file_path, std::string description);

    const std::string resource_id;
    const int base_hp, base_atk;
    int hp, atk, mana;

    Card(std::string resource_id, int base_hp, int base_atk, int mana);

    void draw(aho::DrawPhase& p);
};


#endif //FRONTAGE_CLIENT_CARD_HPP
