#!/bin/bash

# 执行 cmake 命令
cmake -B build
if [ $? -ne 0 ]; then
    echo "Error: cmake command failed."
    exit 1
fi

# 执行 make 命令
make
if [ $? -ne 0 ]; then
    echo "Error: make command failed."
    exit 1
fi

# 获取传入的参数
test_name="$1"

# 运行测试程序
./bin/"$test_name"
if [ $? -ne 0 ]; then
    echo "Error: test program failed."
    exit 1
fi

echo "All commands executed successfully."
