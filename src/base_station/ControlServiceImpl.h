#ifndef NETWORK_COMPONENTS_CONTROLSERVICEIMPL_H
#define NETWORK_COMPONENTS_CONTROLSERVICEIMPL_H

#include <iostream>
#include <memory>
#include <string>
#include <thread>

#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>

#include "DTOs.h"
#include "controlplane.grpc.pb.h"

#include "Connection.h"
#include "ConnectionPool.h"
#include "QualityGenerator.h"

using control::ConnectionReply;
using control::ConnectionRequest;
using control::ControlService;
using control::DisconnectionReply;
using control::DisconnectionRequest;
using control::ScanReply;
using control::ScanRequest;
using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;

using base_station::Connection;
using base_station::ConnectionPool;
using base_station::QualityGenerator;
using base_station::State;

class ControlServiceImpl final : public ControlService::Service {

    Status Scan(ServerContext *context, const ScanRequest *request, ScanReply *reply) override;

    Status Connect(ServerContext *context, const ConnectionRequest *request,
                   ConnectionReply *reply) override;

    Status Disconnect(ServerContext *context, const DisconnectionRequest *request,
                      DisconnectionReply *reply) override;

    std::shared_ptr<State> state_;
    std::unique_ptr<ConnectionPool> connectionPool_;
    QualityGenerator qualityGenerator_;

  public:
    explicit ControlServiceImpl(const std::shared_ptr<State> &state,
                                std::unique_ptr<ConnectionPool> pool);
};

#endif // NETWORK_COMPONENTS_CONTROLSERVICEIMPL_H
