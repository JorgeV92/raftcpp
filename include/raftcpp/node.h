#ifndef RAFTCPP_INCLUDE_RAFTCPP_NODE_H_
#define RAFTCPP_INCLUDE_RAFTCPP_NODE_H_

#include <map>
#include <memory>
#include <optional>
#include <random>
#include <set>
#include <string>
#include <vector>

#include "raftcpp/config.h"
#include "raftcpp/message.h"
#include "raftcpp/state_machine.h"
#include "raftcpp/storage.h"
#include "raftcpp/types.h"

namespace raftcpp {

class Node {
public:
    Node(Config config, std::unique_ptr<Storage> storage, 
        std::unique_ptr<StateMachine> state_machine);
    void Tick();
    void Step(const Message& messgae);
    bool Propose(const std::string& command);
    std::vector<Message> DrainMessages();
    Role role() const {return role_;}
    NodeId id() const {return config_.self_id;}
    Term current_term() const { return leader_id_; }
    LogIndex commit_index() const { return commit_index_; }
    LogIndex last_applied() const { return last_applied_; }
    LogIndex last_log_index() const;
    Term last_log_term() const;
    std::optional<LogEntry> GetLogEntry(LogIndex index) const;
    const StateMachine& state_machine() const { return *state_machine_; } 
private:
    void ResetElectionTimer();
    void ResetHeartbeatTimer();
    void StartElection();
    void BecomeFollower(Term new_term, NodeId leader_id);
    void BecomeLeader();

    void HandleRequestVote(const Message& message);
    void HandleRequestVoteResponse(const Message& message);
    void HandleAppendEntries(const Message& message);
    void HandleAppendEntriesResponse(const Message& message);

    void SendRequestVotes();
    void SendHeartbeats();
    void ReplicateTo(NodeId peer_id);
    Message BuildAppendEntries(NodeId peer_id) const;
    void AdvanceCommitIndex();
    void ApplyCommitEntries();

    bool IsCandidateLogUpDate(LogIndex candidate_last_log_index, Term candidate_last_log_term) const;
    bool HasMajority(std::size_t count) const;
    bool IsMember(NodeId node_id) const;
    void QueueMessage(const Message& message);

    Config config_;
    std::unique_ptr<Storage> storage_;
    std::unique_ptr<StateMachine> state_machine_;
    Role role_ = Role::kFollower;
    HardState hard_state_;
    NodeId leader_id_ = kInvalidNodeId;
    LogIndex commit_index_ = 0;
    LogIndex last_applied_ = 0;
    int election_elapsed_ticks_ = 0;
    int heartbeat_elapsed_ticks_ = 0;
    int randomized_election_timeout_ticks_ = 0;
    std::mt19937_64 rng_;
    std::set<NodeId> votes_received_;
    std::map<NodeId, LogIndex> next_index_;
    std::map<NodeId, LogIndex> match_index_;
    std::vector<Message> outbox_;
};

}

#endif  // RAFTCPP_INCLUDE_RAFTCPP_NODE_H_
