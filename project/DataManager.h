#pragma once
#pragma once
#include "common.h"

class SqliteManager {
public:
	SqliteManager() :_db(nullptr) {}
	//~SqliteManager() {
	//	//关闭数据库
	//	Close();
	//}
	//创建数据库
	void Open(const string& path);
	//关闭文件描述符
	void Close();
	//执行sql语句
	void ExecuteSql(const string& sql);
	//得到二维数组表
	//void Get(sqlite3*& _db);
	void GetTable(const string& sql, int& row, int& col, char**& ppRet);
	//防止拷贝
	SqliteManager(const SqliteManager&) = delete;
	SqliteManager& operator=(const SqliteManager&) = delete;
private:
	//数据库对象
	sqlite3* _db;
	char* errmsg;
};
//因为系统接口会自动开辟内存，所以用类似于RAII的思想将GetTable进行封装，
//作用域结束自动释放空间
class AutoGetTable {
public:
	AutoGetTable(SqliteManager* dbObject, const string& sql, int& row, int& col, char**& ppRet)
		:_dbObject(dbObject), _ppObject(nullptr) {
		_dbObject->GetTable(sql, row, col, ppRet);
		_ppObject = ppRet;
	}
	~AutoGetTable() {
		if (_ppObject) {
			sqlite3_free_table(_ppObject);
		}
	}
private:
	AutoGetTable(const AutoGetTable&) = delete;
	AutoGetTable& operator=(const AutoGetTable&) = delete;
private:
	SqliteManager* _dbObject;
	char**         _ppObject;
};
//////////////////////////////////////////////////////////////////////////////////////
class DataManager {
public:
	DataManager() {
		_dbmgr.Open(SQLITE);
		Init();
	}
	~DataManager() {
		_dbmgr.Close();
	}
	//初始化表
	void Init();
	//得到当前目录下的文件夹和文件
	void GetDoc(const string& path, set<string>& doc);
	void InsertDoc(const string& path, const string& doc);
	void DeleteDoc(const string& path, const string& doc);
	void SearchDoc(const string& key, map<string, string>& doc);
	void HighLight(const string& key, const string& dest);
private:
	SqliteManager  _dbmgr;

};
