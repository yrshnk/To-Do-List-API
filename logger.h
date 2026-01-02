#pragma once
#include <spdlog/spdlog.h>
#include <memory>

class Logger {
public:
    static std::shared_ptr<spdlog::logger> get() {
        static auto logger = spdlog::stdout_color_mt("todo_logger");
        return logger;
    }
};
