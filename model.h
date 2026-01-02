#pragma once
#include <string>
#include <nlohmann/json.hpp>

enum class Status {
    TODO,
    IN_PROGRESS,
    DONE
};

inline Status status_from_string(const std::string& s) {
    if (s == "todo") return Status::TODO;
    if (s == "in_progress") return Status::IN_PROGRESS;
    if (s == "done") return Status::DONE;
    throw std::invalid_argument("Invalid status: " + s);
}

inline std::string status_to_string(Status s) {
    switch (s) {
        case Status::TODO: return "todo";
        case Status::IN_PROGRESS: return "in_progress";
        case Status::DONE: return "done";
    }
    return "todo"; // default
}

struct Task {
    int id = 0;
    std::string title;
    std::string description;
    Status status = Status::TODO;
};

inline void to_json(nlohmann::json& j, const Task& t) {
    j = nlohmann::json{
        {"id", t.id},
        {"title", t.title},
        {"description", t.description},
        {"status", status_to_string(t.status)}
    };
}

inline void from_json(const nlohmann::json& j, Task& t) {
    t.id = j.value("id", 0);
    t.title = j.value("title", std::string());
    t.description = j.value("description", std::string());
    if (j.contains("status")) t.status = status_from_string(j["status"].get<std::string>());
}
