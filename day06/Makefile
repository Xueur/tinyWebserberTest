client:
	g++ util.cpp client.cpp InetAddress.cpp -o client 

server:
	g++ util.cpp server.cpp Epoll.cpp InetAddress.cpp Socket.cpp Channel.cpp Server.cpp EventLoop.cpp -o server

clean:
	rm server && rm client