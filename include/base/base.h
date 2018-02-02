/**********************************************************
 * Copyright (C) 2017 All rights reserved.
 * \file base.h
 * \brief
 * \note	注意事项：
 *
 *\version 1.0
 ** \author Allen.L
 ** \date 2017-07-18
 ** \last modified 2010-01-01 08:45:29
**********************************************************/
#ifndef _BASE_H
#define _BASE_H

#include <errno.h>

namespace base {

class Logging;
class LoggerManger;
class Logger;
class Config;
class StringUtil;
class Encrypt;
class Exception;

class Noncopyable {
 protected:
  Noncopyable() {}
  ~Noncopyable() {}
 private:
  Noncopyable(const Noncopyable&);
  const Noncopyable& operator=(const Noncopyable&);
};

inline int GetSystemErrno() {
	return errno;
}

} //base

#include "define.h"
#include "config.h"
#include "logger.h"
#include "encrypt.h"
#include "singleton.h"
#include "string_util.h"
#include "json_util.h"

#endif

