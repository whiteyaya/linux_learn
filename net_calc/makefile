.PHONY:all
all:client server
client:client.cc
	g++ -o $@ $^ -std=c++11 -ljsoncpp
server:server.cc
	g++ -o $@ $^ -std=c++11 -lpthread -ljsoncpp
.PHONY:
clean:
	rm -f client server

