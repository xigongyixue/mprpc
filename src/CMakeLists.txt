# 将所有源文件加入到 SRC_LIST 变量中
aux_source_directory(. SRC_LIST)
# 生成动态库 mprpc
add_library(mprpc ${SRC_LIST})
# 链接外部库
target_link_libraries(mprpc muduo_net muduo_base pthread)