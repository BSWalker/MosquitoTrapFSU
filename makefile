# makefile for c++ compiling on mosquito trap project
#
# Author: Steven B Walker
# Date:   9/17/18
#
########################################################

CC = g++ -std=c++11 -Wall -Wextra -I.
LNK = -lpthread -lrt

all: crontab_file_gen.x lmosqshm.a shmtest.x shmtest2.x

crontab_file_gen.x: crontab_file_generator.cpp
	$(CC) -o crontab_file_gen.x crontab_file_generator.cpp

shmtest.x: shmtest.o lmosqshm.a
	$(CC) -o shmtest.x shmtest.o mosq_shm.o $(LNK)

shmtest2.x: shmtest2.o lmosqshm.a
	$(CC) -o shmtest2.x shmtest2.o mosq_shm.o $(LNK)

mosq_shm.o: mosq_shm.cpp mosq_shm.h
	$(CC) -c mosq_shm.cpp $(LNK)

lmosqshm.a: mosq_shm.o
	ar -cvq lmosqshm.a mosq_shm.o

shmtest.o: shmtest.cpp lmosqshm.a
	$(CC) -c shmtest.cpp $(LNK)

shmtest2.o: shmtest.cpp lmosqshm.a
	$(CC) -c shmtest2.cpp $(LNK)
