#!/bin/bash 
g++ *.cpp ../shared/*.cpp ../shared/*/*.cpp -o client -I../shared -lssl
