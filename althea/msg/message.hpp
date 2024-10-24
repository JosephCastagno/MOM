#pragma once 

#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <string>
#include <sstream>

#include "message_types.hpp"
#include "message_utils.hpp"
#include "../xml/pugixml.hpp"

struct message_t {
    std::string m_topic;
    std::string m_data;

    message_t(std::string topic, std::string data)
        : m_topic(std::move(topic)), m_data(std::move(data)) {}

    message_t() = default;

    message_t(std::string topic, const message_data_t &data)
        : m_topic(std::move(topic)), m_data(std::move(data.serialize()))
    {}

    message_t(const std::string &xml) {
        const auto [doc, node] = 
            msg_utils::string_to_xml(xml, "Message");

        pugi::xml_node topic_node = node.child("Topic");
        if (!topic_node) {
            throw std::runtime_error("mismatched topic / data");
        }
        m_topic = topic_node.child_value();

        pugi::xml_node data_node = node.child("Data");
        if (!data_node) {
            throw std::runtime_error("mismatched topic / data");
        }

        std::ostringstream oss;
        for (pugi::xml_node child : data_node.children()) {
            child.print(oss, "", 
                        pugi::format_no_declaration | pugi::format_raw);
        }
        m_data = oss.str(); 
    }

    std::string serialize() const {
        pugi::xml_document doc;

        // topic
        pugi::xml_node message_node = doc.append_child("Message");
        message_node.append_child("Topic").text().set(m_topic.c_str());

        // data
        pugi::xml_node data_node = message_node.append_child("Data");
        data_node.append_buffer(m_data.c_str(), m_data.size());

        std::ostringstream oss;
        doc.save(oss, "", pugi::format_no_declaration);
        return oss.str();
    }
}; // message_t
