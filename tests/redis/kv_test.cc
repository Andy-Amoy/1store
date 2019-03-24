#include <boost/test/unit_test.hpp>
#include <seastar/core/future.hh>

#include "seastarx.hh"
#include "tests/test-utils.hh"
#include "utils/hash.hh"

#include "tests/cql_test_env.hh"
#include "tests/cql_assertions.hh"
#include "redis/redis_keyspace.hh"

SEASTAR_TEST_CASE(test_redis_set){
    return do_with_redis_env_thread([] (auto& e) {
        auto&& reply = e.execute_redis("set a b").get0();
        assert_that(std::move(reply)).is_redis_reply()
            .with_status(bytes("OK"));

        auto msg = e.execute_cql(sprint("select * from redis.%s ", redis::SIMPLE_OBJECTS)).get0();
        assert_that(msg).is_rows()
            .with_size(1)
            .with_row({
                {bytes_type->decompose(data_value(bytes("a")))},
                {bytes_type->decompose(data_value(bytes("b")))},
            });
        return make_ready_future<>();
    });
}

