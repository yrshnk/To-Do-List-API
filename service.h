
#pragma once
#include "repository.h"
#include "redis_cache.h"
#include "message_queue.h"
#include <nlohmann/json.hpp>
#include <optional>
#include <vector>

class TaskService {
public:
    TaskService(TaskRepository& repo, RedisCache& cache, MessageQueue& mq);

    Task create(const nlohmann::json& payload);
    std::vector<Task> get_all();
    std::optional<Task> get_by_id(int id);
    std::optional<Task> update_full(int id, const nlohmann::json& payload);
    std::optional<Task> update_partial(int id, const nlohmann::json& payload);
    bool remove(int id);

private:
    TaskRepository& repo_;
    RedisCache& cache_;
    MessageQueue& mq_;
    const int cache_ttl_seconds_ = 60;
    std::string cache_key(int id) const { return "task:" + std::to_string(id); }
};
