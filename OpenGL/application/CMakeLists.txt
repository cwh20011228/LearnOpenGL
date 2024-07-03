# 递归将本文件夹下的所有cpp放到变量APP中
file(GLOB_RECURSE APP ./ *.cpp)

# 将APP中的所有cpp编译为app这个lib库
add_library(app ${APP})