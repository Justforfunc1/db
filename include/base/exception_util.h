/**********************************************************
 * Copyright (C) 2017 All rights reserved.
 * \file exception_util.h
 * \brief
 * \note	注意事项：
 *
 *\version 1.0
 ** \author Allen.L
 ** \date 2017-07-06
 ** \last modified 2010-01-01 08:47:00
**********************************************************/
#ifndef _EXCEPTION_UTIL_H
#define _EXCEPTION_UTIL_H

#include <exception>
#include <ostream>
#include "define.h"

namespace base {

// 类名 : Exception
// 说明 : 异常处理类
// 注意 : 该类是对std::exception的封装
//
// TODO :
//

class Exception : public std::exception {
 public:
  Exception();
  Exception(const char *, int32);
  Exception(const char *, int32, const char *);
  virtual ~Exception() throw();
  virtual std::string name() const;
  virtual void print(std::ostream &) const;
  virtual const char *what() const throw();
  virtual Exception *clone() const;
  virtual void _throw() const;
  const char *file() const;
  int32 line() const;
 private:
  const char *file_;
  int32 line_;
  const char *desc_;
  static const char *name_;
  mutable std::string str_;
};

std::ostream& operator <<(std::ostream& out, const Exception& ex);

// 类名 : NullHandleException
// 说明 : 空异常处理类
// 注意 :
//
// TODO :
//

class NullHandleException : public Exception {
 public:
  NullHandleException(const char *, int32);
  virtual ~NullHandleException() throw();
  virtual std::string name() const;
  virtual Exception *clone() const;
  virtual void _throw() const;

 private:
  static const char *name_;
};

// 类名 : IllegalArgumentException
// 说明 : 不合法参数异常类
// 注意 :
//
// TODO :
//

class IllegalArgumentException : public Exception {
 public:
  IllegalArgumentException(const char *, int32);
  IllegalArgumentException(const char *, int32, const std::string &);
  virtual ~IllegalArgumentException() throw();
  virtual std::string name() const;
  virtual void print(std::ostream &) const;
  virtual Exception *clone() const;
  virtual void _throw() const;
  std::string reason() const;

 private:
  static const char *name_;
  std::string reason_;
};

// 类名 : SyscallException
// 说明 : 系统调用异常类
// 注意 :
//
// TODO :
//

class SyscallException : public Exception {
 public:
  SyscallException(const char *, int32);
  SyscallException(const char *, int32, int32);
  virtual std::string name() const;
  virtual void print(std::ostream &) const;
  virtual Exception *clone() const;
  virtual void _throw() const;
  int32 error();

  int error_;
  static const char *name_;
};

} //base

#endif

