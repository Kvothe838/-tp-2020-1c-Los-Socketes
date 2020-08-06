all:
	-cd Shared/Debug && make all 
	-cd Broker/Debug && make all
	-cd GameCard/Debug && make all
	-cd GameBoy/Debug && make all
	-cd Team/Debug && make all

clean:
	-cd Shared/Debug && make clean 
	-cd Broker/Debug && make clean
	-cd GameCard/Debug && make clean
	-cd GameBoy/Debug && make clean
	-cd Team/Debug && make clean
