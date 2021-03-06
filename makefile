# makefile for c++ compiling on mosquito trap project
#
# Author: Steven B Walker
# Date:   9/17/18
#
########################################################

CC = g++ -std=c++11 -Wall -Wextra -I.
LNK = -lpthread -lrt -lwiringPi

all: crontab_file_gen.x fansense.x tempsense.x adc.x start_collection.x stop_collection.x trap_test.x pressure_sensor.x 

crontab_file_gen.x: crontab_file_generator.cpp
	$(CC) -o crontab_file_gen.x crontab_file_generator.cpp

shmtest.x: shmtest.o
	$(CC) -o shmtest.x shmtest.o mosq_shm.o $(LNK)

shmtest2.x: shmtest2.o
	$(CC) -o shmtest2.x shmtest2.o mosq_shm.o $(LNK)

fansense.x: fan_rpm_sense.o mosq_shm.o
	$(CC) -o fansense.x fan_rpm_sense.o mosq_shm.o $(LNK)

tempsense.x: temp_sense.o mosq_shm.o
	$(CC) -o tempsense.x temp_sense.o mosq_shm.o $(LNK)

adc.x: adc.o
	$(CC) -o adc.x adc.o mosq_shm.o $(LNK)
		  
pressure_sensor.x: pressure_sensor.o
	$(CC) -o pressure_sensor.x pressure_sensor.o mosq_shm.o $(LNK)
				 
start_collection.x: start_collection.o log.o
	$(CC) -o start_collection.x start_collection.o mosq_shm.o log.o $(LNK)
					    
stop_collection.x: stop_collection.o log.o
	$(CC) -o stop_collection.x stop_collection.o mosq_shm.o log.o $(LNK)

trap_test.x: trap_test.o log.o
	$(CC) -o trap_test.x trap_test.o mosq_shm.o log.o $(LNK)

#logsensors.x: logsensors.o mosq_shm.o
#	$(CC) -o logsensors.x logsensors.o mosq_shm.o $(LNK)

mosq_shm.o: mosq_shm.cpp mosq_shm.h
	$(CC) -c mosq_shm.cpp $(LNK)

shmtest.o: shmtest.cpp
	$(CC) -c shmtest.cpp $(LNK)

shmtest2.o: shmtest2.cpp
	$(CC) -c shmtest2.cpp $(LNK)

fan_rpm_sense.o: fan_rpm_sense.cpp
	$(CC) -c fan_rpm_sense.cpp $(LNK)

temp_sense.o: temp_sense.cpp
	$(CC) -c temp_sense.cpp $(LNK)

sensortest.o: sensortest.cpp
	$(CC) -c sensortest.cpp $(LNK)

#logsensors.o: logsensors.cpp
#	$(CC) -c logsensors.cpp $(LNK)

adc.o: adc.cpp
	$(CC) -c adc.cpp
		  
pressure_sensor.o: pressure_sensor.cpp
	$(CC) -c pressure_sensor.cpp
				 
start_collection.o: start_collection.cpp
	$(CC) -c start_collection.cpp
					    
stop_collection.o: stop_collection.cpp
	$(CC) -c stop_collection.cpp
							   
trap_test.o: trap_test.cpp
	$(CC) -c trap_test.cpp

log.o: log.h log.cpp	
	$(CC) -c log.cpp

