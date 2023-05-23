#include "ControlServiceImpl.h"
#include "Logging.h"
#include <Poco/Net/SocketNotification.h>

using namespace base_station;

Status ControlServiceImpl::Scan(ServerContext *context, const ScanRequest *request,
                                ScanReply *reply)
{
    LOG_DEBUG("Received 'Scan' from {}", request->ueid());
    reply->set_quality(qualityGenerator_.generate());
    return Status::OK;
}

Status ControlServiceImpl::Connect(ServerContext *context, const ConnectionRequest *request,
                                   ConnectionReply *reply)
{
    std::string ueid = request->ueid();
    LOG_DEBUG("Received 'Connect' from {}:{}", request->ueid(), request->filename());

    auto udpPort = connectionPool_->reserveConnection(ueid, request->filename());
    if (!udpPort) {
        reply->set_success(false);
        reply->set_error(control::CONNECTION_NO_SOCKET_FREE);
        LOG_DEBUG("No connections left.");
    }
    else {
        reply->set_success(true);
        reply->set_error(control::CONNECTION_SUCCESS);
        reply->set_dataport(*udpPort);
        LOG_DEBUG("Successfully connected {} to port {}.", request->ueid(), *udpPort);
    }

    return Status::OK;
}

ControlServiceImpl::ControlServiceImpl(const std::shared_ptr<State> &state,
                                       std::unique_ptr<ConnectionPool> pool)
    : state_(state), connectionPool_(std::move(pool))
{
}

Status ControlServiceImpl::Disconnect(ServerContext *context, const DisconnectionRequest *request,
                                      DisconnectionReply *reply)
{
    LOG_DEBUG("Received 'Disconnect' from {}:{}", request->ueid(), request->dataport());
    auto success = connectionPool_->releaseConnection(request->ueid(), request->dataport());
    if(!success)
        LOG_WARN("Could not release connection for {}:{}", request->ueid(), request->dataport());
    reply->set_success(success);
    reply->set_error(success ? control::DISCONNECTION_SUCCESS
                             : control::DISCONNECTION_NO_CONNECTION_FOUND);
    return Status::OK;
}
