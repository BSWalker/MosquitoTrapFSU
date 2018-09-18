# makefile for c++ compiling on mosquito trap project
#
# Author: Steven B Walker
# Date:   9/17/18
#
########################################################

CC = g++ -std=c++11 -Wall -Wextra

all: crontab_file_gen.x

crontab_file_gen.x: crontab_file_generator.cpp
	$(CC) -I. -o crontab_file_gen.x crontab_file_generator.cpp
