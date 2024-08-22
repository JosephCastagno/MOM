#pragma once 

#include <chrono>
#include <ctime>
#include <iomanip>
#include <string>
#include <sstream>
#include <variant>

#include "../xml/pugixml.hpp"

struct pulse_data_t {
    std::chrono::time_point<std::chrono::system_clock> m_timestamp;

    pulse_data_t() : m_timestamp(std::chrono::system_clock::now()) {}

    std::string to_xml() const {
        pugi::xml_document doc;

        pugi::xml_node pulse_node = doc.append_child("Pulse");

        const std::time_t time = 
            std::chrono::system_clock::to_time_t(m_timestamp);
        const std::tm* tm = std::localtime(&time);

        std::stringstream ss;
        ss << std::put_time(tm, "%Y-%m-%d %H:%M:%S");
        pulse_node.append_attribute("timestamp") = ss.str().c_str();

        std::ostringstream oss;
        doc.save(oss, "", pugi::format_no_declaration);

        return oss.str();
    }
}; // pulse_data_t 

struct shutdown_data_t {
    std::string m_trigger;

    shutdown_data_t(std::string trigger) : m_trigger(std::move(trigger)) {}

    std::string to_xml() const {
        pugi::xml_document doc;

        pugi::xml_node sdown_node = doc.append_child("Shutdown");
        sdown_node.append_attribute("trigger") = m_trigger.c_str();

        std::ostringstream oss;
        doc.save(oss, "", pugi::format_no_declaration);
        return oss.str();
    }
}; // shutdown_data_t

using message_data_t = std::variant<pulse_data_t, shutdown_data_t>;
struct message_t {
    std::string m_topic;
    message_data_t m_data;

    message_t(std::string topic, message_data_t data) 
        : m_topic(std::move(topic)), m_data(std::move(data)) {} 

    std::string serialize() const {
        pugi::xml_document doc;
        
        pugi::xml_node message_node = doc.append_child("Message");
        message_node.append_child("Topic").text().set(m_topic.c_str());

        pugi::xml_node data_node = message_node.append_child("Data");
        std::visit([&](auto&& data) {
                std::string str_data = data.to_xml();
                data_node.append_buffer(str_data.c_str(), str_data.size());
            }, m_data);

        std::ostringstream oss;
        doc.save(oss, "", pugi::format_no_declaration);
        return oss.str();
    }
}; // message_t

std::ostream& operator<<(std::ostream& os, const message_t& message) {
    os << message.m_topic << "\n";
    std::visit([&os](auto&& data) {
            os << data.to_xml();
        }, message.m_data);
    return os;
} // message_t
