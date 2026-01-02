
#include "service.h"
#include "logger.h"
#include <spdlog/spdlog.h>

TaskService::TaskService(TaskRepository& repo, RedisCache& cache, MessageQueue& mq)
: repo_(repo), cache_(cache), mq_(mq) {}

Task TaskService::create(const nlohmann::json& payload) {
    Task t;
    t.title = payload.value("title", "");
    t.description = payload.value("description", "");
    if (payload.contains("status")) t.status = status_from_string(payload["status"].get<std::string>());
    Task created = repo_.create(t);

    nlohmann::json j = created;
    cache_.set(cache_key(created.id), j.dump(), cache_ttl_seconds_);
    mq_.publish("task_created", j.dump());
    spdlog::info("Created task id={}", created.id);
    return created;
}

std::vector<Task> TaskService::get_all() { return repo_.get_all(); }

std::optional<Task> TaskService::get_by_id(int id) {
    auto c = cache_.get(cache_key(id));
    if (c) {
        try { return nlohmann::json::parse(*c).get<Task>(); } 
        catch (...) { spdlog::warn("Cache parse error for id={}", id); }
    }
    auto t = repo_.get_by_id(id);
    if (t) cache_.set(cache_key(id), nlohmann::json(*t).dump(), cache_ttl_seconds_);
    return t;
}

std::optional<Task> TaskService::update_full(int id, const nlohmann::json& payload) {
    Task t;
    t.title = payload.value("title", "");
    t.description = payload.value("description", "");
    if (payload.contains("status")) t.status = status_from_string(payload["status"].get<std::string>());
    auto updated = repo_.update_full(id, t);
    if (updated) {
        nlohmann::json j = *updated;
        cache_.set(cache_key(id), j.dump(), cache_ttl_seconds_);
        mq_.publish("task_updated", j.dump());
        spdlog::info("Updated (full) task id={}", id);
    }
    return updated;
}

std::optional<Task> TaskService::update_partial(int id, const nlohmann::json& payload) {
    auto updated = repo_.update_partial(id, payload);
    if (updated) {
        nlohmann::json j = *updated;
        cache_.set(cache_key(id), j.dump(), cache_ttl_seconds_);
        mq_.publish("task_updated", j.dump());
        spdlog::info("Updated (partial) task id={}", id);
    }
    return updated;
}

bool TaskService::remove(int id) {
    cache_.del(cache_key(id));
    bool ok = repo_.remove(id);
    if (ok) {
        nlohmann::json j = {{"id", id}};
        mq_.publish("task_deleted", j.dump());
        spdlog::info("Deleted task id={}", id);
    }
    return ok;
}
