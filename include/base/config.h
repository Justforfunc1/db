/**********************************************************
 * Copyright (C) 2017 All rights reserved.
 * \file config.h
 * \brief
 * \note	注意事项：
 *
 *\version 1.0
 ** \author Allen.L
 ** \date 2017-07-06
 ** \last modified 2010-01-01 08:45:43
**********************************************************/
#ifndef _CONFIG_H
#define _CONFIG_H

#ifdef __GNUC__
#include <ext/hash_map>
#define  HASH_NAME    __gnu_cxx
#else
#include <hash_map>
#define  HASH_NAME    std
#endif

#include "define.h"

namespace base {

#define BASE_CONFIG base::Config::GetConfig()
#define CONFIG_LOAD(filename) BASE_CONFIG.Load(filename)
#define CONFIG_GETSTR(section, key, d) BASE_CONFIG.GetString(section, key, d)
#define CONFIG_GETINT(section, key, d) BASE_CONFIG.GetInt(section, key, d)

struct StrHash {
	size_t operator()(const std::string& str) const {
		return HASH_NAME::__stl_hash_string(str.c_str());
	}
};

// 字符串比较
struct StrCmp {
	bool operator()(const char *s1, const char *s2) const {
		return strcmp(s1, s2) == 0;
	}
};

typedef HASH_NAME::hash_map<std::string, std::string, StrHash>		STR_STR_MAP;
typedef STR_STR_MAP::iterator		STR_STR_MAP_ITER;
typedef HASH_NAME::hash_map<std::string, STR_STR_MAP*, StrHash>		STR_MAP;
typedef STR_MAP::iterator		STR_MAP_ITER;

// 类名 : Config
// 说明 : 读取配置文件的类
// 注意 : 解析配置文件,并将配置项以key-value的形式存储到内存中
//		  目前只能解析特定格式的配置文件
// 示例 : 以[]为节点 "=" 左右为键值的配置文件格式
// TODO :
//

class Config {
 public:
  Config();
  ~Config();
  Config(const Config&) = delete;
  Config &operator =(const Config&) = delete;

  // 加载文件
  int32 Load(const char *filename);
  // 取一个字符串
  const char *GetString(const char *section, const std::string &key, const char *d = NULL);
  // 取一string列表
  std::vector<const char*> GetStringList(const char *section, const std::string &key);
  // 取一个整形
  int32 GetInt(char const *section, const std::string &key, int d = 0);
  // 取一个整形列表
  std::vector<int> GetIntList(const char *section, const std::string &key);
  // 取一个section下所有的key
  int32 GetSectionKey(const char *section, std::vector<std::string> &keys);
  // 获取所有section
  int32 GetSectionName(std::vector<std::string> &sections);
  // 获取配置文件字符串
  std::string ConfigToString();
  // 获取静态实例
  static Config& GetConfig();

 private:
  // 解析字符串
  int32 ParseValue(char *str, char *key, char *val);
  // section判断
  char *IsSectionName(char *str);
  //双层map
  STR_MAP config_map_;
};

} //base

#endif

