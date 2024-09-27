#!/bin/bash
gcc main.c process_keyevent.c -o main -lX11 -lGL -lGLX -lGLU && ./main
