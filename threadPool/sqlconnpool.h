/*************************************************************************
    > File Name: sqlconnpool.h
    > Author: ma6174
    > Mail: ma6174@163.com 
    > Created Time: 2024年10月01日 星期二 21时33分14秒
 ************************************************************************/
#ifndef SQLCONNPOOL_H
#define SQLCONNPOOL_H

#include <mysql/mysql.h>
#include <string>
#include <queue>
#include <mutex>
#include <semaphore.h>
#include <thread>
#include "../log/log.h"

using namespace std;

class SqlConnPool{
public:
  static SqlConnPool* getInstance();
  MYSQL* GetConn();//获取一个可用的数据库连接。
  void FreeConn(MYSQL* conn);//释放一个数据库连接，将其返回到连接池中。
  int GetFreeConnCount();//获取当前连接池中可用连接的数量。

  void Init(const char* user,int port,const char* host, const char* pwd, const char* dbName,int connSize);
  void ClosePool();

private:
  SqlConnPool()=default;
  ~SqlConnPool(){ClosePool();}

  int MaxConn_;

  mutex mtx_;
  sem_t semId_;
  queue<MYSQL*>connQue_;
};

class SqlConnRAII{
public:
  SqlConnRAII(MYSQL** sql, SqlConnPool connPool){
    assert(connPool);
    *sql = connPool->GetConn();
    sql_=*sql;
    connPool_=connPool;
  }
  ~SqlConnPool(){
    if(sql_){sql_->FreeConn(sql_);}
  }
private:
  MYSQL* sql_
  SqlConnPool connPool_;

}


#endif
