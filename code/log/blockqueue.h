# ifndef BLOCKQUEUE_H
# define BLOCKQUEUE_H

#include <deque>
#include <condition_variable>
#include <mutex>
#include <sys/time.h>
using namespace std;

template<typename T>
class Blockqueue{
public:
  explicit Blockqueue(size_t maxSize=1000);
  ~Blockqueue();
  bool full();
  bool empty();
  void push_back(const T& item);
  void push_front(const T& item);
  bool pop(T& item);
  bool pop(T& item, int timeout);
  void close();
  void flush();
  void clear();
  T front();
  T back();
  size_t capacity();
  size_t size();

private:
  deque<T>deq_;
  mutex mtx_;
  bool isClosed;
  condition_variable condConsumer_;
  condition_variable condProducer_;
  size_t capacity_;
};


template <typename T>
Blockqueue<T>::Blockqueue(size_t maxSize):capacity_(maxSize){
  assert(maxSize>0);
  isClosed=false;
}

template<typename T>
Blockqueue<T>::~Blockqueue(){close();}

template<typename T> 
void Blockqueue<T>::clear(){
  lock_guard<mutex>locker(mtx_);
  deq_.clear();
}

template<typename T> 
void Blockqueue<T>::close(){
  clear();
  isClosed=true;
  condConsumer_.notify_all();
  condProducer_.notify_all();
}

template<typename T>
bool Blockqueue<T>::empty(){ 
  lock_guard<mutex>locker(mtx_);
  return deq_.empty();
}

template<typename T>
bool Blockqueue<T>::full(){ 
  lock_guard<mutex>locker(mtx_);
  return deq_.size() >= capacity_;
}


template<typename T>
void Blockqueue<T>::push_back(const T& item){ 
  unique_lock<mutex>locker(mtx_);
  while(deq_.size()>=capacity_){
    condProducer_.wait(locker);
  }
  deq_.push_back(item);
  condConsumer_.notify_one();
}

template<typename T>
void Blockqueue<T>::push_front(const T& item){ 
  unique_lock<mutex>locker(mtx_);
  while(deq_.size()>=capacity_){
    condProducer_.wait(locker);
  }
  deq_.push_front(item);
  condConsumer_.notify_one();
}


template<typename T>
bool Blockqueue<T>::pop(T& item){ 
  unique_lock<mutex>locker(mtx_);
  while(deq_.empty()){
    condProducer_.wait(locker);
  }
  item = deq_.front();
  deq_.pop_front();
  condConsumer_.notify_one();
  return true;
}

template<typename T>
bool Blockqueue<T>::pop(T& item, int timeout){ 
  unique_lock<mutex>locker(mtx_);
  while(deq_.empty()){
    if(condProducer_.wait_for(locker, chrono::seconds(timeout))==cv_status::timeout){return false;}
    if(isClosed){return false;}
  }
  item = deq_.front();
  deq_.pop_front();
  condConsumer_.notify_one();
  return true;
}


template<typename T>
T Blockqueue<T>::front(){ 
  lock_guard<mutex>locker(mtx_);
  return deq_.front();
}

template<typename T>
T Blockqueue<T>::back(){ 
  lock_guard<mutex>locker(mtx_);
  return deq_.back();
}


template<typename T>
size_t Blockqueue<T>::capacity(){ 
  lock_guard<mutex>locker(mtx_);
  return deq_.capacity();
}

template<typename T>
size_t Blockqueue<T>::size(){ 
  lock_guard<mutex>locker(mtx_);
  return deq_.size();
}

// 唤醒消费者
template<typename T>
void Blockqueue<T>::flush() {
    condConsumer_.notify_one();
}

# endif
