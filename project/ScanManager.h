#pragma once
#include"common.h"
#include"DataManager.h"

class ScanManager {
public:
	//开始比对扫描
	ScanManager(vector<string>& path) : _path(path) {}
	void Start() {
		Thread();
		Search();
	}
	void DirectoryList(const string& path, vector<string>& subdirs, vector<string>& subfiles);
	void Scan(string& path);
	void Thread() {
		for (int i = 0; i < _path.size(); ++i) {
			thread thr(&ScanManager::Scan,this,std::ref(_path[i]));
			thread_list.push_back(std::move(thr));
		}
		for (int i = 0; i < thread_list.size();++i) {
			thread_list[i].join();
		}
	}
	void Search();
private:
	DataManager _datamgr;
	vector<string> _path;
	vector<thread> thread_list;
};