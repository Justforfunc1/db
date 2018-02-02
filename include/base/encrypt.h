/**********************************************************
 * Copyright (C) 2017 All rights reserved.
 * \file encrypt.h
 * \brief
 * \note	注意事项：
 *
 *\version 1.0
 ** \author Allen.L
 ** \date 2017-07-03
 ** \last modified 2010-01-01 08:46:37
**********************************************************/
#ifndef _ENCRYPT_H
#define _ENCRYPT_H

#include "define.h"
#include "md5.h"
#include "rc4.h"

namespace base {
namespace utils {

// 类名 : Encrypt
// 说明 : 数据常用的加密处理类
// TODO :
//

class Encrypt {
 public:
  // 生成MD5，默认32位
  std::string Get32BitMd5(const std::string str);
  // 生成MD5，截取16位
  std::string Get16BitMd5(const std::string str);
  // RC4加密
  bool Rc4Encrypt(unsigned char *content, uint32 size, const std::string &skey);

};

} //utils
} //base

#endif

