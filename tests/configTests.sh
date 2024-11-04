#!/bin/bash

TESTS_DIR="./config"

run_test() {
    local config_file=$1
    echo -e "Running test for configuration file: $config_file"
    
    ../webserv "$config_file"
    if [ $? -ne 0 ]; then
        echo -e "Error handled: $config_file\n"
    else
        echo -e "Error not handled $config_file\n"
        echo -e "Test failed $config_file: Error not handled\n"
        exit 1
    fi
}

find "$TESTS_DIR" -type f -name "*.conf" | while read config_file; do
    run_test "$config_file"
done
