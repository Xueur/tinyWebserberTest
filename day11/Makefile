client:
	g++ util.cpp client.cpp InetAddress.cpp Socket.cpp Buffer.cpp -o client -g

server:
	g++ util.cpp server.cpp Epoll.cpp InetAddress.cpp Socket.cpp Channel.cpp Server.cpp EventLoop.cpp Acceptor.cpp Connection.cpp Buffer.cpp ThreadPool.cpp -o server -g

clean:
	rm server && rm client

test:
	g++ -pthread util.cpp Buffer.cpp Socket.cpp InetAddress.cpp ThreadPool.cpp test.cpp -o test

threadTest:
	g++ -pthread ThreadPool.cpp ThreadPoolTest.cpp -o ThreadPoolTest