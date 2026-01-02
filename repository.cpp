
#include "repository.h"
#include <stdexcept>

TaskRepository::TaskRepository(const std::string& db_file) {
    if (sqlite3_open(db_file.c_str(), &db_) != SQLITE_OK) {
        throw std::runtime_error("Cannot open DB");
    }
    init_db();
}

TaskRepository::~TaskRepository() {
    if (db_) sqlite3_close(db_);
}

void TaskRepository::init_db() {
    const char* sql = "CREATE TABLE IF NOT EXISTS tasks("
                      "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                      "title TEXT,"
                      "description TEXT,"
                      "status TEXT);";
    char* err = nullptr;
    if (sqlite3_exec(db_, sql, nullptr, nullptr, &err) != SQLITE_OK) {
        std::string e = err; sqlite3_free(err);
        throw std::runtime_error(e);
    }
}

Task TaskRepository::create(const Task& t) {
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db_, "INSERT INTO tasks(title, description, status) VALUES (?, ?, ?);", -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, t.title.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, t.description.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, status_to_string(t.status).c_str(), -1, SQLITE_TRANSIENT);
    if (sqlite3_step(stmt) != SQLITE_DONE) throw std::runtime_error("Insert failed");
    sqlite3_finalize(stmt);

    Task created = t;
    created.id = static_cast<int>(sqlite3_last_insert_rowid(db_));
    return created;
}

std::vector<Task> TaskRepository::get_all() {
    std::vector<Task> result;
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db_, "SELECT id, title, description, status FROM tasks;", -1, &stmt, nullptr);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Task t;
        t.id = sqlite3_column_int(stmt, 0);
        t.title = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        t.description = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        t.status = status_from_string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3)));
        result.push_back(t);
    }
    sqlite3_finalize(stmt);
    return result;
}

std::optional<Task> TaskRepository::get_by_id(int id) {
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db_, "SELECT id, title, description, status FROM tasks WHERE id=?;", -1, &stmt, nullptr);
    sqlite3_bind_int(stmt, 1, id);
    Task t;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        t.id = sqlite3_column_int(stmt, 0);
        t.title = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        t.description = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        t.status = status_from_string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3)));
        sqlite3_finalize(stmt);
        return t;
    }
    sqlite3_finalize(stmt);
    return std::nullopt;
}

std::optional<Task> TaskRepository::update_full(int id, const Task& t) {
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db_, "UPDATE tasks SET title=?, description=?, status=? WHERE id=?;", -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, t.title.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, t.description.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, status_to_string(t.status).c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 4, id);
    if (sqlite3_step(stmt) != SQLITE_DONE) { sqlite3_finalize(stmt); return std::nullopt; }
    sqlite3_finalize(stmt);
    return get_by_id(id);
}

std::optional<Task> TaskRepository::update_partial(int id, const nlohmann::json& payload) {
    auto current = get_by_id(id);
    if (!current) return std::nullopt;
    Task t = current.value();
    if (payload.contains("title")) t.title = payload["title"].get<std::string>();
    if (payload.contains("description")) t.description = payload["description"].get<std::string>();
    if (payload.contains("status")) t.status = status_from_string(payload["status"].get<std::string>());
    return update_full(id, t);
}

bool TaskRepository::remove(int id) {
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db_, "DELETE FROM tasks WHERE id=?;", -1, &stmt, nullptr);
    sqlite3_bind_int(stmt, 1, id);
    bool ok = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);
    return ok;
}
