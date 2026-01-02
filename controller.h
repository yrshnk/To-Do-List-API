#pragma once
#include "service.h"
#include "rate_limiter.h"
#include <cpp-httplib/httplib.h>

class TaskController {
public:
    TaskController(TaskService& service, RateLimiter& limiter);

    void register_routes(httplib::Server& svr);

private:
    TaskService& service_;
    RateLimiter& limiter_;  
};
