#pragma once

#include "../xml/pugixml.hpp"

namespace msg_utils {
    inline std::pair<pugi::xml_document, pugi::xml_node> string_to_xml(
        const std::string &xml,
        const std::string &topic_key) 
    {
        std::cout << "attempted xml conversion" << std::endl;
        std::cout << xml << std::endl;
        pugi::xml_document doc;
        pugi::xml_parse_result parse_result = doc.load_string(xml.c_str());

        if (!parse_result) {
            throw std::runtime_error(
                "malformed xml: " + std::string(parse_result.description()));
        }

        pugi::xml_node data_node = doc.child(topic_key.c_str());
        if (!data_node) {
            throw std::runtime_error("mismatched topic / data");
        }

        return std::make_pair(std::move(doc), data_node);
    }
} // namespace msg_utils

