/**********************************************************
 * Copyright (C) 2017 All rights reserved.
 * \file define.h
 * \brief
 * \note	注意事项：
 *
 *\version 1.0
 ** \author Allen.L
 ** \date 2017-06-30
 ** \last modified 2017-07-19 16:04:02
**********************************************************/
#ifndef _DEFINE_H
#define _DEFINE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <errno.h>
#include <stdbool.h>

#include <cstddef>
#include <iostream>
#include <memory>
#include <list>
#include <map>
#include <set>
#include <vector>
#include <deque>
#include <queue>
#include <fstream>
#include <sstream>

#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <pthread.h>

#ifndef UNUSED
#define UNUSED(v) ((void)(v))
#endif

#ifndef NULL
#define NULL	0
#endif

#ifndef WIN32
#define __stdcall
#endif


typedef signed char				int8;
typedef signed short			int16;
typedef signed int				int32;
typedef signed long long		int64;

typedef unsigned char			uint8;
typedef unsigned short			uint16;
typedef unsigned int			uint32;
typedef unsigned long long		uint64;

#define SAFE_DELETE(_point_)   if(_point_ != NULL) { delete _point_; _point_ = NULL; }
#define SAFE_DELETE_NOT_NULL(_point_)   if(_point_ != NULL) { delete _point_; }
#define SAFE_DELETE_ARRAY(_point_array_) if(_point_array_ != NULL) {delete [] _point_array_; _point_array_ = NULL; }
#define SAFE_DELETE_ARRAY_NOT_NULL(_point_array_) if(_point_array_ != NULL) {delete [] _point_array_; }

#define MAX_LENGTH_1024		1024
#define MAX_LENGTH_512		512
#define MAX_LENGTH_128		128
#define MAX_LENGTH_64		64
#define MAX_LENGTH_16		16

#endif

