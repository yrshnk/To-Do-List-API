#include "repository.h"
#include "cache.h"
#include "service.h"
#include "controller.h"
#include "message_queue.h"
#include "rate_limiter.h"
#include <cpp-httplib/httplib.h>
#include <iostream>

int main() {
    TaskRepository repo("tasks.db");
    RedisCache cache;
    MessageQueue mq;
    TaskService service(repo, cache, mq);
    TaskController controller(service);

    httplib::Server svr;
    controller.register_routes(svr);

    RateLimiter limiter(10, 1);

    svr.Get("/metrics", [&service](const httplib::Request&, httplib::Response& res){
        nlohmann::json j;
        j["create"] = Metrics::instance().get("create");
        j["get_all"] = Metrics::instance().get("get_all");
        j["get_by_id"] = Metrics::instance().get("get_by_id");
        j["update_full"] = Metrics::instance().get("update_full");
        j["update_partial"] = Metrics::instance().get("update_partial");
        j["delete"] = Metrics::instance().get("delete");
        j["cache_hit"] = Metrics::instance().get("cache_hit");
        res.set_content(j.dump(), "application/json");
        res.status = 200;
    });

    std::cout << "Server running on http://0.0.0.0:8080\n";
    svr.listen("0.0.0.0", 8080);
    return 0;
}
