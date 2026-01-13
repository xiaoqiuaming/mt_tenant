#include "server/trans/TransServer.h"
#include <iostream>

namespace yao {

bool YaoTransServer::handleRequest(const RequestContext& context) {
    // 事务服务器为共享资源，不进行租户隔离
    std::cout << "Handling transaction request" << std::endl;
    return true;
}

bool YaoTransServer::initialize() {
    std::cout << "YaoTransServer initialized" << std::endl;
    return true;
}

bool YaoTransServer::start() {
    std::cout << "YaoTransServer started" << std::endl;
    return true;
}

void YaoTransServer::stop() {
    std::cout << "YaoTransServer stopped" << std::endl;
}

} // namespace yao