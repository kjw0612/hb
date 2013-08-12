#pragma once
class VisInfo;

#include "Document.h"
#include "VisWindow.h"

#include <vector>
#include <string>
#include <map>
#include <set>

class PathInfo 
{
public:
	PathInfo() ;
	~PathInfo() ;
	std::vector<std::string> dayList;
	std::map<std::string, std::set<std::string> > fileSet;

	void Initialize(std::string path);
};

class VisInfo
{
	static VisInfo *instance;
public:
	VisInfo(void);
	~VisInfo(void);
	std::string dataPath;
	PathInfo pathInfo;

	static VisInfo *getInstance();
	std::map<std::string, Document *> docs;
	std::map<int, VisWindow *> wins;

	void setDataPath(std::string path);
	Document* getDocument(std::string day, std::string filename);
	VisWindow* createWindow(Document *doc);
	void removeWindow(int id);
};

