
#pragma once
#include "model.h"
#include <vector>
#include <optional>
#include <string>
#include <sqlite3.h>
#include <nlohmann/json.hpp>

class TaskRepository {
public:
    TaskRepository(const std::string& db_file);
    ~TaskRepository();

    Task create(const Task& t);
    std::vector<Task> get_all();
    std::optional<Task> get_by_id(int id);
    std::optional<Task> update_full(int id, const Task& t);
    std::optional<Task> update_partial(int id, const nlohmann::json& payload);
    bool remove(int id);

private:
    sqlite3* db_;
    void init_db();
};
