#pragma once

#include <string>
#include <memory>
#include "message.pb.h"

class message_t {
 public:
    message_t() = default;

    explicit message_t(const std::string &topic) {
        m_msg.set_topic(topic);
    }

    static message_t from_serialized(const std::string &serialized_data) {
        message_t msg;
        if (!msg.parse(serialized_data)) {
            throw std::runtime_error("Failed to parse protobuf message");
        return msg;
    }

    std::string get_topic() const {
        return m_msg.topic();
    }

    std::string serialize() const {
        std::string output;
        m_msg.SerializeToString(&output);
        return output;
    }

    bool parse(const std::string &data) {
        return m_msg.ParseFromString(data);
    }

    messaging::Envelope& protobuf_message() {
        return m_msg;
    }

    const messaging::Envelope& protobuf_message() const {
        return m_msg;
    }

 private:
    messaging::Envelope m_msg;
};
