#include"DataManager.h"
void SqliteManager::Open(const string& path) {

	int ret = sqlite3_open(path.c_str(), &_db);
	if (ret != SQLITE_OK) {
		assert(false);
	}
}
void SqliteManager::Close() {
	sqlite3_close(_db);
}
void SqliteManager::ExecuteSql(const string& sql) {
	sqlite3_exec(_db, sql.c_str(), nullptr, nullptr, &errmsg);
}
void SqliteManager::GetTable(const string& sql, int& row, int& col, char**& ppRet) {
	int ret = sqlite3_get_table(_db, sql.c_str(), &ppRet, &row, &col, &errmsg);
	if (ret != SQLITE_OK) {
		//assert(false);
	}
}
void DataManager::Init() {
	char sql[256];
	sprintf(sql, "create table %s (id integer primary key autoincrement,doc_path text,doc_name text,doc_name_pinyin text,doc_name_initials text)", TB_DOC);
	_dbmgr.ExecuteSql(sql);

}
void DataManager::GetDoc(const string& path, set<string>& doc) {
	char query_sql[1024];
	sprintf(query_sql, "select doc_name from %s where doc_path = '%s'", TB_DOC, path.c_str());
	int row, col;
	char** ppRet;
	AutoGetTable get(&_dbmgr, query_sql, row, col, ppRet);
	for (int i = 1; i <= row; ++i) {
		doc.insert(ppRet[i]);
	}
}

void DataManager::InsertDoc(const string& path, const string& doc) {

	char sql[1024];
	sprintf(sql, "insert into %s(doc_path,doc_name,doc_name_pinyin,doc_name_initials) values('%s','%s','%s','%s')", TB_DOC, path.c_str(), doc.c_str(), ChineseConvertPy(doc).c_str(), ChineseConvertPinYinInitials(doc).c_str());
	_dbmgr.ExecuteSql(sql);
}
void DataManager::DeleteDoc(const string& path, const string& doc) {
	char sql[1024];
	sprintf(sql, "delete from %s where doc_path = '%s' and doc_name = '%s'", TB_DOC, path.c_str(), doc.c_str());
	_dbmgr.ExecuteSql(sql);

	//char sql_s[256];
	string path_ = path;
	path_ += '\\';
	path_ += doc;
	sprintf(sql, "delete from %s where doc_path like '%s%%'", TB_DOC, path_.c_str());
	_dbmgr.ExecuteSql(sql);
}
void DataManager::SearchDoc(const string& key, map<string, string>& docs) {
	//利用模糊匹配搜索数据并打印
	//原串查询
	char sql[1024];
	//sqlite中文模糊匹配不支持模糊查询，只支持精确查询
	sprintf(sql, "select doc_path,doc_name from %s where doc_name like '%%%s%%' or doc_name_pinyin like '%%%s%%' or doc_name_initials like '%%%s%%'", TB_DOC, key.c_str(), ChineseConvertPy(key).c_str(), ChineseConvertPinYinInitials(key).c_str());
	int row, col;
	char** ppRet;
	AutoGetTable get(&_dbmgr, sql, row, col, ppRet);
	for (int i = 1; i <= row; ++i) {
		docs.insert(make_pair(ppRet[i*col + 0], ppRet[i*col + 1]));
	}
	
}
void DataManager::HighLight(const string& key, const string& dest) {

	//原串全称高亮
	{
		if (dest.find(key) != string::npos) {

			size_t first_end = dest.find(key);
			string pre_str = dest.substr(0, first_end);
			cout << pre_str;
			int length = key.size();
			string order_str = dest.substr(first_end, length);
			ColourPrintf(order_str.c_str());
			string after_str = dest.substr(first_end + length, string::npos);
			cout << after_str ;
			return;
		}

	}
	//根据拼音搜索
	{

		string key_pinyin = ChineseConvertPy(key);
		string dest_pinyin = ChineseConvertPy(dest);
		size_t start_pinyin = 0, start_dest = 0;
		size_t index_pinyin = 0, index_dest = 0;
		if (dest_pinyin.find(key_pinyin) != string::npos) {
			size_t first_end_pinyin = dest_pinyin.find(key_pinyin);
			//基于GBK编码，所以每一个汉字是占两个字节
			char Ch[3] = {0};
			while (index_pinyin < first_end_pinyin) {
				if (dest[index_dest] < 0) {
					Ch[0] = dest[index_dest];
					Ch[1] = dest[index_dest + 1];
					string tmp = ChineseConvertPy(Ch);
					index_pinyin += tmp.size();
					index_dest += 2;
					if (index_pinyin>first_end_pinyin) {
						return;
					}
				}
				else {
					index_pinyin += 1;
					index_dest += 1;
				}
				
			}
			//此时已经求得原串的起始下标
			string pre = dest.substr(start_dest,index_dest);
			cout << pre;
			start_dest = index_dest;
			start_pinyin = index_pinyin;
			int x = index_pinyin + key_pinyin.size();
			while (index_pinyin < x) {
				//基于GBK编码，所以每一个汉字是占两个字节
				if (dest[index_dest]<0) {
					Ch[0] = dest[index_dest];
					Ch[1] = dest[index_dest + 1];
					string tmp = ChineseConvertPy(Ch);
					index_pinyin += tmp.size();
					index_dest += 2;
				}
				else {
					index_pinyin += 1;
					index_dest += 1;
				}
				
			}
			//此时已经求得原串的末尾下标
			string order = dest.substr(start_dest,index_dest - start_dest);
			ColourPrintf(order.c_str());
			string after = dest.substr(index_dest);
			cout << after;
			return;
		}

	}
	//根据首字母查找
	{

		string key_py = ChineseConvertPinYinInitials(key);
		string dest_py = ChineseConvertPinYinInitials(dest);
		size_t start_py = 0, start_dest = 0;
		size_t index_py = 0, index_dest = 0;
		if (dest_py.find(key_py) != string::npos) {
			size_t first_end_py = dest_py.find(key_py);
			//基于GBK编码，所以每一个汉字是占两个字节
			while (index_py < first_end_py) {
				if (dest[index_py]<0) {
					index_py += 1;
					index_dest += 2;
				}
				else {
					index_py += 1;
					index_dest += 1;
				}
				
			}
			//此时已经求得原串的起始下标
			string pre = dest.substr(start_dest, index_dest);
			cout << pre;
			start_dest = index_dest;
			start_py = index_py;
			int x = index_py + key_py.size();
			while (index_py < x) {
				//基于GBK编码，所以每一个汉字是占两个字节
				if (dest[index_dest]<0) {
					index_py += 1;
					index_dest += 2;
				}
				else {
					index_py += 1;
					index_dest += 1;
				}
				
			}
			//此时已经求得原串的末尾下标
			string order = dest.substr(start_dest, index_dest - start_dest);
			ColourPrintf(order.c_str());
			string after = dest.substr(index_dest);
			cout << after;
			return;
		}

	}
}

