# raftcpp


## Goals

- Keep the **Raft algorithm core deterministic and testable**.
- Separate **protocol logic** from **I/O, RPC, persistence, and threading**.
- Use **small, named abstractions** that match the paper: leader election, log replication, commit advancement, and state machine application.
- Provide a **clear extension path** toward WAL, snapshots, joint consensus, and RPC integration.