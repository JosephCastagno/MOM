#pragma once 

#include <chrono>
#include <ctime>
#include <iomanip>
#include <string>
#include <sstream>
#include <variant>

#include "../xml/pugixml.hpp"

namespace msg_utils {
    inline std::pair<pugi::xml_document, pugi::xml_node> string_to_xml(
        const std::string &xml,
        const std::string &topic_key) 
    {
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

    template<typename T>
    T deserialize_msg(const std::string &data) {
        return T::deserialize(data);
    }
} // namespace msg_utils

struct message_t {
    std::string m_topic;
    std::string m_data;

    message_t(std::string topic, std::string data)
        : m_topic(std::move(topic)), m_data(std::move(data)) {}

    std::string serialize() const {
        pugi::xml_document doc;

        // topic
        pugi::xml_node message_node = doc.append_child("Message");
        message_node.append_child("Topic").text().set(m_topic.c_str());

        // data
        pugi::xml_node data_node = doc.append_child("Data");
        data_node.append_buffer(m_data.c_str(), m_data.size());

        std::ostringstream oss;
        doc.save(oss, "", pugi::format_no_declaration);
        return oss.str();
    }
}; // message_t

struct pulse_data_t {
    std::chrono::time_point<std::chrono::system_clock> m_timestamp;

    pulse_data_t() 
        : m_timestamp(std::move(std::chrono::system_clock::now())) {}

    std::string serialize() const {
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

    static pulse_data_t deserialize(const std::string& xml) {
        pulse_data_t result;
        const auto [doc, data_node] = 
            msg_utils::string_to_xml(xml, "Pulse");

        std::string timestamp_str = data_node.attribute("timestamp").value();
        std::tm tm = {};
        std::istringstream ss(timestamp_str);
        ss >> std:: get_time(&tm, "%Y-%m-%d %H:%M:%S");

        if (ss.fail()) {
            throw std::runtime_error(
                "Failed to parse timestamp: " + timestamp_str);
        }

        result.m_timestamp = std::chrono::system_clock::from_time_t(
            std::mktime(&tm));
        return result;
    }
}; // pulse_data_t 

struct shutdown_data_t {
    std::string m_trigger;

    shutdown_data_t(std::string trigger) : m_trigger(std::move(trigger)) {}
    shutdown_data_t() = default;

    std::string serialize() const {
        pugi::xml_document doc;

        pugi::xml_node sdown_node = doc.append_child("Shutdown");
        sdown_node.append_attribute("trigger") = m_trigger.c_str();

        std::ostringstream oss;
        doc.save(oss, "", pugi::format_no_declaration);
        return oss.str();
    }

    static shutdown_data_t deserialize(const std::string &xml) {
        shutdown_data_t result;
        const auto [doc, data_node] = 
            msg_utils::string_to_xml(xml, "Shutdown");
        result.m_trigger = data_node.attribute("trigger").value();
        return result;
    }
}; // shutdown_data_t
