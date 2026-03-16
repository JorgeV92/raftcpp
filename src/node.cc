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
    hard_state_ = storage_->GetHardState();
    ResetElectionTimer();
    ResetHeartbeatTimer();
}

void Node::Tick() {
    if (role_ == Role::kLeader) {
        ++heartbeat_elapsed_ticks_;
        if (heartbeat_elapsed_ticks_ >= config_.heartbeat_interval_ticks) {
            SendHeartbeats();
            ResetHeartbeatTimer();
        }
        return;
    }
    ++election_elapsed_ticks_;
    if (election_elapsed_ticks_ >= randomized_election_timeout_ticks_) 
        StartElection();
}

void Node::Step(const Message& message) {
    if (!IsMember(message.from) || !IsMember(message.to) || message.to != id()) return;
    if (message.term > hard_state_.current_term) {
        BecomeFollower(message.term, kInvalidNodeId);
    }
    switch (message.type) {
        case MessageType::kRequestVoteRequest:
            HandleRequestVote(message);
            break;
        case MessageType::kRequestVoteResponse:
            HandleRequestVoteResponse(message);
            break;
        case MessageType::kAppendEntriesRequest:
            HandleAppendEntries(message);
            break;
        case MessageType::kAppendEntriesReponse:
            HandleAppendEntriesResponse(message);
            break;
    }
    ApplyCommitEntries();
}

bool Node::Propose(const std::string& command) {
    if (role_ != Role::kLeader) return false;
    const LogIndex next_log_index = last_log_index() + 1;
    storage_->Append({
        LogEntry{
            .index = next_log_index, 
            .term = hard_state_.current_term,
            .command = command
        }
    });
    match_index_[id()] = next_log_index;
    next_index_[id()] = next_log_index + 1;

    for (NodeId peer_id : config_.member_ids) {
        if (peer_id == id()) continue;
        ReplicateTo(peer_id);
    }
    return true;
}

std::vector<Message> Node::DrainMessages() {
    std::vector<Message> drained;
    drained.swap(outbox_);
    return drained;
}

LogIndex Node::last_log_index() const { return storage_->LastIndex(); }

Term Node::last_log_term() const { return storage_->TermAt(last_log_index()); }

std::optional<LogEntry> Node::GetLogEntry(LogIndex index) const { return storage_->GetEntry(index); }

void Node::ResetElectionTimer() {
    election_elapsed_ticks_ = 0;
    std::uniform_int_distribution<int> dist(config_.election_timeout_ticks, 
                                            config_.election_timeout_ticks * 2);
    randomized_election_timeout_ticks_ = dist(rng_);
}

void Node::ResetHeartbeatTimer() { heartbeat_elapsed_ticks_ = 0; }

void Node::StartElection() {
    role_ = Role::kCandidate;
    leader_id_ = kInvalidNodeId;
    ++hard_state_.current_term;
    storage_->SetHardState(hard_state_);

    votes_received_.clear();
    votes_received_.insert(id());
    ResetElectionTimer();
    SendRequestVotes();
    if (HasMajority(votes_received_.size()))
        BecomeLeader();
}


bool Node::IsMember(NodeId node_id) const {
    return std::find(config_.member_ids.begin(), config_.member_ids.end(), node_id) != config_.member_ids.end();
}

}