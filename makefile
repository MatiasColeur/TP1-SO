#!/bin/bash

gcc  -fsanitize=address -g -Wall main.c -o run.o beSafe.c beSafe.h
