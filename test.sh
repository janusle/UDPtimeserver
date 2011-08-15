#!/bin/bash

make clean
make
clear

./time-server server.config
