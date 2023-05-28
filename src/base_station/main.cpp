#include <memory>
#include <string>
#include <thread>

#include <spdlog/spdlog.h>

#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>

#include <Poco/FileStream.h>
#include <Poco/Net/DatagramSocket.h>
#include <Poco/Net/SocketAddress.h>

#include <Poco/Util/HelpFormatter.h>
#include <Poco/Util/Option.h>
#include <Poco/Util/OptionSet.h>
#include <Poco/Util/ServerApplication.h>

#include "ControlServiceImpl.h"
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

using namespace utils::logging;

namespace base_station {

class BaseStation : public Poco::Util::ServerApplication {
  public:
    BaseStation() {}

  protected:
    void initialize(Application& self) override
    {
        loadConfiguration(); // Load configuration files, if any
        Poco::Util::ServerApplication::initialize(self);
    }

    void uninitialize() override { Poco::Util::ServerApplication::uninitialize(); }

    void defineOptions(Poco::Util::OptionSet& options) override
    {
        Poco::Util::ServerApplication::defineOptions(options);

        options.addOption(
            Poco::Util::Option("help", "h")
                .required(false)
                .repeatable(false)
                .description("Display help information")
                .callback(Poco::Util::OptionCallback<BaseStation>(this, &BaseStation::handleHelp)));

        options.addOption(
            Poco::Util::Option("ip-address", "i")
                .required(false)
                .repeatable(false)
                .argument("ip")
                .binding("ip")
                .description("IP address")
                .callback(
                    Poco::Util::OptionCallback<BaseStation>(this, &BaseStation::handleIpAddress)));

        options.addOption(
            Poco::Util::Option("grpc-port", "p")
                .required(false)
                .repeatable(false)
                .argument("grpcPort")
                .binding("grpcPort")
                .description("IP address")
                .callback(
                    Poco::Util::OptionCallback<BaseStation>(this, &BaseStation::handleGrpcPort)));
    }

    void handleHelp([[maybe_unused]] const std::string& name,
                    [[maybe_unused]] const std::string& value)
    {
        displayHelp();
        stopOptionsProcessing();
        exit(Poco::Util::Application::EXIT_OK);
    }

    void handleIpAddress([[maybe_unused]] const std::string& name, const std::string& value)
    {
        _ip = value;
    }
    void handleGrpcPort([[maybe_unused]] const std::string& name, const std::string& value)
    {
        _grpcPort = std::stoi(value);
    }

    std::string serverAddress() { return std::format("{}:{}", _ip, _grpcPort); }

    void displayHelp()
    {
        Poco::Util::HelpFormatter helpFormatter(options());
        helpFormatter.setCommand(commandName());
        helpFormatter.setUsage("OPTIONS");
        helpFormatter.setHeader("A server application for the base station.");
        helpFormatter.format(std::cout);
    }

    int main([[maybe_unused]] const std::vector<std::string>& args) override
    {
        LOG_INFO("IP address: {}", _ip);
        LOG_INFO("GrpcPort: {}", _grpcPort);

        uint32_t maxConnections = 2;
        uint32_t maxClients = 2;
        auto connectionPool = std::make_unique<ConnectionPool>(maxConnections, maxClients);
        ControlServiceImpl service(std::move(connectionPool));

        grpc::EnableDefaultHealthCheckService(true);
        grpc::reflection::InitProtoReflectionServerBuilderPlugin();
        ServerBuilder builder;
        builder.AddListeningPort(serverAddress(), grpc::InsecureServerCredentials())
            .RegisterService(&service);
        std::unique_ptr<Server> server(builder.BuildAndStart());
        spdlog::get("console")->info("Server listening on {}", serverAddress());
        std::thread grpcServer([&] { server->Wait(); });
        grpcServer.join();

        return Poco::Util::ServerApplication::EXIT_OK;
    }

  private:
    std::string _ip = "127.0.0.1";
    int _grpcPort = 50051;
};

} // namespace base_station

int main(int argc, char* argv[])
{
    initializeLogger();
    base_station::BaseStation app;
    return app.run(argc, argv);
}
