#ifndef RAFTCPP_INCLUDE_RAFTCPP_MESSAGE_H_
#define RAFTCPP_INCLUDE_RAFTCPP_MESSAGE_H_

#include <vector>

#include "raftcpp/log_entry.h"
#include "raftcpp/types.h"

namespace raftcpp {

enum class MessageType {
    kRequestVoteRequest,
    kRequestVoteResponse,
    kAppendEntriesRequest,
    kAppendEntriesReponse,
};

struct Message {
    MessageType type = MessageType::kAppendEntriesRequest;
    NodeId from = kInvalidNodeId;
    NodeId to = kInvalidNodeId;
    Term term = 0;

    LogIndex last_log_index = 0;
    Term last_log_term = 0;

    bool vote_granted = false;

    NodeId leader_id = kInvalidNodeId;
    LogIndex prev_log_index = 0;
    Term prev_log_term =0;
    std::vector<LogEntry> entries;
    LogIndex leader_commit = 0;

    bool success = false;
    LogIndex match_index = 0;
    LogIndex reject_hint = 0;
};

}


#endif  // RAFTCPP_INCLUDE_RAFTCPP_MESSAGE_H_