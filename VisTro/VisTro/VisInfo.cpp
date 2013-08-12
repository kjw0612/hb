#include "stdafx.h"
#include "VisInfo.h"
#include "Logger.h"
#include <string>
#include <algorithm>
#include <vector>
#include <map>

using namespace std;


namespace {
	bool isYYYYMMDD(const char *name) {
		if (strlen(name) != 8) return false;
		int value = atoi(name);
		return 19000000 <= value && value <= 21000000;
	}

	class PathManager {
	public:
		WIN32_FIND_DATA wfd;
		HANDLE hFind;
		BOOL next;

		PathManager(const char *sPath) {
			next = false;
			if((hFind = FindFirstFile(sPath, &wfd)) == INVALID_HANDLE_VALUE)
			{
				hFind = NULL;
				return;
			}
			do
			{
				if(strcmp(wfd.cFileName, ".") != 0 && strcmp(wfd.cFileName, "..") != 0)
				{
					next = true;
					break;
				}
			}
			while(FindNextFile(hFind, &wfd));
		}
		~PathManager() {
			if (hFind != NULL) FindClose(hFind);
		}

		BOOL hasNext() {
			return next;
		}

		WIN32_FIND_DATA getNext() {
			WIN32_FIND_DATA res = wfd;
			if (next) {
				next = FindNextFile(hFind, &wfd);
			}
			return res;
		}
	};
}

PathInfo::PathInfo(void) {
}

PathInfo::~PathInfo(void) {
}

void PathInfo::Initialize(string path) {
    char sPath[4096], gPath[4096];

	sprintf_s(sPath, sizeof(sPath), "%s\\*", path.c_str());
	log_println("sPath : " + (string)sPath);

	PathManager manager(sPath);
	while (manager.hasNext()) {
		WIN32_FIND_DATA fdFile = manager.getNext();
		if(fdFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			string dirName = fdFile.cFileName;
			if (isYYYYMMDD(dirName.c_str())) {
				dayList.push_back(dirName);
				sprintf_s(gPath, sizeof(gPath), "%s\\%s\\*.csv", path.c_str(), dirName.c_str());

				PathManager dayManager(gPath);
				while (dayManager.hasNext()) {
					WIN32_FIND_DATA csvFile = dayManager.getNext();
					if (!(csvFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
						string filename = csvFile.cFileName;
						fileSet[dirName].insert(filename);
					}
				}
			}
		}
	}
}


VisInfo * VisInfo::instance = NULL;

VisInfo::VisInfo(void)
{
}


VisInfo::~VisInfo(void)
{
}

VisInfo *VisInfo::getInstance() {
	if (instance == NULL) instance = new VisInfo();
	return instance;
}

void VisInfo::setDataPath(string path) {
	dataPath = path;
	pathInfo.Initialize(dataPath);
}


Document* VisInfo::getDocument(std::string day, std::string filename) {
	string path = dataPath + "\\" + day + "\\" + filename;
	if (docs.find(path) == docs.end()) {
		docs[path] = new Document(path.c_str());
	}
	return docs[path];
}


static int wId = 0;
VisWindow* VisInfo::createWindow(Document *doc) {
	int nowid = wId ++;
	wins[nowid] = new VisWindow(this, doc, nowid);
	return wins[nowid];
}

void VisInfo::removeWindow(int id) {
	wins.erase(id);
}