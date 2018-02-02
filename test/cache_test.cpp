/**********************************************************
 * Copyright (C) 2017 All rights reserved.
 * \file cache_test.cpp
 * \brief
 * \note	注意事项：
 *
 *\version 1.0
 ** \author Allen.L
 ** \date 2017-08-22
 ** \last modified 2010-01-01 14:54:52
**********************************************************/

#include "base/base.h"
#include "base/define.h"
#include "cache_handle.h"

using namespace std;
using namespace base;
using namespace db;

int main( int argc, char **argv ) {

	CacheHandle::ptr_t cache_db;
	cache_db = CacheHandle::Create();
	if(cache_db->IsConnection()) {
		cout << "redis server is start" << endl;
	}

	Connection::ptr_t c = cache_db->Get();
	if (!c->IsValid()) {
		delete c;
		c = NULL;
		return 0;
	}

	uint64 ret = 0;
	Reply r = c->Run(Command("SET")("test")("123"));
	if (r.type_ == REDIS_REPLY_INTEGER) {
		ret = (uint64)r.integer_;
	}

	cache_db->Put(c);

///////////////////////////////////////////////////

	c = cache_db->Get();
	if (!c->IsValid()) {
		delete c;
		c = NULL;
		return 0;
	}

	string str;
	r = c->Run(Command("GET")("test"));
	if (r.type_ == REDIS_REPLY_STRING) {
		str = r.str_;
	}
	cache_db->Put(c);

	cout << "get test value : " << str << endl;

	return ret;
}
