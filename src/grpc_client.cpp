#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <future>

// Include the needed gRPC headers
#include <grpcpp/grpcpp.h>

// Include the gRPC generated header
#include "jccapstone.grpc.pb.h"

#include "grpc_client.hpp"

using grpc::ClientContext;
using grpc::Status;
using jccapstone::MsgRequest;
using jccapstone::MsgReply;
using jccapstone::Capstone;

// Custom method to call the RPC enpoint:
//   -It assembles the client's payload, sends it, and
//    presents the response back from the gRPC server
void CapstoneClient::CallCapostoneDone(std::promise<std::string> &&prms, const std::string& mtype, const std::string& mpayload) {
	// Placeholder for the promise
	std::string retval;

	// Prepare the message to call the RPC endpoint
    	MsgRequest request;
    	request.set_msg_type(mtype);
	request.set_msg_payload(mpayload);

    	// Container for the data we expect from the RPC endpoint
    	MsgReply reply;

    	// Context for the client:
	//   - It could be used to convey extra information to
    	//     the server and/or tweak certain RPC behaviors
    	ClientContext context;

    	// Call the RPC endpoint
    	Status status = stub_->CapstoneDone(&context, request, &reply);

    	// Act upon its status.
    	if (status.ok()) {
		retval = reply.message();
    	} else {
		std::cout << status.error_code() << ": " << status.error_message() << std::endl;
      		retval = "RPC failed";
    	}

	// Set the promise value
	prms.set_value(retval);
 }


int main(int argc, char** argv) {
	// Process the command line arguments
  	std::string target_str;
  	std::string arg_str("--target");
  	if (argc > 1) {
		std::string arg_val = argv[1];
    		size_t start_pos = arg_val.find(arg_str);
    		if (start_pos != std::string::npos) {
			start_pos += arg_str.size();
      			if (arg_val[start_pos] == '=') {
				target_str = arg_val.substr(start_pos + 1);
      			} else {
				std::cout << "The only correct argument syntax is --target=" << std::endl;
        			return 0;
      			}
    		} else {
			std::cout << "The only acceptable argument is --target=" << std::endl;
      			return 0;
    		}
  	} else {
		// Default argument
    		target_str = "localhost:50051";
  	}

  	// Create the channel to the gRPC service
  	auto channel = grpc::CreateChannel(target_str, grpc::InsecureChannelCredentials());

  	// Instantiate the client to the gRPC service
  	CapstoneClient *client = new CapstoneClient(channel);

  	// Prepare the arguments for the RPC endpoint
  	std::string mtype("up");
	std::string mpayload("Udacity");

	// Create the promise and future to receive the answer from the worker thread
        std::promise<std::string> prms;
        std::future<std::string> ftr = prms.get_future();
	
	// Start the worker thread passing it a member function and its arguments:
	//   - The thread worker is in charge of calling the RPC endpoint on the gRPC service
        std::thread t(&CapstoneClient::CallCapostoneDone, client, std::move(prms), mtype, mpayload);
	
	// Retrieve the message from the RPC endpoing via future:
	//   -It blocks the parent thread until the answer is available
        std::string reply = ftr.get();

  	std::cout << "The final evaluation is: " << reply << std::endl;

	// Thread barrier
	t.join();

  	return 0;
}
