/**********************************************************
 * Copyright (C) 2017 All rights reserved.
 * \file string_util.h
 * \brief
 * \note	注意事项：
 *
 *\version 1.0
 ** \author Allen.L
 ** \date 2017-07-06
 ** \last modified 2010-01-01 08:48:14
**********************************************************/
#ifndef _STRING_UTIL_H
#define _STRING_UTIL_H

#include "define.h"

namespace base {
namespace utils {

// 类名 : StringUtil
// 说明 : 字符串相关操作类
// 注意 : 涉及到标准c字符串与c++ string类的相关操作
//
// TODO :

class StringUtil {
 public:
  // 把string以delim分隔开,放到list中
  static void Split(char *str, const char *delim, std::vector<char*> &list);
  // 替换字符串中对应的子字符串
  static bool StrReplaceAll(std::string &str, const std::string &replace, const std::string &dest);
  // 通过分割符来分割字符串，并将分割好的数据存在对应的数据中
  // 当输入字符串为空时，结果会返回一个空值的字符串
  static bool SplitString(const std::string &str, const std::string &delim, std::vector<std::string> &result);
  static bool SplitString(const std::string &str, const std::string &delim, std::list<std::string> &result);
  // 合并字符串, 使用分隔符分隔
  static std::string MergeString(const std::vector<std::string> &vector, const std::string &delim);
  // 删除左边(右边)的空白符，制表符，换行符
  // 不处理中间空格，但会删除所有的制表符和换行符
  static std::string TrimRight(const std::string &str);
  static std::string TrimLeft(const std::string &str);
  static std::string Trim(const std::string &str);
  // 字符串大小写转换
  static char *StrToLower(char *str);
  static char *StrToUpper(char *str);
  // 类型操作
  static int32 IsInt(const char *str);
  static std::string IntToStr(const long &num);
  static int32 StrToInt(const char *str, int32 dest = 0);
  static bool CheckNumber(const std::string &num);
  // 格式化时间
  static void FormatTimeStr(char *str);

};

} //utils
} //base
#endif

