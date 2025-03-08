#pragma once

#include <chrono>

namespace timestamp {
    inline int64_t get_current_timestamp() {
        auto now = std::chrono::system_clock::now();
        return std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()
        ).count();
    }

    inline std::chrono::system_clock::time_point timestamp_to_timepoint(
        int64_t timestamp_ms)
    {
        return std::chrono::system_clock::time_point(
            std::chrono::milliseconds(timestamp_ms)
        );
    }

    inline std::string format_timestamp(int64_t timestamp_ms) {
        auto tp = timestamp_to_timepoint(timestamp_ms);
        std::time_t time = std::chrono::system_clock::to_time_t(tp);
        return std::string(std::ctime(&time));
    }
} // namespace timestamp 
