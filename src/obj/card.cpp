//
// Created by morimoto_hibiki on 2025/12/11.
//

#include "card.hpp"

std::map<int, Image> Card::numbers_repo;
std::map<std::string, CardResource> Card::resource_repo;

bool Card::register_resource(std::string name, std::string illust_file_path, std::string description) {

    return true;
}

bool Card::init_numbers_repo() {
    for (int i = 0; i <= 30; i++) {
        numbers_repo;
    }
    return true;
}

void Card::draw(aho::DrawPhase &p) {

}
