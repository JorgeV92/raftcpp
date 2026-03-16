#include "raftcpp/node.h"

#include <algorithm>
#include <cstddef>
#include <stdexcept>

namespace raftcpp {

namespace {
std::uint64_t MixSeed(std::uint64_t base_seed, NodeId node_id) {
    constexpr std::uint64_t kGoldenRatio = 0x9E3779B97F4A7C15ULL;
    return base_seed ^ (static_cast<std::uint64_t>(node_id) * kGoldenRatio);
}

} // namespace

Node::Node(Config config, std::unique_ptr<Storage> storage, 
            std::unique_ptr<StateMachine> state_machine) 
    : config_(std::move(config)), storage_(std::move(storage)), state_machine_(std::move(state_machine)),
    rng_(MixSeed(config_.random_seed, config_.self_id)) {
    
    if (config_.self_id == kInvalidNodeId) throw std::invalid_argument("Node requires a non-zero self id.");
    if (!storage_) throw std::invalid_argument("Node requires storage.");
    if (!state_machine_) throw std::invalid_argument("Node requires state machine.");
    if (config_.member_ids.empty()) throw std::invalid_argument("Node requires at least one cluster memeber.");
    if (!IsMember(config_.self_id)) throw std::invalid_argument("self_id must be present in member_ids.");
    if (config_.election_timeout_ticks <= config_.heartbeat_interval_ticks) 
        throw std::invalid_argument("election must be greater than heartbeat");
}

bool Node::IsMember(NodeId node_id) const {
    return std::find(config_.member_ids.begin(), config_.member_ids.end(), node_id) != config_.member_ids.end();
}

}