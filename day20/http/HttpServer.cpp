#include "HttpServer.h"
#include "HttpResponse.h"
#include "HttpRequest.h"
#include "HttpContext.h"
#include "Acceptor.h"
#include "TcpServer.h"
#include "TcpConnection.h"
#include "Buffer.h"
#include "EventLoop.h"
#include "CurrentThread.h"
#include "TimerQueue.h"
#include <arpa/inet.h>
#include <functional>
#include <iostream>
 
void HttpServer::HttpDefaultCallBack(const HttpRequest& request, HttpResponse *resp){
    resp->SetStatusCode(HttpResponse::HttpStatusCode::k404NotFound);
    resp->SetStatusMessage("Not Found");
    resp->SetCloseConnection(true);
}

HttpServer::HttpServer(EventLoop * loop, const char *ip, const int port) : loop_(loop), auto_close_(true) {
    server_ = std::make_unique<TcpServer>(loop_, ip, port);
    server_->setConnectCallback(
        std::bind(&HttpServer::onConnection, this, std::placeholders::_1));

    server_->setMessageCallback(
        std::bind(&HttpServer::onMessage, this, std::placeholders::_1)
    );
    SetHttpCallback(std::bind(&HttpServer::HttpDefaultCallBack, this, std::placeholders::_1, std::placeholders::_2));
};

HttpServer::~HttpServer(){
};

void HttpServer::onConnection(const TcpConnectionPtr &conn){
    int clnt_fd = conn->getFd();
    struct sockaddr_in peeraddr;
    socklen_t peer_addrlength = sizeof(peeraddr);
    getpeername(clnt_fd, (struct sockaddr *)&peeraddr, &peer_addrlength);

    std::cout << CurrentThread::tid()
              << " Server::OnNewConnection : new connection "
              << "[fd#" << clnt_fd << "]"
              << " from " << inet_ntoa(peeraddr.sin_addr) << ":" << ntohs(peeraddr.sin_port)
              << std::endl;
    if (auto_close_) {
         loop_->RunAfter(AUTOCLOSETIMEOUT, std::move(std::bind(&HttpServer::ActiveClose, this, std::weak_ptr<TcpConnection>(conn))));
    }
}

void HttpServer::onMessage(const TcpConnectionPtr &conn){
    if (conn->getState() == TcpConnection::State::Connected)
    {
        if(auto_close_)
            conn->updateLastActiveTime(TimeStamp::now());
        HttpContext *context = conn->context();
        if (!context->ParaseRequest(conn->getRecvBuf()->c_str(), conn->getRecvBuf()->size()))
        {
            conn->Send("HTTP/1.1 400 Bad Request\r\n\r\n");
            conn->handleClose();
        }

        if (context->GetCompleteRequest())
        {
            onRequest(conn, *context->request());
            context->ResetContextStatus();
        }
    }
}

void HttpServer::SetHttpCallback(const HttpServer::HttpResponseCallback &cb){
    response_callback_ = std::move(cb);
}

void HttpServer::onRequest(const TcpConnectionPtr &conn, const HttpRequest &request){
    std::string connection_state = request.GetHeader("Connection");
    bool close = (connection_state == "Close" ||
                  (request.version() == HttpRequest::Version::kHttp10 &&
                  connection_state != "keep-alive"));
    HttpResponse response(close);
    response_callback_(request, &response);

    conn->Send(response.message().c_str());

    if(response.IsCloseConnection()){
        conn->handleClose();
    }
}

void HttpServer::start(){
    server_->Start();
}

void HttpServer::SetThreadNums(int thread_nums) { server_->SetThreadNums(thread_nums); }

void HttpServer::ActiveClose(std::weak_ptr<TcpConnection> & connection){
    auto conn = connection.lock(); //防止conn已经被释放
    if (conn)
    {
        if(TimeStamp::addTime(conn->getLastActiveTime(), AUTOCLOSETIMEOUT) < TimeStamp::now()){
            conn->handleClose();
        }else{
            loop_->RunAfter(AUTOCLOSETIMEOUT, std::move(std::bind(&HttpServer::ActiveClose, this, connection)));
        }
    }
}