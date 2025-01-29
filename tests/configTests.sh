#!/bin/bash

RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m' 

TESTS_DIR="./config"
total_tests=0
failed_tests=0

run_test() {
    local config_file=$1
    echo -e "Running test for configuration file: $config_file"
    total_tests=$((total_tests + 1))

    ../webserv "$config_file"
    if [ $? -ne 0 ]; then
        echo -e "${GREEN}Error handled: $config_file\n${NC}"
        failed_tests=$((failed_tests + 1))
    else
        echo -e "${RED}Error not handled $config_file\n${NC}"
        echo -e "${RED}Test failed $config_file: Error not handled\n${NC}"
        exit 1
    fi
}

find "$TESTS_DIR" -type f -name "*.conf" | while read config_file; do
    run_test "$config_file"
done

if [ $failed_tests -ne $total_tests ]; then
    echo -e "${RED}Some incorrect config not handled\n${NC}"
    exit 1
else
    exit 0
fi