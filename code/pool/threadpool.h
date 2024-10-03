#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <thread>
#include <assert.h>
using namespace std;

class ThreadPool{
public:
  ThreadPool()=default;
  ThreadPool(ThreadPool&&)=default;
  explicit ThreadPool(int threadCount=8):pool_(make_shared<Pool>()){
    assert(threadCount>0);
    for(int i=0;i<8;i++){
      thread([this](){
        unique_lock<mutex>locker(pool_->mtx_);
        while(true){
          if(!pool_->tasks.empty()){
            auto task = move(pool_->tasks.front());
            pool_->tasks.pop();
            locker.unlock();
            task();
            locker.lock();
          }
          else if(pool_->isClosed){
              break;
            }
            else{
              pool_->cond_.wait(locker);// 等待,如果任务来了就notify
            }
        }
      }).detach();
    }

  }

  ~ThreadPool(){
    if(pool_){
      unique_lock<mutex>locker(pool_->mtx_);
      pool_->isClosed=true;
    }
    pool_->cond_.notify_all();
  }

  template<typename A>
  void AddTask(A&& task){
    unique_lock<mutex>locker(pool_->mtx_);
    pool_->tasks.emplace(forward<A>(task));
    pool_->cond_.notify_one();
  }
private:
  struct Pool{
    mutex mtx_;
    bool isClosed;
    condition_variable cond_;
    queue<function<void()>>tasks;// 任务队列，函数类型为void()
  };
  shared_ptr<Pool> pool_;





};


#endif

