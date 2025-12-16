# 将所有源文件加入到 SRC_LIST 变量中
aux_source_directory(. SRC_LIST)
# 生成动态库 mprpc
add_library(mprpc SHARED ${SRC_LIST})