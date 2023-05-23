#include <memory>
#include <string>
#include <thread>

#include <spdlog/spdlog.h>

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/strings/str_format.h"

#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>

#include <Poco/FileStream.h>
#include <Poco/Net/DatagramSocket.h>
#include <Poco/Net/SocketAddress.h>

#include "ControlServiceImpl.h"
#include "DTOs.h"
#include "Logging.h"
#include "controlplane.grpc.pb.h"

using control::ConnectionReply;
using control::ConnectionRequest;
using control::ControlService;
using control::ScanReply;
using control::ScanRequest;
using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using Poco::FileOutputStream;
using Poco::Net::DatagramSocket;
using Poco::Net::SocketAddress;

using base_station::State;
using namespace utils::logging;

ABSL_FLAG(uint16_t, port, 50051, "Server port for the service");

namespace base_station {
void RunServer(uint16_t grpcPort)
{
    const std::shared_ptr<State> state = std::make_shared<State>(grpcPort);
    std::string server_address = absl::StrFormat("0.0.0.0:%d", grpcPort);
    uint32_t maxConnections = 2;
    uint32_t maxClients = 2;
    auto connectionPool = std::make_unique<ConnectionPool>(maxConnections, maxClients);
    ControlServiceImpl service(state, std::move(connectionPool));

    grpc::EnableDefaultHealthCheckService(true);
    grpc::reflection::InitProtoReflectionServerBuilderPlugin();
    ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);
    std::unique_ptr<Server> server(builder.BuildAndStart());
    spdlog::get("console")->info("Server listening on {}", server_address);
    std::thread grpcServer([&] { server->Wait(); });
    grpcServer.join();
}
} // namespace base_station

using base_station::RunServer;

int main(int argc, char **argv)
{
    absl::ParseCommandLine(argc, argv);
    initializeLogger();
    RunServer(absl::GetFlag(FLAGS_port));
    return 0;
}
