#include "EventLoop.h"
#include "Epoll.h"
#include "Channel.h"
#include "CurrentThread.h"
#include <unistd.h>
#include <assert.h>
#include <vector>
#include <sys/eventfd.h>

EventLoop::EventLoop(): quit(false), tid(CurrentThread::tid()) {
    ep = std::make_unique<Epoll>();
    wakeup_fd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    wakeup_ch = std::make_unique<Channel>(this, wakeup_fd);
    calling_functors = false;
    wakeup_ch->setReadCallback(std::bind(&EventLoop::HandleRead, this));
    wakeup_ch->enableReading();
}

EventLoop::~EventLoop() {
    deleteChannel(wakeup_ch.get());
    ::close(wakeup_fd);
}

void EventLoop::loop() {
    while (!quit) {
        std::vector<Channel*>chs = ep->poll();
        for (auto ch : chs) {
            ch->handleEvent();
        }
        DoToDoList();
    }
}

void EventLoop::updateChannel(Channel* ch) {
    ep->updateChannel(ch);
}

void EventLoop::deleteChannel(Channel* ch) {
    ep->deleteChannel(ch);
}

bool EventLoop::IsInLoopThread(){
    return CurrentThread::tid() == tid;
}

void EventLoop::RunOneFunc(std::function<void()> cb){
    if(IsInLoopThread()){
        cb();
    }else{
        QueueOneFunc(cb);
    }
}

void EventLoop::QueueOneFunc(std::function<void()> cb){
    {
        std::unique_lock<std::mutex> lock(mtx);
        to_do_list.emplace_back(std::move(cb));
    }
    if (!IsInLoopThread() || calling_functors) {
        uint64_t write_one_byte = 1;  
        ssize_t write_size = ::write(wakeup_fd, &write_one_byte, sizeof(write_one_byte));
        (void) write_size;
        assert(write_size == sizeof(write_one_byte));
    } 
}

void EventLoop::DoToDoList(){
    calling_functors = true;
    std::vector < std::function<void()>> functors;
    {
        std::unique_lock<std::mutex> lock(mtx); 
        functors.swap(to_do_list);
    }
    for(const auto& func: functors){
        func();
    }

    calling_functors = false;
}

void EventLoop::HandleRead(){
    uint64_t read_one_byte = 1;
    ssize_t read_size = ::read(wakeup_fd, &read_one_byte, sizeof(read_one_byte));
    (void) read_size;
    assert(read_size == sizeof(read_one_byte));
    return;
}
