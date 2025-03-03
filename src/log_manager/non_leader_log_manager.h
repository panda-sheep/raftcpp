#pragma once

#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <queue>

#include "common/constants.h"
#include "common/id.h"
#include "common/timer.h"
#include "common/timer_manager.h"
#include "log_manager/blocking_queue_interface.h"
#include "log_manager/blocking_queue_mutex_impl.h"
#include "proto/raft.grpc.pb.h"
#include "proto/raft.pb.h"
#include "statemachine/state_machine.h"

namespace raftcpp {

class NonLeaderLogManager final {
public:
    NonLeaderLogManager(
        const NodeID &this_node_id, std::shared_ptr<StateMachine> fsm,
        std::function<bool()> is_leader_func,
        std::function<std::shared_ptr<raftrpc::Stub>()> get_leader_rpc_client_func,
        const std::shared_ptr<common::TimerManager> &timer_manager);

    ~NonLeaderLogManager(){};

    void Run(std::unordered_map<int64_t, LogEntry> &logs, int64_t committedIndex);
    void Stop();
    bool IsRunning() const;

    void Push(int64_t committed_log_index, int32_t pre_log_term, LogEntry log_entry);

    int64_t CurrLogIndex() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return next_index_ - 1;
    }

    // attention to all_log_entries_ may be large, so as far as possible no copy
    std::unordered_map<int64_t, LogEntry> &Logs() {
        std::lock_guard<std::mutex> lock(mutex_);
        return all_log_entries_;
    }

    int64_t CommittedLogIndex() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return committed_log_index_;
    }

private:
    void CommitLogs(int64_t committed_log_index);

private:
    mutable std::mutex mutex_;

    /// The index which the leader committed.
    int64_t committed_log_index_ = -1;

    /// Next index to be read from leader.
    int64_t next_index_ = 0;

    /// Handle push log result
    bool push_log_result_ = true;

    std::unordered_map<int64_t, LogEntry> all_log_entries_;

    asio::io_service io_service_;

    std::unique_ptr<std::thread> committing_thread_;

    /// The function to get leader rpc client.
    std::function<std::shared_ptr<raftrpc::Stub>()> get_leader_rpc_client_func_;

    std::function<bool()> is_leader_func_ = nullptr;

    std::atomic_bool is_running_ = false;

    NodeID this_node_id_;

    std::shared_ptr<StateMachine> fsm_;

    std::shared_ptr<common::TimerManager> timer_manager_;

    /// The timer used to send pull log entries requests to the leader.
    /// Note that this is only used in non-leader node.
};

}  // namespace raftcpp
