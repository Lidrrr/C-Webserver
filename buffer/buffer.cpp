#include "buffer.h"
using namespace std;

Buffer::Buffer(int initBufferSize): buffer_(initBufferSize), readPos_(0),writePos(0){}

size_t Buffer::WritableBytes() const{
  return buffer_.size()-writePos_;
}

size_t Buffer::ReadableBytes() const{
  return writePos_-readPos_;
}

size_t Buffer:: PrependableBytes() const{
  return readPos_;
}

const char* Buffer::Peek() const{
  return &buffer_[readPos_];
}

void Buffer::EnsureWriteable(size_t len){
  if(len>WritableBytes()){
    MakeSpace(len);
  }
  assert(len<=WritableBytes());
}

void Buffer::HasWritten(size_t len){
  writePos_+=len;
} 

void Buffer::Retrive(size_t len){
  readPos_+=len;
}

void Buffer::RetriveUntil(const char* end){
  assert(Peek()<=end);
  Retrive(end-Peek());
}

void Buffer::RetriveAll(){
  bzero(&buffer[0], buffer_.size());//覆盖原本数据
  readPos_=writePos_=0;
}

// 取出剩余可读的str
void Buffer::RetriveAllToStr(){
  string str(Peek(),);
  RetriveAll;
  return str;
}

char* Buffer::WriteBegin(){
  return &buffer_[writePos_];
}


const char* Buffer::WriteBeginConst() const{
  return &buffer_[writePos_];
}

void Buffer::Append(const char* str, size_t len){
  assert(str);
  EnsureWriteable(len);
  copy(str, str+len, WriteBegin())//将str放到写下标开始的地方
  HasWritten(len);
}

void Buffer::Append(const string& str){
  Append(str.c_str(), str.size());
}

void Buffer::Append(const void* data, size_t len){
  Append(static_cast<const char*>(data), len);
}

//将buffer中的读下标的地方放到该buffer中的写下标位置
void Buffer::Append(const Buffer& buff){
  Append(buff.Peak(), ReadableBytes());
}

char* BeginPtr(){
  return &buffer_[0];
}

const char* BeginPtrConst() const{
  return &buffer_[0];
}

//
ssize_t ReadFd(int fd, int* Error){
  char buff[65535];
  struct iovec iov[2];
  size_t writeable=WritableBytes();

  iov[0].iov_base = BeginWrite();
  iov[0].iov_len = writeable;
  iov[1].iov_base = buff;
  iov[1].iov_len = sizeof(buff);
  
  ssize_t len = readv(fd, iov, 2);

  if(len<0){
    *Error = errno;
  }
  else if(static_cast<size_t>len <= writeable){
    writePos_+=len;
  }
  else{
    writePos=buffer_.size();
    Append(buff, static_cast<size_t>(len-writeable));
  }
  return len;
}

//
ssize_t WriteFd(int fd, int* Error){
  ssize_t len = write(fd, Peek(), ReadableBytes());

  if(len<0){
    *Error = errno;
    return len;
  }

  Retrive(len);
  return len;
}

void MakeSpace(size_t len){
  if(WritableBytes()+PrependableBytes() < len){
    buffer_.resize(writePos_+len+1);
  }
  else{
    size_t readable = ReadableBytes();
    copy(BeginPtr()+readPos_, BeginPtr()+writePos_, BeginPtr());
    readPos_=0;
    writePos_= readable;
    assert(readable == ReadableBytes());
  }
}




















































































