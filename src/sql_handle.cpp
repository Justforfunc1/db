/**********************************************************
 * Copyright (C) 2017 All rights reserved.
 * \file handle.cpp
 * \brief
 * \note	注意事项：
 *
 *\version 1.0
 ** \author Allen.L
 ** \date 2017-08-11
 ** \last modified 2017-08-25 16:46:20
**********************************************************/

#include "sql_handle.h"

namespace db {

#define MAX_WAITS		5
#define OPERATE_READ	0
#define OPERATR_WRITE	1
typedef struct timeval time_cnt;

Command::Command(const std::string& cmd) {
	base::utils::StringUtil::SplitString(cmd, std::string(";"), args_);
}

void Command::PushCommand(const std::string& cmd) {
	args_.push_back(cmd);
}

std::string Command::GetCommandString() {
	if (args_.empty() || args_.size() == 1) {
		return args_.front();
	}
	str_ = base::utils::StringUtil::MergeString(args_, string(";"));
	return str_;
}

std::vector<std::string> Command::GetCommand() const {
	return args_;
}

int32 Command::GetCommandSize() {
	  if (args_.empty()) {
		  return 0;
	  }
	  return args_.size();
}

bool Command::ClearCommand() {
	if (args_.empty()) {
		return true;
	}
	args_.clear();
	return true;
}

/////////////////////////////////////////////////////////////////

ConnectionPool::ConnectionPool() : unix_socket_("NULL"){
	pthread_mutex_init(&pool_mutex_, NULL);
	used_ = 0;
	port_ = 0;
	client_flag_ = 0;
	max_size_ = 5;
	min_size_ = 2;
	timeout_ = 600;
	used_ = 0;
}

ConnectionPool::~ConnectionPool() {
	DestoryPool();
	pthread_mutex_destroy(&pool_mutex_);
}

MYSQL *ConnectionPool::CreateConnection() {
	MYSQL *mysql = new MYSQL();
	MYSQL *conn = NULL;
	char flag = 1;
	try {
		conn = mysql_init(mysql);
		conn = mysql_real_connect(conn, host_.c_str(), user_.c_str(), pwd_.c_str(),
				db_.c_str(), port_, unix_socket_.c_str(), client_flag_);

		mysql_options(conn, MYSQL_OPT_RECONNECT, &flag);
		mysql_set_character_set(conn, "utf8");

	} catch (std::exception &e) {
		logger_->Warning("MySQL ERROR : %s", e.what());
	} catch (...) {
		logger_->Warning("CreateConnection Erroor!");
	}
	return conn;
}

bool ConnectionPool::DestoryConnection(MYSQL *conn) {
	bool ret = false;
	try {
		mysql_close(conn);
		ret = true;
	} catch (std::exception &e) {
		logger_->Warning("MySQL ERROR : %s ", e.what());
	} catch (...) {
		logger_->Warning("Close Connection Erroor!");
	}
	delete conn;
	conn = NULL;
	return ret;
}

bool ConnectionPool::InitPool() {
	DestoryPool();
	bool ret = false;
	{
		MutexScopedLock lock(&pool_mutex_);
		while ((int) (pool_.size()) < min_size_) {
			MYSQL *conn = CreateConnection();
			if (conn == NULL) {
				break;
			}
			time_cnt curTime;
			gettimeofday(&curTime, NULL);
			pool_.push_back(pair<MYSQL *, long>(conn, curTime.tv_sec));
		}
		if ((int) (pool_.size()) == min_size_) {
			ret = true;
		}
	}
	return ret;
}

void ConnectionPool::DestoryPool() {
	MutexScopedLock lock(&pool_mutex_);
	while (!pool_.empty()) {
		pair<MYSQL *, long> item = pool_.front();
		DestoryConnection(item.first);
		pool_.pop_front();
	}
	used_ = 0;
}

bool ConnectionPool::IsClose(MYSQL *conn) {
	int32 ret = 0;
	try {
			ret = mysql_ping(conn);
			if (ret != 0) {
				logger_->Debug("MYSQL mysql_ping fail! error=[%s]", mysql_error(conn));
				return false;
			}
		} catch (std::exception &e) {
			logger_->Warning("MySQL ERROR : %s ", e.what());
		} catch (...) {
			logger_->Warning("Close Connection Erroor!");
		}

	return true;
}

MYSQL *ConnectionPool::GetConnection() {
	int wait_cnt = 0;
	bool isWait = false;

L100: {
		MutexScopedLock lock(&pool_mutex_);
		if (pool_.empty() && used_ >= max_size_ && wait_cnt < MAX_WAITS) { //无可用连接
			isWait = true;
		} else {
			isWait = false;
		}
	}
	if (isWait) {
		sleep(1);
		wait_cnt++;
		goto L100;
	}

	MYSQL *conn = NULL;
	{
		MutexScopedLock lock(&pool_mutex_);
		if (pool_.empty()) {
			if (used_ < max_size_) { //新建
				conn = CreateConnection();
			}
		} else { //池中获取
			pair<MYSQL *, long> item = pool_.front();
			pool_.pop_front();
			conn = item.first;

			// 需要判断该连接是否已经断开
			if (IsClose(conn)) {
				delete conn;
				conn = NULL;
				conn = CreateConnection();
			}
		}
		if (conn != NULL) {
			used_++;
		}
	}
	return conn;
}

void ConnectionPool::Close(MYSQL * conn) {
	time_cnt curTime;
	gettimeofday(&curTime, NULL);
	{
		MutexScopedLock lock(&pool_mutex_);
		//放回池中
		time_cnt curTime;
		gettimeofday(&curTime, NULL);
		pool_.push_back(pair<MYSQL *, long>(conn, curTime.tv_sec));
		used_--;

		//检查是否需要释放的
		while ((int) (pool_.size()) > min_size_) {
			pair<MYSQL *, long> item = pool_.front();
			if ((curTime.tv_sec - item.second) <= timeout_) { //未达空闲时间
				break;
			}
			DestoryConnection(item.first);
			pool_.pop_front();
		}
	}
}

int ConnectionPool::GetUsedNum() {
	return used_;
}

int ConnectionPool::GetPoolNum() {
	int size = 0;
	{
		MutexScopedLock lock(&pool_mutex_);
		size = pool_.size();
	}
	return size;
}

//////////////////////////////////////////////////////////////////////

SqlHandle::SqlHandle(){
	pthread_rwlock_init(&rwlock_mutex_, NULL);
}

SqlHandle::~SqlHandle() {
	DestoryPool();
	pthread_rwlock_destroy(&rwlock_mutex_);
}

bool SqlHandle::InitDB(string host, string user, string pwd, string db,
		uint32 port, string unix_socket, unsigned long clinet_flag) {
	connection_pool_.host_ = host;
	connection_pool_.user_ = user;
	connection_pool_.pwd_ = pwd;
	connection_pool_.db_ = db;
	connection_pool_.port_ = port;
	connection_pool_.unix_socket_= unix_socket;
	connection_pool_.client_flag_ = clinet_flag;

	return true;
}


void SqlHandle::Config(map<string, string>& parameters) {

	if (parameters["host"].empty()) {
		logger_->Error("sqlhandle profile lost[%s]!!!", "host");
	} else {
		connection_pool_.host_ = parameters["host"];
	}

	if (parameters["user"].empty()) {
		logger_->Error("sqlhandle profile lost[%s]!!!", "user");
	} else {
		connection_pool_.user_ = parameters["user"];
	}

	if (parameters["pwd"].empty()) {
		logger_->Error("sqlhandle profile lost[%s]!!!", "pwd");
	} else {
		connection_pool_.pwd_ = parameters["pwd"];
	}

	if (parameters["db"].empty()) {
		logger_->Error("sqlhandle profile lost[%s]!!!", "db");
	} else {
		connection_pool_.db_ = parameters["db"];
	}

	if (parameters["port"].empty()) {
		logger_->Error("sqlhandle profile lost[%s]!!!", "port");
	} else {
		connection_pool_.port_ = atoi(parameters["port"].c_str());
	}

	if (parameters["unix_socket"].empty()) {
		logger_->Error("sqlhandle profile lost[%s]!!!", "unix_socket");
	} else {
		connection_pool_.unix_socket_ = parameters["unix_socket"];
	}

	if (parameters["client_flag"].empty()) {
		logger_->Error("sqlhandle profile lost[%s]!!!", "client_flag");
	} else {
		connection_pool_.client_flag_ = atoi(parameters["client_flag"].c_str());
	}

	if (parameters["max_size"].empty()) {
		logger_->Error("sqlhandle profile lost[%s]!!!", "max_size");
	} else {
		connection_pool_.max_size_ = atoi(parameters["max_size"].c_str());
	}

	if (parameters["min_size"].empty()) {
		logger_->Error("sqlhandle profile lost[%s]!!!", "min_size");
	} else {
		connection_pool_.min_size_ = atoi(parameters["min_size"].c_str());
	}

	if (parameters["timeout"].empty()) {
		logger_->Error("sqlhandle profile lost[%s]!!!", "timeout");
	} else {
		connection_pool_.timeout_ = atoi(parameters["timeout"].c_str());
	}

	CreatePool();
}

int32 SqlHandle::Select(const char* sql, string &result) {
	int ret = -1;
	if (sql == nullptr || *sql == 0) {
		return ret;
	}
	if (strstr(sql, "SELECT") == NULL && strstr(sql, "select") == NULL) {
		logger_->Warning("The select interface does not support other data operations! sql=[%s]",sql);
		return ret;
	}
	return Execute(sql, result);
}
int32 SqlHandle::Insert(const char* sql) {
	int ret = -1;
	if (sql == nullptr || *sql == 0) {
		return ret;
	}
	if (strstr(sql, "INSERT") == NULL && strstr(sql, "insert") == NULL) {
		logger_->Warning("The insert interface does not support other data operations! sql=[%s]",sql);
		return ret;
	}
	return NoRawExecute(sql);
}
int32 SqlHandle::Update(const char* sql) {
	int ret = -1;
	if (sql == nullptr || *sql == 0) {
		return ret;
	}
	if (strstr(sql, "UPDATE") == NULL && strstr(sql, "update") == NULL) {
		logger_->Warning("The update interface does not support other data operations! sql=[%s]",sql);
		return ret;
	}
	return NoRawExecute(sql);
}
int32 SqlHandle::Delete(const char* sql) {
	int ret = -1;
	if (sql == nullptr || *sql == 0) {
		return ret;
	}
	if (strstr(sql, "DELETE") == NULL && strstr(sql, "delete") == NULL) {
		logger_->Warning("The delete interface does not support other data operations! sql=[%s]",sql);
		return ret;
	}
	return NoRawExecute(sql);
}

int32 SqlHandle::ExecuteBulk(const std::vector<std::string>& command) {
	int ret = 0;
	std::string sql;
	if (command.empty() || command.size() == 1) {
		  return -1;
	}
	int size = command.size();
	sql = base::utils::StringUtil::MergeString(command, string(";"));

	ConnectionPool *pool = &connection_pool_;
	MYSQL *conn = pool->GetConnection();
	MYSQL_RES *res = nullptr;
	if (conn == nullptr) {
		logger_->Warning("GetConnection error when I call Execute!");
		return -1;
	}
	if (mysql_set_server_option(conn, MYSQL_OPTION_MULTI_STATEMENTS_ON) != 0) {
		logger_->Warning("MYSQL mysql_set_server_option fail! error=[%s]! ",mysql_error(conn));
		pool->Close(conn);
		return -1;
	}

	if (mysql_query(conn, sql.c_str()) != 0) {
		logger_->Warning("MYSQL mysql_query fail! sql=[%s] ,error=[%s]! ",sql.c_str() ,mysql_error(conn));
		pool->Close(conn);
		return -1;
	}

	do {
		res = mysql_store_result(conn);
		if (res == nullptr) {
			if (mysql_field_count(conn) == 0) {
				long num_rows = mysql_affected_rows(conn);
				if (num_rows == -1) {
					logger_->Warning("MYSQL mysql_affected_rows fail! error=[%s]! ",mysql_error(conn));
					pool->Close(conn);
					return -1;
				} else {
					logger_->Debug("MYSQL mysql_affected_rows success! affected_rows=[%ld]! ",num_rows);
					ret += num_rows;
				}
			} else {
				logger_->Warning("MYSQL mysql_store_result fail! error=[%s]! ",mysql_error(conn));
				pool->Close(conn);
				return -1;
			}
			ClearResult(res);
		} else {
			logger_->Debug("MYSQL illegal operation! sql=[%s]! ",sql.c_str());
			ClearResult(res);
		}
	}while(!mysql_next_result(conn));

	if (mysql_set_server_option(conn, MYSQL_OPTION_MULTI_STATEMENTS_OFF) != 0) {
		logger_->Warning("MYSQL mysql_set_server_option fail! error=[%s]! ",mysql_error(conn));
		pool->Close(conn);
		return -1;
	}
	if (ret != size) {
		logger_->Warning("SqlHandle ExecuteBulk fail! expect_rows=[%d], success_rows=[%d]! ", size, ret);
	}
	pool->Close(conn);
	return ret;
}

int32 SqlHandle::Transaction(const std::vector<std::string>& command) {
	int ret = -1;
	std::string sql;
	ConnectionPool *pool = &connection_pool_;
	MYSQL *conn = pool->GetConnection();
	MYSQL_RES *res = nullptr;
	if (conn == nullptr) {
		logger_->Warning("GetConnection error when I call Execute!");
		return -1;
	}

	if (mysql_query(conn, "BEGIN") != 0) {
		logger_->Warning("MYSQL mysql_query BEGIN fail! error=[%s]! ",mysql_error(conn));
		pool->Close(conn);
		return -1;
	}

	for (auto it = command.cbegin(); it != command.cend(); ++it) {
		sql = *it;
		if (mysql_query(conn, sql.c_str()) != 0) {
			logger_->Warning("MYSQL mysql_query fail! sql=[%s] ,error=[%s]! ",sql.c_str() ,mysql_error(conn));

			if (mysql_query(conn, "ROLLBACK") != 0) {
				logger_->Warning("MYSQL mysql_query ROLLBACK fail! error=[%s]! ",mysql_error(conn));
			}
			pool->Close(conn);
			return -1;
		}
		res = mysql_store_result(conn);
		if (res == nullptr) {
			if (mysql_field_count(conn) == 0) {
				long num_rows = mysql_affected_rows(conn);
				if (num_rows == -1) {
					logger_->Warning("MYSQL mysql_affected_rows fail! error=[%s]! ",mysql_error(conn));
					pool->Close(conn);
					return -1;
				} else {
					logger_->Debug("MYSQL mysql_affected_rows success! affected_rows=[%ld]! ",num_rows);
					ret += num_rows;
				}
			} else {
				logger_->Warning("MYSQL mysql_store_result fail! error=[%s]! ",mysql_error(conn));
				pool->Close(conn);
				return -1;
			}
		} else {
			logger_->Debug("MYSQL illegal operation! sql=[%s]! ",sql.c_str());
			ClearResult(res);
		}
	}

	if (mysql_query(conn, "COMMIT") != 0) {
		logger_->Warning("MYSQL mysql_query COMMIT fail!,error=[%s]! ",mysql_error(conn));
		pool->Close(conn);
		return -1;
	}

	pool->Close(conn);
	return ret;

}

int32 SqlHandle::NoRawExecute(const char* sql) {
	ConnectionPool *pool = &connection_pool_;
	MYSQL *conn = pool->GetConnection();
	MYSQL_RES *res = nullptr;
	if (conn == nullptr) {
		logger_->Warning("GetConnection error when I call Execute!");
		return -1;
	}
	if (mysql_query(conn, sql) != 0) {
		logger_->Warning("MYSQL mysql_query fail! sql=[%s] ,error=[%s]! ",sql ,mysql_error(conn));
		pool->Close(conn);
		return -1;
	}
	res = mysql_store_result(conn);
	if (res == nullptr) {
		if (mysql_field_count(conn) == 0) {
			long num_rows = mysql_affected_rows(conn);
			if (num_rows == -1) {
				logger_->Warning("MYSQL mysql_affected_rows fail! error=[%s]! ",mysql_error(conn));
				pool->Close(conn);
				return -1;
			} else {
				logger_->Debug("MYSQL mysql_affected_rows success! affected_rows=[%ld]! ",num_rows);
				pool->Close(conn);
				return num_rows;
			}
		} else {
			logger_->Warning("MYSQL mysql_store_result fail! error=[%s]! ",mysql_error(conn));
			pool->Close(conn);
			return -1;
		}
	}
	logger_->Debug("MYSQL illegal operation! sql=[%s]! ",sql);
	ClearResult(res);
	pool->Close(conn);
	return -1;
}

int32 SqlHandle::Execute(const char* sql, string &result) {
	ConnectionPool *pool = &connection_pool_;
	MYSQL_RES *res = nullptr;
	MYSQL_ROW row = nullptr;
	MYSQL_FIELD *fields = nullptr;
	MYSQL *conn = pool->GetConnection();
	if (conn == nullptr) {
		logger_->Warning("GetConnection error when I call Execute!");
		return -1;
	}
	RWMutexScopedLock(&rwlock_mutex_, false);
	if (mysql_query(conn, sql) != 0) {
		logger_->Warning("MYSQL mysql_query fail! sql=[%s] ,error=[%s]! ",sql ,mysql_error(conn));
		pool->Close(conn);
		return -1;
	}
	 res = mysql_store_result(conn);
	if (res == nullptr) {
		logger_->Warning("MYSQL mysql_store_result fail! error=[%s]! ",mysql_error(conn));
		pool->Close(conn);
		return -1;
	}
	try {
			uint32 rows_index = 0;
			uint32 num_rows = GetNumRows(res);
			uint32 num_column = GetNumFields(res);
			Json::Value array;
			Json::Value object;
			Json::FastWriter writer;
			Json::Value tmp_json;

			fields = GetFetchFields(res);
			while ((row = mysql_fetch_row(res))) {
				for (uint32 i=0; i < num_column; i++) {
					array[fields[i].org_name] = row[i];
				}
				tmp_json.append(array);
				rows_index++;
			}
			if (rows_index != num_rows) {
				logger_->Warning("mysql_fetch_row data is lost, "
						"the expected rows = [%d], and the actual rows = [%d]", num_rows, rows_index);
			}
			ClearResult(res);
			pool->Close(conn);
			object["result"] = tmp_json;
			result = writer.write(object);
		} catch (std::exception &e) {
			logger_->Warning("MySQL ERROR : %s ", e.what());
		} catch (...) {
			logger_->Warning("sqlhandle Execute Erroor!");
		}

	return 0;
}

uint32 SqlHandle::GetNumRows(MYSQL_RES *res) {
	return mysql_num_rows(res);
}
uint32 SqlHandle::GetNumFields(MYSQL_RES *res) {
	return mysql_num_fields(res);
}
MYSQL_FIELD *SqlHandle::GetFetchFields(MYSQL_RES *res) {
	return mysql_fetch_fields(res);
}

unsigned long *SqlHandle::GetFetchLengths(MYSQL_RES *res) {
	return mysql_fetch_lengths(res);
}

bool SqlHandle::CreatePool() {
	return connection_pool_.InitPool();
}

void SqlHandle::DestoryPool() {
	connection_pool_.DestoryPool();
}

void SqlHandle::ClearResult(MYSQL_RES *res) {
	if (res != nullptr) {
		mysql_free_result(res);
		res = nullptr;
	}
}

} //db

