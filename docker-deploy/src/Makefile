all: main

main: main.cpp proxy.cpp log.cpp cache.cpp request.cpp response.cpp socket.cpp
	g++ -g  -o main main.cpp proxy.cpp log.cpp cache.cpp request.cpp response.cpp socket.cpp -lpthread
test: test.cpp proxy.cpp log.cpp cache.cpp request.cpp response.cpp socket.cpp
	g++ -g  -o test test.cpp proxy.cpp log.cpp cache.cpp request.cpp response.cpp socket.cpp -lpthread


.PHONY:
	clean
clean:
	rm -rf *.o main test