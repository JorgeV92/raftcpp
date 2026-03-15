#ifndef RAFTCPP_INCLUDE_RAFTCPP_TYPES_H_
#define RAFTCPP_INCLUDE_RAFTCPP_TYPES_H_

#include <cstdint>

namespace raftcpp {

using NodeId = std::int64_t;
using Term = std::int64_t;
using LogIndex = std::int64_t;

constexpr NodeId kInvalidNodeId = 0;

enum class Role {
    kFollower,
    kCandidate,
    kLeader,
};

}; // namespace raftcpp

#endif  // RAFTCPP_INCLUDE_RAFTCPP_TYPES_H_