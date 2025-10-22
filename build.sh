#!/bin/bash

gcc ./*.c ./*.h lib/cJSON/cJSON.c -Wall -I./ -I./lib/cJSON -o game.out