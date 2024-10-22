#pragma once

#include <chrono>
#include <ctime>
#include <iostream>
#include <string>
#include <sstream>

#include "message_utils.hpp"
#include "../xml/pugixml.hpp"

struct message_data_t {
    virtual std::string serialize() const = 0;
}; // message_data_t

struct pulse_data_t : public message_data_t {
    std::chrono::time_point<std::chrono::system_clock> m_timestamp;

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

struct shutdown_data_t : public message_data_t {
    std::string m_trigger;
    std::string m_reason;

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

struct heartbeat_data_t : public message_data_t {
    std::chrono::time_point<std::chrono::system_clock> m_pulse_ts;
    std::chrono::time_point<std::chrono::system_clock> m_hbeat_ts;

    heartbeat_data_t() = default;

    heartbeat_data_t(std::chrono::time_point<std::chrono::system_clock> ts)
     : m_pulse_ts(std::move(ts)), 
       m_hbeat_ts(std::move(std::chrono::system_clock::now()))
    {}

    heartbeat_data_t(const std::string &xml) {
        const auto [doc, data_node] = 
             msg_utils::string_to_xml(xml, "Heartbeat");
         
        std::string pulse_ts_str = data_node.attribute("pulse_ts").value();
        std::tm pulse_tm = {};
        std::istringstream ss(pulse_ts_str);
        ss >> std:: get_time(&pulse_tm, "%Y-%m-%d %H:%M:%S");

        if (ss.fail()) {
            throw std::runtime_error(
                "Failed to parse timestamp: " + pulse_ts_str);
        }

        m_pulse_ts = std::chrono::system_clock::from_time_t(
            std::mktime(&pulse_tm));

        std::string hbeat_ts_str = data_node.attribute("hbeat_ts").value();
        std::tm hbeat_tm = {};
        ss.str(hbeat_ts_str);
        ss >> std:: get_time(&hbeat_tm, "%Y-%m-%d %H:%M:%S");

        if (ss.fail()) {
            throw std::runtime_error(
                "Failed to parse timestamp: " + hbeat_ts_str);
        }

        m_hbeat_ts = std::chrono::system_clock::from_time_t(
            std::mktime(&hbeat_tm));
     }

    std::string serialize() const {
        pugi::xml_document doc;

        pugi::xml_node hbeat_node = doc.append_child("Heartbeat");
        const std::time_t pulse_ts = 
            std::chrono::system_clock::to_time_t(m_pulse_ts);
        const std::tm* pulse_tm = std::localtime(&pulse_ts);

        std::stringstream ss;
        ss << std::put_time(pulse_tm, "%Y-%m-%d %H:%M:%S");
        hbeat_node.append_attribute("pulse_ts") = ss.str().c_str();

        const std::time_t hbeat_ts = 
            std::chrono::system_clock::to_time_t(m_hbeat_ts);
        const std::tm* hbeat_tm = std::localtime(&hbeat_ts);

        ss.str("");
        ss << std::put_time(hbeat_tm, "%Y-%m-%d %H:%M:%S");
        hbeat_node.append_attribute("hbeat_ts") = ss.str().c_str();

        std::ostringstream oss;
        doc.save(oss, "", pugi::format_no_declaration);
        return oss.str();
    }
}; // heartbeat_data_t

struct market_data_t : public message_data_t {
    std::string m_symbol;
    int m_price;
    std::chrono::time_point<std::chrono::system_clock> m_timestamp;

    market_data_t() = default;

    market_data_t(const std::string &sym, 
                  const int price, 
                  std::chrono::time_point<std::chrono::system_clock> ts)
     : m_symbol(sym), m_price(price), m_timestamp(std::move(ts))
    {}

    market_data_t(const std::string &xml) {
        const auto [doc, data_node] = 
            msg_utils::string_to_xml(xml, "Market Data");
        m_symbol = data_node.attribute("symbol").value();
        m_price = std::stoi(data_node.attribute("price").value());

        std::string ts_str = data_node.attribute("timestamp").value();
        std::tm tm = {};
        std::istringstream ss(ts_str);
        ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");

        if (ss.fail()) {
            throw std::runtime_error(
                "Failed to parse timestamp: " + ts_str);
        }

        m_timestamp = std::chrono::system_clock::from_time_t(
            std::mktime(&tm));
    }

    std::string serialize() const {
        pugi::xml_document doc;

        pugi::xml_node mkt_data_node = doc.append_child("Market Data");
        mkt_data_node.append_attribute("symbol") = m_symbol.c_str();
        mkt_data_node.append_attribute("price") = std::to_string(m_price).c_str();

        const std::time_t ts = 
            std::chrono::system_clock::to_time_t(m_timestamp);
        const std::tm* tm = std::localtime(&ts);
        std::stringstream ss;
        ss << std::put_time(tm, "%Y-%m-%d %H:%M:%S");
        mkt_data_node.append_attribute("timestamp") = ss.str().c_str();

        std::ostringstream oss;
        doc.save(oss, "", pugi::format_no_declaration);
        return oss.str();
    }
}; // market_data_t 

struct order_data_t : public message_data_t {
    std::string m_participant;
    std::string m_symbol;
    int m_price;
    uint32_t m_quantity;

    order_data_t() = default;

    order_data_t(const std::string participant, 
                 const std::string sym,
                 int price,
                 uint32_t quantity)
    : m_participant(participant), 
      m_symbol(sym), 
      m_price(price), 
      m_quantity(quantity)
    {}

    order_data_t(const std::string &xml) {
        const auto [doc, data_node] = 
            msg_utils::string_to_xml(xml, "Order");
        m_participant = data_node.attribute("participant").value();
        m_symbol = data_node.attribute("symbol").value();
        m_price = std::stoi(data_node.attribute("price").value());
        unsigned long q = std::stoul(data_node.attribute("quantity").value());
        m_quantity = static_cast<uint32_t>(q);
    }

    std::string serialize() const {
        pugi::xml_document doc;

        pugi::xml_node order_node = doc.append_child("Order");
        order_node.append_attribute("participant") = m_participant.c_str();
        order_node.append_attribute("symbol") = m_symbol.c_str();
        order_node.append_attribute("price") = std::to_string(m_price).c_str();
        order_node.append_attribute("quantity") = 
            std::to_string(m_quantity).c_str();

        std::ostringstream oss;
        doc.save(oss, "", pugi::format_no_declaration);
        return oss.str();
    }
}; // order_data_t
