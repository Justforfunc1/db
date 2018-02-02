/**********************************************************
 * Copyright (C) 2017 All rights reserved.
 * \file cache_handle.h
 * \brief
 * \note	注意事项：
 *
 *\version 1.0
 ** \author Allen.L
 ** \date 2017-08-04
 ** \last modified 2010-01-01 14:19:24
**********************************************************/
#ifndef _CACHE_HANDLE_H
#define _CACHE_HANDLE_H

#include <iostream>
#include <vector>
#include <memory>
#include <thread>
#include <mutex>

#include "hiredis/hiredis.h"
#include "base/base.h"


namespace db {


// 基于hiredis的缓存接口封装
// 经过项目测试接口功能正常
// 暂未发现异常
//
//
//

template <class T>
std::string ConvertToString(const T v) {
	std::stringstream ss;
	ss << v;
	return ss.str();
}

// 类名 : Command
// 说明 : 命令集合封装
// 注意 :
//
// 示例 :
// TODO :
//

class Command {
 public:
  Command(const std::string& cmd) {
	  args_.push_back(cmd);
  }

  template <typename T>
  Command &operator()(const T& cmd) {
	  args_.push_back(ConvertToString(cmd));
	  return (*this);
  }

  operator const std::vector<std::string> &() {
	  return args_;
  }

 private:
  std::vector<std::string> args_;
};

// 类名 : Reply
// 说明 : redis返回包处理类
// 注意 :
//
// 示例 :
// TODO :
//

typedef struct Reply {
  int32 type_;
  long long integer_;
  std::string str_;
  std::vector<Reply> elements_;

  Reply(const redisReply *reply)
    : type_(REDIS_REPLY_ERROR), integer_(0), str_("") {
		if (reply != NULL) {
			type_ = reply->type;
			switch (type_) {
				case REDIS_REPLY_STRING:
				case REDIS_REPLY_STATUS:
				case REDIS_REPLY_ERROR:
					str_ = std::string(reply->str);
					break;
				case REDIS_REPLY_INTEGER:
					integer_ = reply->integer;
					break;
				case REDIS_REPLY_ARRAY:
					for (size_t i = 0; i < reply->elements; i++) {
						elements_.push_back(reply->element[i]);
					}
					break;
				default:
					break;
			}
		}
	}
} Reply;

// 类名 : Connection
// 说明 : redis连接管路类
// 注意 :
//
// 示例 :
// TODO :
//

class Connection : base::Noncopyable, base::Logging {
 public:
  typedef Connection *ptr_t;
  ~Connection();
  static ptr_t Create(const std::string &host = "127.0.0.1", const uint32 port = 6379, uint32 timeout = 100);
  Reply Run(const std::vector<std::string> &command);
  bool IsValid();

 private:
  Connection(const std::string &host, const uint32 port, uint32 timeout = 100);
  bool Append(const std::vector<std::string>& command);
  Reply GetReply();

  redisContext *context_;
  friend class CacheHandle;
};


// 类名 : CacheHandle
// 说明 : 缓存操作类
// 注意 :
//
// 示例 :
// TODO :
//

class CacheHandle {
 public:
  typedef std::unique_ptr<CacheHandle> ptr_t;
  ~CacheHandle();
  static ptr_t Create(const std::string &host = "127.0.0.1", uint32 port = 6379, uint32 timeout = 100);
  Connection::ptr_t Get();
  void Put(Connection::ptr_t conn);
  void SetDB(const uint32 value);
  bool IsConnection();
  int32 Flush();

 private:
  CacheHandle(const std::string &host, uint32 port, uint32 timeout);
  void ClearConnection();

  std::string host_;
  uint32 port_;
  uint32 timeout_;
  uint32 db_;
  std::deque<Connection::ptr_t> conn_pool_;
  std::mutex pool_mutext;

};

} //db

#endif

