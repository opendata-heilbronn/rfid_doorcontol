test:
	gcc -g -Wall --std=c++11 -c main.cpp ACR122U.cpp -lpthread -lm -pthread -I/usr/local/include/PCSC 
	gcc -g -Wall --std=c++11 -o door main.o ACR122U.o -lpthread -lm -L/usr/lib/x86_64-linux-gnu/ -L/usr/local/lib -lpcsclite -lstdc++ -lsqlite3 -lwiringPi

.PHONY: test
