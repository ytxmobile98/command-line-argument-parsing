#!/usr/bin/env sh

while getopts "ab:" opt; do
    case $opt in
        a) echo "Option $opt" ;;
        b) echo "Option $opt: $OPTARG" ;;
        \?) echo "Invalid option: -$opt" ;;
    esac
done