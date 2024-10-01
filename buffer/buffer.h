#ifndef BUFFER_H
#define BUFFER_H
#include <cstring>   //perror
#include <iostream>
#include <unistd.h>  // write
#include <sys/uio.h> //readv
#include <vector> //readv
#include <atomic>
#include <assert.h>
using namespace std;
class Buffer{
public:
  //构造函数 析构函数
  Buffer(int initBuffersize = 1024);
  ~Buffer()=default;

  //可读 可写 预留大小 
  size_t WritableBytes() const;
  size_t ReadableBytes() const;
  size_t PrependableBytes() const;

  //?
  //获取读下标的元素
  const char* Peek() const;
  
  //确保可写的长度
  void EnsureWriteable(size_t len);
  //移动写下标，在Append中使用
  void HasWritten(size_t len);

  //读取
  void Retrive(size_t len);
  void RetriveUntil(const char* end);
  void RetriveAll();
  std::string  RetriveAllToStr();

  //?                
  //获取写下标的元素 
  char* WriteBegin();
  const char* WriteBeginConst() const;

  //写
  void Append(const string& str);
  void Append(const void* data, size_t len);
  void Append(const char* str, size_t len);
  void Append(const Buffer& buff);

  //
  ssize_t ReadFd(int fd, int* Error);
  ssize_t WriteFd(int fd, int* Error);
private:
  vector<char>buffer_;
  atomic<size_t>readPos_;
  atomic<size_t>writePos_;

  char* BeginPtr();
  const char* BeginPtrConst() const;
  void MakeSpace(size_t len);
}
 #endif
