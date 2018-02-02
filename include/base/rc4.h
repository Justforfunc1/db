/**********************************************************
 * Copyright (C) 2017 All rights reserved.
 * \file rc4.h
 * \brief
 * \note	注意事项：1.定义一个RC4_KEY结构 2.用函数RC4Init初始化状态 3.用RC4Works加密
 *
 *\version 1.0
 ** \author Allen.L
 ** \date 2017-07-03
 ** \last modified 2017-07-19 15:53:04
**********************************************************/
#ifndef _RC4_H
#define _RC4_H

#include <cstring>
#include <iostream>

typedef unsigned char Byte;
typedef unsigned int uint32;
#ifdef __cplusplus
extern "C" {
#endif

// 定义密钥变量 RC4_KEY rc4key
struct RC4_KEY{
	Byte bySTab[256];   //256字节的S表
	Byte byIt, byJt;    //t时刻的两个指针
};

// 初始化密钥 RC4Init(255字节以内的口令字符串,口令长度,&rc4key)
bool RC4Init(const char *, uint32, RC4_KEY *);

// 加/解密 RC4Works(无符号字符型数据,数据长度,&rc4key)
bool RC4Works (Byte *content, uint32 size, RC4_KEY *);

#ifdef __cplusplus
}
#endif

#endif

