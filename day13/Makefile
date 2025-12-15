src=$(wildcard src/*.cpp)

server:
	g++ -std=c++17 -pthread -g \
	$(src) \
	server.cpp \
	-o server
	
single_client:
	g++ src/util.cpp src/Buffer.cpp src/Socket.cpp src/InetAddress.cpp single_client.cpp -o single_client

ThreadPoolTest:
	g++ -pthread src/ThreadPool.cpp ThreadPoolTest.cpp -o ThreadPoolTest

multiple_client:
	g++ src/util.cpp src/Buffer.cpp src/Socket.cpp src/InetAddress.cpp src/ThreadPool.cpp \
	-pthread \
	multiple_client.cpp -o multiple_client

clean:
	rm server && rm single_client && rm multiple_client