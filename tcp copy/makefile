.PHONY:all
all:client server
client:client.c
	g++ -o $@ $^ -std=c++11
server:server.c
	g++ -o $@ $^ -std=c++11 -lpthread
.PHONY:
clean:
	rm -f client server

