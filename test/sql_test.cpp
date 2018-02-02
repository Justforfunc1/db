/**********************************************************
 * Copyright (C) 2017 All rights reserved.
 * \file sql_test.cpp
 * \brief
 * \note	注意事项：
 *
 *\version 1.0
 ** \author Allen.L
 ** \date 2017-08-23
 ** \last modified 2010-01-01 14:55:44
**********************************************************/

#include "base/base.h"
#include "sql_handle.h"
#include "mysql/mysql.h"

#define INIT_FILE "mysql.ini"
using namespace std;
using namespace base;
using namespace db;


void read1 () {

	const char * sql = "SELECT t.code,t.name FROM s_area1 t where t.pcode = 0";
	string result;
	if (SqlHandle::Instance().Select(sql,result) == 0) {
		cout << result <<endl;
	}


}

void write1 () {
	const char * sql = "INSERT into s_area1 VALUES(830000, '测试行政区1',0,0,NOW())";
	if (SqlHandle::Instance().Insert(sql) != -1) {
		cout << "install success!!" << endl;
	}

}

int main( int argc, char **argv ) {
	CONFIG_LOAD(INIT_FILE);
	string log_file = string(CONFIG_GETSTR("logging", "logger", "../logs/sql_test_YYYYMMDD.log"));
	string log_level = string(CONFIG_GETSTR("logging", "level", "debug"));
	BASE_LOGGER_INIT(log_file, log_level, false);

	map<string, string> sqlparms;
	vector<string> keys;
	BASE_CONFIG.GetSectionKey("mysql", keys);
	for (vector<string>::const_iterator it = keys.begin(); it != keys.end(); it++) {
		sqlparms[*it] = CONFIG_GETSTR("mysql", *it, "");
	}

	SqlHandle::Instance().Config(sqlparms);

	string sql = string( "INSERT INTO `s_area1` VALUES (110000, '北京市', 0, 0, '2017-8-22 09:13:11');")
			+ "INSERT INTO `s_area1` VALUES (110100, '市辖区', 110000, 0, '2017-8-22 09:11:21');"
			+ "INSERT INTO `s_area1` VALUES (110101, '东城区', 110100, 0, '2017-8-22 08:59:47')";

	int ret = 0;
	Command command(sql);

//	int size = command.GetCommandSize();
//	cout << size << endl;
//	std::vector<string> mycom= command.GetCommand();
//	for (auto it = mycom.cbegin(); it != mycom.cend(); ++it) {
//		cout << *it << endl;
//	}
//	string sql1("INSERT INTO `s_area1` VALUES (110102, '西城区', 110100, 0, '2017-8-22 08:59:47')");
//	command.PushCommand(sql1);

	//测试接口 批量插入
	int size = command.GetCommandSize();
	ret = SqlHandle::Instance().ExecuteBulk(command);
	if (ret == size) {
		cout << "ExecuteBulk success !!" << endl;
	} else {
		cout << "ExecuteBulk fail!!" << ret << endl;
	}

	//测试接口 插入
	command.ClearCommand();
	const char* sql1 = "INSERT INTO `s_area1` VALUES (110102, '西城区', 110100, 0, '2017-8-22 08:59:47')";

	ret = SqlHandle::Instance().Insert(sql1);
	if (ret != -1) {
		cout << "Insert success!!" << endl;
	} else {
		cout << "Insert fail!!" << endl;
	}

	//测试接口 查询
	string result;
	const char* sql2 = "SELECT * FROM s_area1;";
	ret = SqlHandle::Instance().Select(sql2,result);
	if (ret != -1) {
		cout << "Select success!! result = " << result << endl;
	} else {
		cout << "Select fail!!"<< endl;
	}

	//测试接口 更新
	const char* sql3 = "update s_area1 set name = '南城区' where code = 110102;";
	ret = SqlHandle::Instance().Update(sql3);
	if (ret != -1) {
		cout << "Update success!! ret = " << ret << endl;
	} else {
		cout << "Update fail!!" << endl;
	}

	//测试接口 删除
	const char* sql4 = "delete FROM s_area1 where code = 110100;";
	ret = SqlHandle::Instance().Delete(sql4);
	if (ret != -1) {
		cout << "Delete success!! ret = " << ret << endl;
	} else {
		cout << "Delete fail!!" << endl;
	}

	//测试接口 查询
	ret = SqlHandle::Instance().Select(sql2, result);
	if (ret != -1) {
		cout << "Select success!! result = " << result << endl;
	} else {
		cout << "Select fail!!" << endl;
	}

	//测试接口 事务
	ret = SqlHandle::Instance().Transaction(command);
	if (ret != -1) {
		cout << "Transaction success!!" << endl;
	} else {
		cout << "Transaction fail!!" << endl;
	}

	sleep(2);



	std::thread thread1(read1);
	std::thread thread2(read1);
	std::thread thread3(write1);

	thread1.join();
	thread2.join();
	thread3.join();

	return 0;
}
