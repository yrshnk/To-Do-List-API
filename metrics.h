#pragma once
#include <atomic>
#include <string>
#include <unordered_map>
#include <mutex>

class Metrics {
public:
    static Metrics& instance() {
        static Metrics inst;
        return inst;
    }

    void increment(const std::string& key) {
        std::lock_guard<std::mutex> lock(mutex_);
        counters_[key]++;
    }

    int get(const std::string& key) {
        std::lock_guard<std::mutex> lock(mutex_);
        return counters_[key];
    }

private:
    Metrics() = default;
    std::unordered_map<std::string,int> counters_;
    std::mutex mutex_;
};
