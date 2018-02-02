/**********************************************************
 * Copyright (C) 2017 All rights reserved.
 * \file singleton.h
 * \brief
 * \note	注意事项：
 *
 *\version 1.0
 ** \author Allen.L
 ** \date 2017-07-05
 ** \last modified 2017-08-07 16:57:37
**********************************************************/
#ifndef _SINGLETON_H
#define _SINGLETON_H

#include <mutex>
#include <thread>

namespace base {

// 类名 : Singleton
// 说明 : 单例模板类
// 注意 : 1.只支持c++11标准以上(gcc 4.7以上)
//		  2.采用c++11标准静态初始化器获取线程安全单例
//		  3.但是，该方法仅适用于完全符合C ++ 11部分的编译器
//		  4.使用前请注意在多线程环境下测试
//
// 示例 : 1.class Test : public Singleton<Test>   调用 : Test::instance()
//		  2.typedef Singleton<Test> SingletonTest 调用 : SingletonTest::instance()
// TODO :
//

template<typename T, bool defaulted = true>
class Singleton {
 public:
  static T& Instance() {
	  static T static_instance_;
	  return static_instance_;
  }

  Singleton<T>& operator=(const Singleton<T>&) = delete;
  Singleton(const Singleton<T>&) = delete;

 protected:
  Singleton(){};
  virtual ~Singleton(){};
};

template<typename T>
class Singleton<T, false> {
 public:
  template<class... Args>
  static T& Instance(Args&&... args) {
	  std::call_once( static_inited_once_flag_, [&](){
			  static_ptr_instance_ = std::unique_ptr<T>( new T(std::forward<Args>(args)... ) );
			  });
	  return *static_ptr_instance_;
  }
  static T& Instance() {
	  if (nullptr == static_ptr_instance_) {
		  throw std::logic_error(std::string() + typeid(T).name() + " is not inited!\n");
	  }
	  return *static_ptr_instance_;
  }
  static void Release() {
	  if (nullptr != static_ptr_instance_) {
		  delete static_ptr_instance_;
		  static_ptr_instance_ = nullptr;
	  }
  }

  Singleton<T>& operator=(const Singleton<T>&) = delete;
  Singleton(const Singleton<T>&) = delete;

 protected:
  Singleton(){};
  virtual ~Singleton(){};

 private:
  static std::unique_ptr<T>			static_ptr_instance_;
  static std::once_flag				static_inited_once_flag_;

};

template<class T>
std::unique_ptr<T>	Singleton<T, false>::static_ptr_instance_;

template<class T>
std::once_flag	Singleton<T, false>::static_inited_once_flag_;

} //base

#endif

