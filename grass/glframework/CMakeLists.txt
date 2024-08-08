file(GLOB_RECURSE FW ./ *.cpp)

# 将APP中的所有cpp编译为fw这个lib库
add_library(fw ${FW})