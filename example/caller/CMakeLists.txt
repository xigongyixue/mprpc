set(SRC_LIST calluserservice.cc ../user.pb.cc)
add_executable(consummer ${SRC_LIST})
target_link_libraries(consummer mprpc protobuf)