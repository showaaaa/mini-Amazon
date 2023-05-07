CFLAGS = -g -Wall
LDFLAGS = -lpqxx -lpq -pthread
GPB = `pkg-config --cflags --libs protobuf`
VERSION = -std=c++11
all: server

server:  server.h AResponseHandler.h orderProcess.h sql_helper.h UMessageHandler.h world_amazon.pb.h sbuf.h exception.h socket.h protobufCommunication.hpp function.h client_info.h order.h Warehouse.h
	g++ -o server main.cpp server.cpp sql_helper.cpp AResponseHandler.cpp orderProcess.cpp UMessageHandler.cpp socket.cpp function.cpp world_amazon.pb.cc $(LDFLAGS) $(CFLAGS) $(GPB) $(VERSION)

.PHONY:
	clean
clean:
	rm -rf *.o server  




