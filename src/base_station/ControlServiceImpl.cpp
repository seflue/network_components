#include "ControlServiceImpl.h"
#include "DataConsumer.h"
#include "FileConsumer.h"
#include "Logging.h"
#include <memory>

using namespace base_station;

Status ControlServiceImpl::Scan([[maybe_unused]] ServerContext* context,
                                const ScanRequest* request,
                                ScanReply* reply)
{
    LOG_DEBUG("Received 'Scan' from {}", request->ueid());
    reply->set_quality(_qualityGenerator.generate());
    return Status::OK;
}

Status ControlServiceImpl::Connect([[maybe_unused]] ServerContext* context,
                                   const ConnectionRequest* request,
                                   ConnectionReply* reply)
{
    auto ueid = request->ueid();
    auto filename = request->filename();
    LOG_DEBUG("Received 'Connect' from {}:{}", ueid, filename);
    auto udpPort =
        _connectionPool->reserveConnection(ueid, std::make_unique<FileConsumer>(_serviceId, filename));
    if (!udpPort) {
        reply->set_success(false);
        reply->set_error(control::CONNECTION_NO_SOCKET_FREE);
        LOG_DEBUG("No connections left.");
        return Status::CANCELLED;
    }
    else {
        reply->set_success(true);
        reply->set_error(control::CONNECTION_SUCCESS);
        reply->set_dataport(*udpPort);
        LOG_DEBUG("Successfully connected {} to port {}.", ueid, *udpPort);
        return Status::OK;
    }
}

ControlServiceImpl::ControlServiceImpl(std::unique_ptr<ConnectionPool> pool, std::string serviceId) :
    _connectionPool(std::move(pool)),
    _serviceId(std::move(serviceId))
{
}

Status ControlServiceImpl::Disconnect([[maybe_unused]] ServerContext* context,
                                      const DisconnectionRequest* request,
                                      DisconnectionReply* reply)
{
    LOG_DEBUG("Received 'Disconnect' from {}:{}", request->ueid(), request->dataport());
    auto success = _connectionPool->releaseConnection(request->ueid(), request->dataport());
    if (!success)
        LOG_WARN("Could not release connection for {}:{}", request->ueid(), request->dataport());
    reply->set_success(success);
    reply->set_error(success ? control::DISCONNECTION_SUCCESS
                             : control::DISCONNECTION_NO_CONNECTION_FOUND);
    return Status::OK;
}
