cmake_minimum_required(VERSION 3.22)
project(PackerTool LANGUAGES C)

add_executable(packer packer.c stringHash.c)

target_include_directories(packer PRIVATE ../../include)