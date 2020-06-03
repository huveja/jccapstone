cmake_minimum_required(VERSION 3.13)

project(JCCapstone C CXX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

find_package(Threads REQUIRED)

# Find Protobuf installation
set(protobuf_MODULE_COMPATIBLE TRUE)
find_package(Protobuf CONFIG REQUIRED)
message(STATUS "*** Using protobuf ${Protobuf_VERSION}")

set(_PROTOBUF_LIBPROTOBUF protobuf::libprotobuf)
set(_REFLECTION gRPC::grpc++_reflection)

# Find the gRPC compiler
find_program(_PROTOBUF_PROTOC protoc)
# Find the protoc version
execute_process(
	COMMAND ${_PROTOBUF_PROTOC} --version
	RESULT_VARIABLE _status
	OUTPUT_VARIABLE _protoc_version
	ERROR_QUIET
	OUTPUT_STRIP_TRAILING_WHITESPACE
)
message(STATUS "*** Using protoc ${_protoc_version}")

# Check the 'Protobuf' version against the 'protoc' version:
#   - They must match
#
# Convert the string 'Protobuf_VERSION' into a list (i.e.
# a string where the different elements are separated by ';'
string(REPLACE "." ";" _list_buf_ver ${Protobuf_VERSION})
# Get the first 3 elements from the list
list (GET _list_buf_ver 0 _buf_major)
list (GET _list_buf_ver 1 _buf_minor)
list (GET _list_buf_ver 2 _buf_patch)
# Generate a new string with these 3 first elements
set(_buf_version "${_buf_major}.${_buf_minor}.${_buf_patch}")
# Check that the end of the string '_protoc_version' matches with '_buf_version'
if(_protoc_version MATCHES "${_buf_version}$")
	message(STATUS "***   The 'Protobuf' and 'protoc' versions are matching!")
else()
	# Abort CMake if the versions are not matching
	#
	#   The problem it could give is the following:
	#     - https://stackoverflow.com/questions/35744529/protocol-buffer-error-on-compile-during-google-protobuf-min-protoc-version-check
	#   Potential solution to the problem:
	#     1. Check where is located the found 'protoc':
	#          - Run: 'which protoc'
	#     2. Replace this exectutable for an executable with the right version:
	#          - Download the right version from https://github.com/protocolbuffers/protobuf/tags
	#              - See more at:
	#                  - https://stackoverflow.com/questions/52040428/how-to-update-protobuf-runtime-library
	message(FATAL_ERROR "***   The 'Protobuf' and 'protoc' versions are not matching!" )
endif()


# Find gRPC installation
find_package(gRPC CONFIG REQUIRED)
message(STATUS "*** Using gRPC ${gRPC_VERSION}")
set(_GRPC_GRPCPP gRPC::grpc++)

# Find the gRPC plugin to generate the service interface and stubs
find_program(_GRPC_CPP_PLUGIN_EXECUTABLE grpc_cpp_plugin)

# Get the needed proto file
get_filename_component(hw_proto "./protos/jccapstone.proto" ABSOLUTE)
get_filename_component(hw_proto_path "${hw_proto}" PATH)

# Call gRPC to generate the needed code
set(hw_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/jccapstone.pb.cc")
set(hw_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/jccapstone.pb.h")
set(hw_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/jccapstone.grpc.pb.cc")
set(hw_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/jccapstone.grpc.pb.h")
add_custom_command(
      OUTPUT "${hw_proto_srcs}" "${hw_proto_hdrs}" "${hw_grpc_srcs}" "${hw_grpc_hdrs}"
      COMMAND ${_PROTOBUF_PROTOC}
      ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
        --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
        -I "${hw_proto_path}"
        --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
        "${hw_proto}"
      DEPENDS "${hw_proto}")

# Include the automatically generated *.pb.h files
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

# Define the gRPC executable targets
foreach(_target
  grpc_client grpc_server)
  add_executable(${_target} "src/${_target}.cpp"
    ${hw_proto_srcs}
    ${hw_grpc_srcs})
  target_link_libraries(${_target}
    ${_REFLECTION}
    ${_GRPC_GRPCPP}
    ${_PROTOBUF_LIBPROTOBUF})
endforeach()
