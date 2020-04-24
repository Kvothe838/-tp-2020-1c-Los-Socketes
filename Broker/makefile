COMPILER=gcc
BIN_PATH=bin

all:
	make client

clean:
	rm -rf bin

client:
	mkdir -p $(BIN_PATH)
	$(COMPILER) src/broker.c -g -Wall -lreadline -lcommons -o $(BIN_PATH)/broker
