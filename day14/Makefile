tcp=$(wildcard tcp/*.cpp)

server:
	g++ -std=c++17 -pthread -g \
	$(tcp) \
	server.cpp \
	-o server
	
single_client:
	g++ tcp/util.cpp tcp/Buffer.cpp single_client.cpp -o single_client

ThreadPoolTest:
	g++ -pthread tcp/ThreadPool.cpp ThreadPoolTest.cpp -o ThreadPoolTest

multiple_client:
	g++ tcp/util.cpp tcp/Buffer.cpp tcp/Socket.cpp tcp/InetAddress.cpp tcp/ThreadPool.cpp \
	-pthread \
	multiple_client.cpp -o multiple_client

clean:
	rm server && rm single_client && rm multiple_client