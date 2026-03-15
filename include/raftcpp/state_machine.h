#ifndef RAFTCPP_INCLUDE_RAFTCPP_STATE_MACHINE_H_
#define RAFTCPP_INCLUDE_RAFTCPP_STATE_MACHINE_H_

#include <vector>

#include "raftcpp/log_entry.h"

namespace raftcpp {

class StateMachine {
public:
    virtual ~StateMachine() = default;
    virtual void Apply(const LogEntry& entry) = 0;
};

class RecordingStateMachine final : public StateMachine {
public:
    void Apply(const LogEntry& entry) override { commands_.push_back(entry.command); }
    const std::vector<std::string>& applied_commands() const {return commands_;}
private:
    std::vector<std::string> commands_;
};

}

#endif  // RAFTCPP_INCLUDE_RAFTCPP_STATE_MACHINE_H_