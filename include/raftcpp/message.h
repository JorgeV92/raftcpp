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


}


#endif  // RAFTCPP_INCLUDE_RAFTCPP_MESSAGE_H_