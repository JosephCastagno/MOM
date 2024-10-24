#pragma once

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

    inline std::chrono::system_clock::time_point parse_ts_w_ms(
        const std::string& timestamp_str)
    {
        std::istringstream ss(timestamp_str);
        std::tm dt = {};
        char dot;
        int milliseconds;

        ss >> std::get_time(&dt, "%Y-%m-%d %H:%M:%S");
        if (ss.fail()) {
            throw std::runtime_error(
                "Failed to parse timestamp (date and time): " + timestamp_str);
        }

        ss >> dot >> milliseconds;
        if (ss.fail() || dot != '.') {
            throw std::runtime_error(
                "Failed to parse milliseconds in timestamp: " + timestamp_str);
        }

        dt.tm_isdst = -1;
        std::time_t time = std::mktime(&dt);
        if (time == -1) {
            throw std::runtime_error("Failed to convert time: " + timestamp_str);
        }

        auto time_point = std::chrono::system_clock::from_time_t(time);
        time_point += std::chrono::milliseconds(milliseconds);

        return time_point;
    }

    inline std::string get_ts_w_ms() {
        auto now = std::chrono::system_clock::now();

        auto now_time_t = std::chrono::system_clock::to_time_t(now);
        auto duration_since_epoch = now.time_since_epoch();
        auto milliseconds = 
            std::chrono::duration_cast<std::chrono::milliseconds>(
                duration_since_epoch) % 1000;

        std::ostringstream oss;
        oss << std::put_time(std::localtime(&now_time_t), "%Y-%m-%d %H:%M:%S");
        oss << '.' << std::setfill('0') << std::setw(3) << milliseconds.count();

        return oss.str();
    }

    inline std::string format_ts(
        const std::chrono::system_clock::time_point& time_point) {
        std::time_t time_t_point = std::chrono::system_clock::to_time_t(time_point);

        auto duration_since_epoch = time_point.time_since_epoch();
        auto milliseconds = 
            std::chrono::duration_cast<std::chrono::milliseconds>(duration_since_epoch) % 1000;

        std::tm tm_time;
            localtime_r(&time_t_point, &tm_time);

        std::ostringstream oss;
        oss << std::put_time(&tm_time, "%Y-%m-%d %H:%M:%S");

        oss << '.' << std::setfill('0') << std::setw(3) << milliseconds.count();
        return oss.str();
    }
} // namespace msg_utils

