#pragma once
#include <string>
#include <hiredis/hiredis.h>

class RateLimiter {
public:
    RateLimiter(const std::string& host = "127.0.0.1", int port = 6379, int limit = 10, int window_sec = 60);
    ~RateLimiter();

    bool allow(const std::string& key);

private:
    redisContext* context_;
    int limit_;
    int window_sec_;
};
