/**********************************************************
 * Copyright (C) 2017 All rights reserved.
 * \file sql_handle.h
 * \brief
 * \note	注意事项：
 *
 *\version 1.0
 ** \author Allen.L
 ** \date 2017-08-11
 ** \last modified 2010-01-01 14:19:01
**********************************************************/
#ifndef _SQL_HANDLE_H
#define _SQL_HANDLE_H

#include "mysql/mysql.h"
#include "base/base.h"

// 数据库操作类的封装基于MySQL C API的封装实现
// 线程安全 但不保证多线程情况下任务的顺序性，提供读写锁保证任务顺序性顺序性
// 简单的封装了mysql数据接口，如需实现其他业务功能，参考源代码实现。
//
//

using namespace std;
namespace db {

// 类名 : MutexScopedLock
// 说明 : 数据库互斥量
// 注意 : pthread_mutex_t结构体
//
// 示例 :
// TODO :
//

class MutexScopedLock {
 public:
  MutexScopedLock(pthread_mutex_t* lock) {
	  m_lock = lock;
	  pthread_mutex_lock(m_lock);
  }

  ~MutexScopedLock() {
	  pthread_mutex_unlock(m_lock);
	  m_lock = NULL;
  }

 private:
  pthread_mutex_t* m_lock;

};

// 类名 : RWMutexScopedLock
// 说明 : 数据库读写锁
// 注意 :
//
// 示例 :
// TODO :
//

class RWMutexScopedLock {
 public:
  RWMutexScopedLock(pthread_rwlock_t* lock, bool write = true) {
	  m_lock = lock;
	  if (write) {
		  pthread_rwlock_wrlock(m_lock);
	  } else {
		  pthread_rwlock_rdlock(m_lock);
	  }
  }

  ~RWMutexScopedLock() {
	  pthread_rwlock_unlock(m_lock);
	  m_lock = NULL;
  }

  bool upgrade2writer() {
	  pthread_rwlock_unlock(m_lock);
	  pthread_rwlock_wrlock(m_lock);
	  return true;
  }

 private:
  pthread_rwlock_t* m_lock;

};

template <class T>
std::string ConvertToString(const T v) {
	std::stringstream ss;
	ss << v;
	return ss.str();
}

// 类名 : Command
// 说明 : SQL语句集合
// 注意 : 适用于执行批量sql语句处理、事务。支持多语句以";"号隔开的sql字符串初始化。或者单条sql插入,";"结尾不必需。
//
// 示例 :
// TODO :
//

class Command {
 public:
  Command(const std::string& cmd);

  template <typename T>
  Command &operator()(const T& cmd) {
	  args_.push_back(ConvertToString(cmd));
	  return (*this);
  }

  operator const std::vector<std::string> &() {
	  return args_;
  }
  // 单条数据末尾插入，语句末尾不需要";"号
  void PushCommand(const std::string& cmd);
  // 获取命令集合
  std::vector<std::string> GetCommand() const;
  // 获取命令集合字符串,";"号隔开
  std::string GetCommandString();

  int32 GetCommandSize();
  bool ClearCommand();

 private:
  std::string str_;
  std::vector<std::string> args_;

};


// 类名 : ConnectionPool
// 说明 : 数据库连接池
// 注意 :
//
// 示例 :
// TODO :
//

class ConnectionPool : base::Logging {
 public:
  ConnectionPool();
  ~ConnectionPool();

  bool InitPool();
  MYSQL* GetConnection();
  void DestoryPool();
  void Close(MYSQL *conn);
  int32 GetUsedNum();
  int32 GetPoolNum();

 private:
  friend class SqlHandle;

  MYSQL *CreateConnection();
  bool IsClose(MYSQL *conn);
  bool DestoryConnection(MYSQL *conn);


  deque< pair<MYSQL *, long> > pool_;
  pthread_mutex_t pool_mutex_;

  string host_;
  string user_;
  string pwd_;
  string db_;
  uint32 port_;
  string unix_socket_;
  unsigned long client_flag_;
  int32 max_size_;
  int32 min_size_;
  int32 timeout_;
  int32 used_;
};

// 类名 : ConnectionPool
// 说明 : 数据库连接池
// 注意 :
//
// 示例 :
// TODO : 1.sql语句组装优化 2.sql旧结果集处理优化 3.sql结果集数据获取封装
//

class SqlHandle : public base::Singleton<SqlHandle>, base::Logging {
 public:
  SqlHandle();
  ~SqlHandle();
  bool CreatePool();
  // 数据库参数，采用配置文件初识化
  void Config(map<string, string>& parameters);
  // 数据库参数，采用参数初始化
  bool InitDB(string host, string user, string pwd, string db,
		  uint32 port, string unix_socket, unsigned long clinet_flag);

  // 查询接口，成功返回0，失败返回-1
  int32 Select(const char* sql, string &result);
  // 插入接口，单条插入，返回受影响行数，失败返回-1
  int32 Insert(const char* sql);
  // 更新接口，单条更新，返回受影响行数，失败返回-1
  int32 Update(const char* sql);
  // 插入接口，单条删除，返回受影响行数，失败返回-1
  int32 Delete(const char* sql);
  // 批量操作接口，返回执行成功条数，失败返回-1。建议批量插入、更新、删除等批处理
  int32 ExecuteBulk(const std::vector<std::string>& command);
  // 事务操作接口，采用隐式事务，成功受影响行数，失败返回-1
  int32 Transaction(const std::vector<std::string>& command);

  inline ConnectionPool& GetConnectionPool() {
	  return connection_pool_;
  }

 private:
  ConnectionPool connection_pool_;
  pthread_rwlock_t rwlock_mutex_;

  void DestoryPool();
  void ClearResult(MYSQL_RES *res);
  uint32 GetNumRows(MYSQL_RES *res);
  uint32 GetNumFields(MYSQL_RES *res);
  unsigned long *GetFetchLengths(MYSQL_RES *res);
  MYSQL_FIELD * GetFetchFields(MYSQL_RES *res);
  int32 NoRawExecute(const char* sql);
  int32 Execute(const char* sql, string &result);

};

} //db

#endif

