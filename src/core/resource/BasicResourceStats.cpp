#include "core/resource/BasicResourceStats.h"
#include <iostream>

namespace yao {

BasicResourceStats::BasicResourceStats() 
    : cpuUsage_(0.0), memoryUsage_(0), diskUsage_(0) {
}

BasicResourceStats::~BasicResourceStats() = default;

double BasicResourceStats::getCpuUsage() const {
    return cpuUsage_.load();
}

size_t BasicResourceStats::getMemoryUsage() const {
    return memoryUsage_.load();
}

size_t BasicResourceStats::getDiskUsage() const {
    return diskUsage_.load();
}

void BasicResourceStats::reset() {
    cpuUsage_.store(0.0);
    memoryUsage_.store(0);
    diskUsage_.store(0);
}

void BasicResourceStats::updateCpuUsage(double usage) {
    double current = cpuUsage_.load();
    while (!cpuUsage_.compare_exchange_weak(current, current + usage));
}

void BasicResourceStats::updateMemoryUsage(size_t usage) {
    memoryUsage_.fetch_add(usage);
}

void BasicResourceStats::updateDiskUsage(size_t usage) {
    diskUsage_.fetch_add(usage);
}

} // namespace yao