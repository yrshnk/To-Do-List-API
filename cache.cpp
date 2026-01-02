
#include "cache.h"
#include <stdexcept>

RedisCache::RedisCache(const std::string& host, int port) {
    context_ = redisConnect(host.c_str(), port);
    if (!context_ || context_->err) throw std::runtime_error("Redis connect failed");
}

RedisCache::~RedisCache() {
    if (context_) redisFree(context_);
}

void RedisCache::set(const std::string& key, const std::string& value, int ttl_seconds) {
    redisCommand(context_, "SET %s %s EX %d", key.c_str(), value.c_str(), ttl_seconds);
}

std::optional<std::string> RedisCache::get(const std::string& key) {
    redisReply* reply = (redisReply*)redisCommand(context_, "GET %s", key.c_str());
    if (!reply) return std::nullopt;
    std::optional<std::string> result;
    if (reply->type == REDIS_REPLY_STRING) result = reply->str;
    freeReplyObject(reply);
    return result;
}

void RedisCache::del(const std::string& key) {
    redisCommand(context_, "DEL %s", key.c_str());
}
