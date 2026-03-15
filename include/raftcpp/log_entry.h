#ifndef RAFTCPP_INCLUDE_RAFTCPP_LOG_ENTRY_H_
#define RAFTCPP_INCLUDE_RAFTCPP_LOG_ENTRY_H_

#include <string>
#include "raftcpp/types.h"

namespace raftcpp {

struct LogEntry {
    LogIndex index = 0;
    Term term = 0;
    std::string command;
};

}

#endif  // RAFTCPP_INCLUDE_RAFTCPP_LOG_ENTRY_H_