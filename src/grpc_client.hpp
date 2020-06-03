#ifndef GRPC_CLIENT_HPP_
#define GRPC_CLIENT_HPP_

#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <future>

// Include the needed gRPC headers
#include <grpcpp/grpcpp.h>

// Include the gRPC generated header
#include "jccapstone.grpc.pb.h"

using grpc::Channel;
using jccapstone::Capstone;

// It defines the stub that allows to call the RPC endpoints from the defined gRPC service
class CapstoneClient {
	public:
		// Constructor:
  		//   - The argument is the channel to communicate with the gRPC service
  		CapstoneClient(std::shared_ptr<Channel> channel) : stub_(Capstone::NewStub(channel)) {}

  		// Custom method to call the RPC enpoint
  		void CallCapostoneDone(std::promise<std::string> &&prms, const std::string& mtype, const std::string& mpayload);

 	private:
		std::unique_ptr<Capstone::Stub> stub_;
};

#endif /* GRPC_CLIENT_HPP_ */
