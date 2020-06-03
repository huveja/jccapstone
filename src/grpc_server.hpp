#ifndef GRPC_SERVER_HPP_
#define GRPC_SERVER_HPP_

#include <iostream>
#include <memory>
#include <string>

// Include the needed gRPC headers
#include <grpcpp/grpcpp.h>

// Include the gRPC generated header
#include "jccapstone.grpc.pb.h"

using grpc::ServerContext;
using grpc::Status;
using jccapstone::MsgRequest;
using jccapstone::MsgReply;
using jccapstone::Capstone;

// Subclass the service definition to implment the logic and data
// behind the gRPC service
class CapstoneServiceImpl final : public Capstone::Service {
	// Custome method that implements the RPC endpoint
	Status CapstoneDone(ServerContext* context, const MsgRequest* request, MsgReply* reply) override;
};

void RunServer();

#endif /* GRPC_SERVER_HPP_ */
