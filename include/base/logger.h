/**********************************************************
 * Copyright (C) 2017 All rights reserved.
 * \file logger.h
 * \brief
 * \note	注意事项：
 *
 *\version 1.0
 ** \author Allen.L
 ** \date 2017-07-06
 ** \last modified 2010-01-01 08:47:31
**********************************************************/
#ifndef _LOGGER_H
#define _LOGGER_H

#include <stdarg.h>
#include <sys/syscall.h>
#include <mutex>

#include "define.h"
#include "singleton.h"

#define MAX_FRAGMENT 102400
#define FRAGMENT_SIZE 2048
#define QUEUE_MAXLEN 102401
#define gettid() syscall(__NR_gettid)

namespace base {

enum {
	LOG_LEVEL_ERROR = 0, LOG_LEVEL_WARNING = 1, LOG_LEVEL_INFO = 2, LOG_LEVEL_DEBUG = 3
};

enum {
	EMPTY = 0, WRITING = 1, FILLED = 2, OUTPUT = 3
};

static const char LOG_LEVEL_TAG[][10] = { "error", "warning", "info", "debug" };

typedef struct {
	long req_ltime;
	int32 cache_no;
	int32 status;
} CacheFragment;

#define BASE_LOGGER_MANAGER base::LoggerManager::Instance()
#define BASE_LOGGER_INIT(fname, level, rotate) BASE_LOGGER_MANAGER.Init(fname, level, rotate)


// 类名 : LoggerManager
// 说明 : 日志管理类
// 注意 :
//
// TODO :
//

class LoggerManager : public Singleton<LoggerManager> {
 public:
  LoggerManager();
  ~LoggerManager();

  inline int32 GetLogLevel() { return level_; }

  // 请求碎片,FILLED状态为已用碎片。首次请求，缺省为NULL
  int32 RequestFragment(int32 filled_no = -1);
  // 释放碎片，单线程操作
  int32 ReleaseFragment(int32 empty_no = -1);
  // 交还碎片
  void OverFragment(int32 fillen_no = -1);
  // 返回碎片位置
  char *LocateFragment(int32 frag_no);
  // 初始化日志管理
  void Init(const std::string file_name, const std::string level, bool rotate = true);
  // 创建日志文件
  void CreateLog(uint32 msecond = 0);
  // 启动日志输出线程
  bool CreateWriter();
  // 等待日志输出线程退出
  void StopWriter();
  // 日志输出函数
  static void *WritingLog(void *arg);

 private:
  static char cache_[MAX_FRAGMENT][FRAGMENT_SIZE];
  static CacheFragment queue_[QUEUE_MAXLEN];
  static char log_name_[MAX_LENGTH_128];
  static char date_[MAX_LENGTH_16];
  static int32 empty_head_;
  static int32 empty_tail_;
  static int32 filled_tail_;
  static FILE *log_file_;
  static bool is_run_;
  static bool auto_rotate_;

  int32 level_;
  pthread_t log_writer_;
  std::mutex mutex_;
  void pprint(std::string op);
};

#define BASE_GPLOGGER base::Logger::Instance()
#define LOG_ERROR(_fmt_, args...)	BASE_GPLOGGER.Error(_fmt_, ##args)
#define LOG_DEBUG(_fmt_, args...)	BASE_GPLOGGER.Debug(_fmt_, ##args)
#define LOG_INFO(_fmt_, args...)	BASE_GPLOGGER.Info(_fmt_, ##args)
#define LOG_WARNING(_fmt_, args...) BASE_GPLOGGER.Warning(_fmt_, ##args)


// 类名 : Logger
// 说明 : 日志输出类
// 注意 :
//
// TODO :
//

class Logger : public Singleton<Logger> {
 public:
  Logger(bool delay_ = false);
  ~Logger();

  inline void SetLogLevel(int32 level) { level_ = level; }

  // 日志级别输出
  void Debug(const char *format, ...);
  void Info(const char *format, ...);
  void Warning(const char *format, ...);
  void Error(const char *format, ...);
  // 日志输出函数
  void LogPrint(int32 level, const char *format, va_list args);
  int32 my_vsnprintf(char *str, size_t size, const char *format, va_list ap);

 private:
  int32 frag_no_;
  int32 level_;
  char *pbuf_;
  char *sbuf_;
  bool delay_;
  pid_t pid_;
  std::mutex mutex_;
};

// 类名 : Logging
// 说明 : 日志基类
// 注意 :
//
// TODO :
//

class Logging {
 public:
  Logging() {
	  logger_ = new Logger(false);
  }

  ~Logging() {
	  if (logger_) {
		  delete logger_;
	  }
  }
 protected:
  Logger *logger_;

};

} //base

#endif

