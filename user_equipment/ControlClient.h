//
// Created by seflue on 11.05.2023.
//

#ifndef NETWORK_COMPONENTS_CONTROLCLIENT_H
#define NETWORK_COMPONENTS_CONTROLCLIENT_H

#include "controlplane.grpc.pb.h"
#include <grpcpp/grpcpp.h>
#include "absl/flags/parse.h"
#include "absl/flags/flag.h"
#include "DataObjects.h"
#include <vector>
#include <string>
#include <memory>
#include <iostream>

namespace user_equipment{

struct Channel {
    std::unique_ptr<control::ControlService::Stub> service;
    std::string address;
    int quality;
};

class ControlClient {
public:
    void addChannel(const BaseStation& bs);

    // Assembles the client's payload, sends it and presents the response back
    // from the server.
    void scan(const std::string& ueid);

private:
    std::string ueid;
    std::map<std::string, Channel> channels_;
};

}

#endif //NETWORK_COMPONENTS_CONTROLCLIENT_H
