
#include "message_queue.h"
#include <stdexcept>

MessageQueue::MessageQueue(const std::string& host, int port) {
    context_ = redisConnect(host.c_str(), port);
    if (!context_ || context_->err) throw std::runtime_error("Redis connect failed");
}

MessageQueue::~MessageQueue() {
    if (context_) redisFree(context_);
}

void MessageQueue::publish(const std::string& channel, const std::string& message) {
    redisCommand(context_, "PUBLISH %s %s", channel.c_str(), message.c_str());
}
