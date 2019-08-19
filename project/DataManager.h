#pragma once
#pragma once
#include "common.h"

class SqliteManager {
public:
	SqliteManager() :_db(nullptr) {}
	//~SqliteManager() {
	//	//�ر����ݿ�
	//	Close();
	//}
	//�������ݿ�
	void Open(const string& path);
	//�ر��ļ�������
	void Close();
	//ִ��sql���
	void ExecuteSql(const string& sql);
	//�õ���ά�����
	//void Get(sqlite3*& _db);
	void GetTable(const string& sql, int& row, int& col, char**& ppRet);
	//��ֹ����
	SqliteManager(const SqliteManager&) = delete;
	SqliteManager& operator=(const SqliteManager&) = delete;
private:
	//���ݿ����
	sqlite3* _db;
	char* errmsg;
};
//��Ϊϵͳ�ӿڻ��Զ������ڴ棬������������RAII��˼�뽫GetTable���з�װ��
//����������Զ��ͷſռ�
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
	//��ʼ����
	void Init();
	//�õ���ǰĿ¼�µ��ļ��к��ļ�
	void GetDoc(const string& path, set<string>& doc);
	void InsertDoc(const string& path, const string& doc);
	void DeleteDoc(const string& path, const string& doc);
	void SearchDoc(const string& key, map<string, string>& doc);
	void HighLight(const string& key, const string& dest);
private:
	SqliteManager  _dbmgr;

};
