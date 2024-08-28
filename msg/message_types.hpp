#pragma once

#include <chrono>
#include <ctime>
#include <iostream>
#include <string>
#include <sstream>

#include "message_utils.hpp"
#include "../xml/pugixml.hpp"

class message_data_t {
 public:
    virtual std::string serialize() const = 0;
}; // message_data_t

class pulse_data_t : public message_data_t {
 private:
     std::chrono::time_point<std::chrono::system_clock> m_timestamp;

 public: 
     pulse_data_t()
         : m_timestamp(std::move(std::chrono::system_clock::now())) {}

     pulse_data_t(const std::string &xml) {
        const auto [doc, data_node] = msg_utils::string_to_xml(xml, "Pulse");

        std::string timestamp_str = data_node.attribute("timestamp").value();
        std::tm tm = {};
        std::istringstream ss(timestamp_str);
        ss >> std:: get_time(&tm, "%Y-%m-%d %H:%M:%S");

        if (ss.fail()) {
            throw std::runtime_error(
                "Failed to parse timestamp: " + timestamp_str);
        }

        m_timestamp = std::chrono::system_clock::from_time_t(
            std::mktime(&tm));
     }

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
}; // pulse_data_t

class shutdown_data_t : public message_data_t {
 private:
     std::string m_trigger;
     std::string m_reason;

 public:
     shutdown_data_t() = default;

     shutdown_data_t(std::string trigger, std::string reason) 
         : m_trigger(trigger), m_reason(m_reason) {}

     shutdown_data_t(const std::string &xml) {
        const auto [doc, data_node] = 
            msg_utils::string_to_xml(xml, "Shutdown");
        m_trigger = data_node.attribute("trigger").value();
        m_reason = data_node.attribute("reason").value();
     }

     std::string serialize() const {
        pugi::xml_document doc;

        pugi::xml_node sdown_node = doc.append_child("Shutdown");
        sdown_node.append_attribute("trigger") = m_trigger.c_str();
        sdown_node.append_attribute("reason") = m_reason.c_str();

        std::ostringstream oss;
        doc.save(oss, "", pugi::format_no_declaration);
        return oss.str();
     }
}; // shutdown_data_t



