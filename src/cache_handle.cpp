/**********************************************************
 * Copyright (C) 2017 All rights reserved.
 * \file cache_handle.cpp
 * \brief
 * \note	注意事项：
 *
 *\version 1.0
 ** \author Allen.L
 ** \date 2017-08-04
 ** \last modified 2017-08-22 18:01:07
**********************************************************/

#include "cache_handle.h"

namespace db {

Connection::Connection(const std::string &host, const uint32 port, uint32 timeout) {
	struct timeval tv;
	tv.tv_sec = timeout / 1000;
	tv.tv_usec = (timeout % 1000) * 1000;

	context_ = redisConnectWithTimeout(host.c_str(), port, tv);
	if (context_->err != REDIS_OK) {
		logger_->Warning("cachedb connection fail![host=%s, port=%d, error=%s]", host.c_str(), (int) port, context_->errstr);
		redisFree(context_);
		context_ = NULL;
	}
}

Connection::~Connection() {
	if (context_) {
		redisFree(context_);
		context_=NULL;
	}
}

bool Connection::Append(const std::vector<std::string>& command) {
	if (!IsValid()) {
		return false;
	}
	std::vector<const char*> argv;
	argv.reserve(command.size());
	std::vector<size_t> argvlen;
	argvlen.reserve(command.size());

	for (std::vector<std::string>::const_iterator it = command.begin(); it != command.end(); it++) {
		argv.push_back(it->c_str());
		argvlen.push_back(it->size());
	}

	int ret = redisAppendCommandArgv(context_, static_cast<int>(command.size()), argv.data(), argvlen.data());

	return ret == REDIS_OK;
}

Reply Connection::GetReply() {
	redisReply* r = NULL;
	int ret = redisGetReply(context_, reinterpret_cast<void**>(&r));
	if (ret != REDIS_OK)
		logger_->Warning("get reply for cachedb fail![error=%s, ret=%d]", context_->errstr, ret);

	Reply reply(r);
	freeReplyObject(r);
	r = NULL;

	return reply;
}

Connection::ptr_t Connection::Create(const std::string &host, const uint32 port, uint32 timeout) {
	return new Connection(host, port);
}

Reply Connection::Run(const std::vector<std::string>& command) {
	return Append(command) ? GetReply() : Reply(NULL);
}

bool Connection::IsValid() {
		return context_ && context_->err == REDIS_OK;
}

CacheHandle::CacheHandle(const std::string &host, uint32 port, uint32 timeout) {
	host_ = host;
	port_ = port;
	timeout_ = timeout;
	db_ = 0;
}

CacheHandle::~CacheHandle() {
	ClearConnection();
}

void CacheHandle::ClearConnection() {
	std::unique_lock<std::mutex> lock(pool_mutext);
	for (std::deque<Connection::ptr_t>::iterator it = conn_pool_.begin(); it != conn_pool_.end(); it++) {
		delete (*it);
	}
	conn_pool_.clear();
}

CacheHandle::ptr_t CacheHandle::Create(const std::string &host, uint32 port, uint32 timeout) {
	return CacheHandle::ptr_t(new CacheHandle(host, port, timeout));
}

Connection::ptr_t CacheHandle::Get() {
	Connection::ptr_t conn = NULL;
	{
		std::unique_lock<std::mutex> lock(pool_mutext);
		if (!conn_pool_.empty()) {
			conn = conn_pool_.front();
			conn_pool_.pop_front();
		}
	}

	if (!conn) {
		conn = Connection::Create(host_, port_);
	}

	return conn;
}

void CacheHandle::Put(Connection::ptr_t conn) {
	if (conn->IsValid()) {
		std::unique_lock<std::mutex> lock(pool_mutext);
		conn_pool_.push_back(conn);
	} else {
		delete conn;
		conn = NULL;
	}
}

void CacheHandle::SetDB(const uint32 value) {
	db_ = value;
}

bool CacheHandle::IsConnection() {
	bool ret = false;
	Connection::ptr_t conn = Get();
	if (conn->IsValid()) {
		Reply r = conn->Run(Command("PING"));
		if (r.type_ == REDIS_REPLY_STATUS && r.str_ == "PONG")
			ret = true;
	}
	Put(conn);

	return ret;
}

int32 CacheHandle::Flush() {
	bool ret = -1;
	Connection::ptr_t conn = Get();
	if (conn->IsValid()) {
		Reply r = conn->Run(Command("FLUSHDB"));
		if (r.type_ == REDIS_REPLY_STATUS && r.str_ == "OK") {
			ret = 0;
		}
	}
	Put(conn);

	return ret;
}

} //db
