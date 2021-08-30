#! /bin/sh

DIR=$(dirname "$0")

clang-tidy $DIR/*.cpp \
    -warnings-as-errors=* \
    -header-filter=.* \
    -quiet \
    -checks=-*,clang-analyzer-*,cppcoreguidelines-*,hicpp-*,misc-*,modernize-*,performance-*,portability-*,readability-* \
    -- -std=c++17 && \
clang++ -Wall -Wextra -Wpedantic -Werror -std=c++17 $DIR/*.cpp -lpthread
