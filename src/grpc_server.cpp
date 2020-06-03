#include <iostream>
#include <memory>
#include <string>

// Include the needed gRPC headers
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>
#include <grpcpp/ext/proto_server_reflection_plugin.h>

// Include the gRPC generated header
#include "jccapstone.grpc.pb.h"

#include "grpc_server.hpp"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using jccapstone::MsgRequest;
using jccapstone::MsgReply;
using jccapstone::Capstone;

// Custom method that implements the RPC endpoint
Status CapstoneServiceImpl::CapstoneDone(ServerContext* context, const MsgRequest* request, MsgReply* reply) {
	// Placeholder for the RPC endpoint's answer
	std::string msg_answer;

	// Get the message content
	auto mtype = request->msg_type();
	auto mpayload = request->msg_payload();

	// Build the message answer
	if (mtype == "up") {
		msg_answer = "UP " + mpayload;
	} else {
		msg_answer = "DOWN " + mpayload;
	}

	// Send the answer
	reply->set_message(msg_answer);
	return Status::OK;
}

void RunServer() {
	std::string server_address("0.0.0.0:50051");
	CapstoneServiceImpl service;

	 grpc::EnableDefaultHealthCheckService(true);
	 grpc::reflection::InitProtoReflectionServerBuilderPlugin();
	 ServerBuilder builder;

	 // Listen on the given address without any authentication mechanism
	 builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());

	 // Register the "service" as the instance to be used by te clients:
	 //   - The communication with the client is 'synchronous'
	 builder.RegisterService(&service);

	 // Build the server
	 std::unique_ptr<Server> server(builder.BuildAndStart());
	 std::cout << "Server listening on " << server_address << std::endl;

	 // Wait for the server to shutdown
	 server->Wait();
}

int main(int argc, char** argv) {
	// Start the gRPC server
  	RunServer();

  	return 0;
}
