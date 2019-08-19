#pragma once
#include"ScanManager.h"
#include"DataManager.h"
//�Ե�ǰĿ¼����ɨ��
void ScanManager::DirectoryList(const string& path, vector<string>& subdirs, vector<string>& subfiles) {

	//�ṹ��
	_finddata_t file;
	//�鿴���� ���ֵ� 
	string path_ = path + "\\*.*";
	intptr_t handle = _findfirst(path_.c_str(), &file);
	if (handle == -1) {
		cout << "_findfirst : %s" << path_.c_str() << endl;
	}
	//������Ŀ¼��ѯ
	do {
		if (file.attrib & _A_SUBDIR) {
			//Ϊ�˷�ֹ�ļ�ϵͳѭ����Ӧ�ù�ܵ� . .. �������ļ�
			if (strcmp(file.name, ".") != 0 && strcmp(file.name, "..") != 0) {
				subdirs.push_back(file.name);
			}
		}
		else {
			subfiles.push_back(file.name);
		}
	} while (_findnext(handle, &file) == 0);
	_findclose(handle);
}
void ScanManager::Scan(string& path) {

	//���ļ�ϵͳ�����ݿ���бȶ�
	vector<string> localdirs;
	vector<string> localfiles;
	//��ȡ��·���µ�Ŀ¼���ļ�
	DirectoryList(path, localdirs, localfiles);
	//��Ŀ¼���ļ����ӵ�set����,ʵ������+ȥ��
	std::set<string> localset;
	localset.insert(localdirs.begin(), localdirs.end());
	localset.insert(localfiles.begin(), localfiles.end());

	//���ݿ����
	set<string> dbset;
	_datamgr.GetDoc(path, dbset);
	//����·���µ��ļ��Լ�Ŀ¼���бȶ�
	auto localit = localset.begin();
	auto dbit = dbset.begin();

	while (localit != localset.end() && dbit != dbset.end()) {
		if (*localit > *dbit) {
			//ɾ�����ݿ������
			_datamgr.DeleteDoc(path, *dbit);
			dbit++;
		}
		else if (*localit < *dbit) {
			//�����ݿ���������
			_datamgr.InsertDoc(path, *localit);
			localit++;
		}
		else {
			//����һ�£�ͬʱ���Ƚ�
			localit++;
			dbit++;
		}
	}
	while (localit != localset.end()) {
		//�����ݿ��������ļ�
		_datamgr.InsertDoc(path, *localit);
		localit++;
	}
	while (dbit != dbset.end()) {
		_datamgr.DeleteDoc(path, *dbit);
		dbit++;
	}

	//���ݹ�ȶ�������Ŀ¼
	for (auto& subdirs : localdirs) {
		string subpath = path;
		subpath += '\\';
		subpath += subdirs;
		Scan(subpath);
	}
}
void ScanManager::Search() {
	while (1) {
		string key;
		map<string, string> docs;
		docs.clear();
		printf("��������Ҫ���ҵĶ���");
		cin >> key;
		printf("%-50s%-50s\n", "path", "name");
		cout << "================================================================" << endl;
		_datamgr.SearchDoc(key, docs);
		for (auto& e : docs) {
			cout << e.first;
			for (int i = e.first.size(); i < 50; ++i) {
				cout << " ";
			}
			_datamgr.HighLight(key,e.second);
			//cout << e.second;
			for (int j = e.second.size(); j < 50; ++j) {
				cout << " ";
			}
			cout << endl;
		}
		cout << "================================================================" << endl;
	}

}


