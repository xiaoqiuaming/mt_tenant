#include "core/resource/TenantThreadGroup.h"
#include "core/resource/CgroupController.h"
#include <algorithm>
#include <iostream>
#include <chrono>

namespace yao {

// WorkerThread implementation
WorkerThread::WorkerThread(const std::string& tenantId, LockFreeQueue& queue, CgroupController* cgroup)
    : tenantId_(tenantId), taskQueue_(queue), cgroup_(cgroup), running_(false), busy_(false), executedTasks_(0) {
}

WorkerThread::~WorkerThread() {
    stop();
}

void WorkerThread::start() {
    if (running_) return;

    running_ = true;
    thread_ = std::make_unique<std::thread>(&WorkerThread::run, this);

    // Add thread to cgroup if available
    if (cgroup_) {
        cgroup_->addThread(tenantId_, getId());
    }
}

void WorkerThread::stop() {
    if (!running_) return;

    running_ = false;
    if (thread_ && thread_->joinable()) {
        thread_->join();
    }

    // Remove thread from cgroup
    if (cgroup_) {
        cgroup_->removeThread(tenantId_, getId());
    }
}

std::thread::id WorkerThread::getId() const {
    return thread_ ? thread_->get_id() : std::thread::id();
}

bool WorkerThread::isBusy() const {
    return busy_.load();
}

size_t WorkerThread::getExecutedTasks() const {
    return executedTasks_.load();
}

void WorkerThread::run() {
    while (running_) {
        auto task = taskQueue_.dequeue();
        if (task && task->isValid()) {
            busy_ = true;
            try {
                task->execute();
                executedTasks_.fetch_add(1);
            } catch (const std::exception& e) {
                std::cerr << "Task execution failed for tenant " << tenantId_ << ": " << e.what() << std::endl;
            }
            busy_ = false;
        } else {
            // Sleep briefly to avoid busy waiting
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
}

// TenantThreadGroup implementation
TenantThreadGroup::TenantThreadGroup(const std::string& tenantId, size_t threadCount, CgroupController* cgroup)
    : tenantId_(tenantId), cgroup_(cgroup), running_(false) {
    resize(threadCount);
}

TenantThreadGroup::~TenantThreadGroup() {
    stop();
}

bool TenantThreadGroup::start() {
    if (running_) return true;

    running_ = true;
    for (auto& worker : threads_) {
        worker->start();
    }
    return true;
}

void TenantThreadGroup::stop() {
    if (!running_) return;

    running_ = false;
    for (auto& worker : threads_) {
        worker->stop();
    }
}

bool TenantThreadGroup::submitTask(std::unique_ptr<Task> task) {
    return taskQueue_.enqueue(std::move(task));
}

size_t TenantThreadGroup::getQueueSize() const {
    return taskQueue_.size();
}

size_t TenantThreadGroup::getBusyThreads() const {
    return std::count_if(threads_.begin(), threads_.end(),
                        [](const std::unique_ptr<WorkerThread>& worker) {
                            return worker->isBusy();
                        });
}

size_t TenantThreadGroup::getTotalThreads() const {
    return threads_.size();
}

bool TenantThreadGroup::resize(size_t newThreadCount) {
    if (newThreadCount == threads_.size()) return true;

    if (newThreadCount > threads_.size()) {
        // Add threads
        size_t toAdd = newThreadCount - threads_.size();
        for (size_t i = 0; i < toAdd; ++i) {
            threads_.emplace_back(std::make_unique<WorkerThread>(tenantId_, taskQueue_, cgroup_));
        }
        if (running_) {
            for (size_t i = threads_.size() - toAdd; i < threads_.size(); ++i) {
                threads_[i]->start();
            }
        }
    } else {
        // Remove threads
        size_t toRemove = threads_.size() - newThreadCount;
        for (size_t i = 0; i < toRemove; ++i) {
            threads_.back()->stop();
            threads_.pop_back();
        }
    }

    return true;
}

} // namespace yao