#ifndef RAFTCPP_INCLUDE_RAFTCPP_STORAGE_H_
#define RAFTCPP_INCLUDE_RAFTCPP_STORAGE_H_

#include <optional>
#include <vector>

#include "raftcpp/log_entry.h"
#include "raftcpp/types.h"

namespace raftcpp {

struct HardState {
    Term current_term = 0;
    std::optional<NodeId> voted_for;
};

class Storage {
public:
    virtual ~Storage() = default;

    virtual HardState GetHardState() const = 0;
    virtual void SetHardState(const HardState& hard_state) = 0;
    virtual LogIndex LastIndex() const = 0;
    virtual Term TermAt(LogIndex index) const = 0;
    virtual std::optional<LogEntry> GetEntry(LogIndex index) const = 0;
    virtual std::vector<LogEntry> GetEntries(LogIndex from_index) const = 0;

    virtual void Append(const std::vector<LogEntry>& entries) = 0;
    virtual void TruncateSuffix(LogIndex last_index_to_keep) = 0;
};

}

#endif  // RAFTCPP_INCLUDE_RAFTCPP_STORAGE_H_`