
#pragma once
#include <string>
#include <hiredis/hiredis.h>

class MessageQueue {
public:
    MessageQueue(const std::string& host="127.0.0.1", int port=6379);
    ~MessageQueue();

    void publish(const std::string& channel, const std::string& message);

private:
    redisContext* context_;
};
