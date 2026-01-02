#include "rate_limiter.h"
#include <stdexcept>
#include <sstream>

RateLimiter::RateLimiter(const std::string& host, int port, int limit, int window_sec)
    : limit_(limit), window_sec_(window_sec)
{
    context_ = redisConnect(host.c_str(), port);
    if (!context_ || context_->err) throw std::runtime_error("Redis connect failed");
}

RateLimiter::~RateLimiter() {
    if (context_) redisFree(context_);
}

bool RateLimiter::allow(const std::string& key) {
    std::ostringstream oss;
    oss << "rate:" << key;

    redisReply* reply = (redisReply*)redisCommand(context_,
        "INCR %s", oss.str().c_str());
    if (!reply) return false;

    long count = reply->integer;
    freeReplyObject(reply);

    if (count == 1) {
        redisCommand(context_, "EXPIRE %s %d", oss.str().c_str(), window_sec_);
    }

    return count <= limit_;
}
