
#pragma once
#include <string>
#include <optional>
#include <hiredis/hiredis.h>

class RedisCache {
public:
    RedisCache(const std::string& host="127.0.0.1", int port=6379);
    ~RedisCache();

    void set(const std::string& key, const std::string& value, int ttl_seconds);
    std::optional<std::string> get(const std::string& key);
    void del(const std::string& key);

private:
    redisContext* context_;
};
