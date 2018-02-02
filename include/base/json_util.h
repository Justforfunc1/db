/**********************************************************
 * Copyright (C) 2017 All rights reserved.
 * \file json_util.h
 * \brief
 * \note	注意事项：
 *
 *\version 1.0
 ** \author Allen.L
 ** \date 2017-07-06
 ** \last modified 2010-01-01 08:47:12
**********************************************************/
#ifndef _JSON_UTIL_H
#define _JSON_UTIL_H

#include <algorithm>

#include "define.h"
#include "json/json.h"

namespace base {
namespace utils {

// 类名 : JsonTool
// 说明 : 封装JSONCPP。增加复杂的操作功能 以及增加前置判断来减少JSONCPP的崩溃行为
//		  Get开头的函数为值传递，Find或其他开头的为引用/指针传递
// TODO :
//

class JsonUtil {
 public:
  JsonUtil();
//  explicit JsonUtil(const JsonUtil &);
  ~JsonUtil();

  // JSON字符串解析函数, 讲JSON字符串解析为Json::Value的类对象
  // 已知问题：111{sdsa}ssds解析结果为true，jsonvalue中保存111
  bool ParseJson(const std::string &str, Json::Value &value);
  // 通过路径获取对应的JSON对象
  Json::Value GetJsonValue(const Json::Value &js_value, const std::string &path);
  // 通过key-value来索引JSON对象。此函数可在数组中查找所需要的JSON对象并返回
  Json::Value GetJsonValue(const Json::Value &js_value, const std::string &key, const std::string &value);
  // 获取JSON对象指针，操作指针会影响对应对象
  Json::Value* FindJsonValue(Json::Value &js_value, const std::string &path);
  Json::Value* FindJsonValue(Json::Value &js_array, const std::string &key, const std::string &value);
  // 转换JSON对象到map对象
  // map_result 保存JSON数据的map对象
  // key_list 根据key的列表来转换对应的key。默认为空时，表示所有的key都将被转换
  bool ConvertJsonToMap(const Json::Value &js_value, std::map<std::string,std::string> &map_result, std::string key_list = "");
  // map转换为JSON对象字符串
  std::string ConvertMapToJsonStr(const std::map<std::string,std::string> map);

 private:
  std::string index_split_path_default;
  std::string index_split_key_default;

  // 解析key的索引路径
  bool ParseKeyPath(const std::string &path, std::vector<std::string> &keys);
  // 解析key值的列表。并保存到key的数组中
  bool ParseKeyList(const std::string &key_list, std::vector<std::string> &keys);
  // 通过key找到对象的JSON对象
  Json::Value* FindJsonValueByKey(Json::Value &js_value, std::string &key);

};

} //utils
} //base

#endif

