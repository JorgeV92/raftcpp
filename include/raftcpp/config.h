#ifndef RAFTCPP_INCLUDE_RAFTCPP_CONFIG_H_
#define RAFTCPP_INCLUDE_RAFTCPP_CONFIG_H_

#include <cstdint>
#include <vector>

#include "raftcpp/types.h"

namespace raftcpp {

struct Config {
    NodeId self_id = kInvalidNodeId;
    std::vector<NodeId> member_ids;
    int election_timeout_ticks = 10;
    int heartbeat_interval_ticks = 3;
    std::uint64_t random_seed = 0xC0FFEE;
};

}

#endif  // RAFTCPP_INCLUDE_RAFTCPP_CONFIG_H_