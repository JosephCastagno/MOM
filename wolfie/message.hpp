#pragma once 

#include <string>
#include <sstream>

#include "../xml/pugixml.hpp"

// general message structure
struct message_t {
    std::string m_topic;
    std::string m_data;

    message_t(std::string topic, std::string data) 
        : m_topic(std::move(topic)), m_data(std::move(data)) {} 
}; // message_t

std::ostream& operator<<(std::ostream& os, const message_t& message) {
    os << message.m_topic << "\n";
    os << message.m_data;
    return os;
}

// data field can be populated with serialized versions of the following structs
struct login_data_t {
    std::string m_username;
    bool m_is_admin;

    login_data_t(std::string s, bool admin) 
        : m_username(std::move(s)), m_is_admin(admin) {}

    std::string to_xml() {
        pugi::xml_document doc;

        pugi::xml_node login_node = doc.append_child("Login");
        login_node.append_attribute("admin") = m_is_admin ? "1" : "0";
        login_node.append_attribute("username") = m_username.c_str();

        std::ostringstream oss;
        doc.save(oss, "", pugi::format_no_declaration);

        return oss.str();
    }

}; // login_data_t
