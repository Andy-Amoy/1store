#include "redis/commands/hexists.hh"
#include "redis/commands/unexpected.hh"
#include "seastar/core/shared_ptr.hh"
#include "redis/reply_builder.hh"
#include "redis/request.hh"
#include "redis/redis_mutation.hh"
#include "redis/reply.hh"
#include "db/system_keyspace.hh"
#include "types.hh"
#include "service/storage_proxy.hh"
#include "service/client_state.hh"
#include "mutation.hh"
#include "timeout_config.hh"
#include "redis/prefetcher.hh"
namespace service {
class storage_proxy;
}
namespace redis {

namespace commands {

shared_ptr<abstract_command> hexists::prepare(service::storage_proxy& proxy, request&& req)
{
    if (req._args_count != 2) {
        return unexpected::prepare(std::move(req._command), std::move(bytes {msg_syntax_err}));
    }
    return seastar::make_shared<hexists> (std::move(req._command), maps_schema(proxy), std::move(req._args[0]), std::move(req._args[1]));
}

future<reply> hexists::execute(service::storage_proxy& proxy, db::consistency_level cl, db::timeout_clock::time_point now, const timeout_config& tc, service::client_state& cs)
{
    auto timeout = now + tc.write_timeout;
    return prefetch_map(proxy, _schema, _key, _map_key, cl, timeout, cs).then([this] (auto pd) {
        if (pd && pd->has_data()) {
            return reply_builder::build<ok_tag>();
        }
        return reply_builder::build<error_tag>();
    });
}
}
}
