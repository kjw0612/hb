#include "stdafx.h"
#include "Document.h"
#include "MarketDataLine.h"
#include "xTime.h"

#include <algorithm>
using namespace std;

namespace {
	bool isEndLine(char c) {
		return c=='\n'||c=='\r';
	}
	const int szBUF = 4096;
	class FileBuffer {
	public:
		FILE *fp;
		char buffer[szBUF];
		int now, len, remain, szFile;
		FileBuffer(const char *path) {
			fp = NULL;
			fp = _fsopen(path, "rb", _SH_DENYNO);
			fseek(fp, 0L, SEEK_END);
			szFile = remain = ftell(fp);
			now = 0; len = -1;
			fseek(fp, 0L, SEEK_SET);
		}
		~FileBuffer() {
			fclose(fp);
		}

		void setBuf(int sz){
			fread(buffer, sz, 1, fp);
			now = 0; len = sz;
			remain -= sz;
		}

		char getc() {
			if (now >= len) {
				if (remain <= 0) return -1;
				setBuf( min(remain, szBUF) );
			}
			return buffer[now++];
		}

		void gets(char *str,int mxLen) {
			for (int i=0;i<mxLen-1;i++) {
				char c = getc();
				if (isEndLine(c) || eof()) {
					if ( i > 0 || eof() ){
						str[i] = '\0';
						break;
					} else {
						i --;
						continue;
					}
				}
				str[i] = c;
			}
		}

		bool eof() {
			return now >= len && remain == 0;
		}
	};
};

namespace {
DWORD WINAPI ThreadLoading( LPVOID lpParam ) {
	char buf[4096];
	Document *doc = (Document *)lpParam;
	doc->LoadingPhase = 0.0;
	doc->status = Document::Status::LOADING;

	FileBuffer buffer(doc->filepath.c_str());

	if (buffer.szFile <= 0) {
		doc->status = Document::Status::LOAD_FAIL;
		return 0;
	}
	
	buffer.gets(buf, sizeof(buf)); // category
	MarketDataLine *bmdl = NULL;
	while (!buffer.eof()) {
		buffer.gets(buf, sizeof(buf));
		if (strlen(buf) < 24) continue; // ??
		MarketDataLine *mdl = new MarketDataLine(buf);
		if (bmdl != NULL && 
			mdl->trade.direction != 0 && 
			mdl->ask.isEmpty() && 
			mdl->bid.isEmpty()) {

			mdl->ask.setV(bmdl->ask);
			mdl->bid.setV(bmdl->bid);

			PQC *V = (mdl->trade.direction == -1)?mdl->bid.V:mdl->ask.V;
			int vcnt = (mdl->trade.direction == -1)?mdl->bid.vcnt:mdl->ask.vcnt;
			for (int i=0;i<vcnt;i++){
				if(V[i].Prc == mdl->trade.price) {
					V[i].Cnt = -1;
					V[i].Qty -= mdl->trade.vol;
				}
			}
		}

		if (!mdl->ask.isEmpty() && !mdl->bid.isEmpty()) {
			bmdl = mdl;
		}

		doc->mdls.push_back(mdl);
		doc->LoadingPhase = 100.0 - (100.0 * buffer.remain / (double)buffer.szFile);
	}

	doc->status = Document::Status::SHOW;
	return 0;
}

}

Document::Document(const char * filepath) : status(NONE)
{
	this->filepath = filepath;
	CreateThread(NULL, 0, ThreadLoading, this, 0, NULL);
}

Document::~Document(void)
{
	for (int i=0;i<(int)mdls.size();i++) {
		delete mdls[i];
	}
}

int Document::upperBound(xTime time) {
	if (mdls.empty()) return -1;
	int start, end;
	start = 0; end = (int)mdls.size() - 1;
	while (start <= end) {
		int mid = (start + end) / 2;
		if (time.getTime() > mdls[mid]->arriveTime.getTime()) {
			start = mid + 1;
		} else {
			end = mid - 1;
		}
	}

	return start - 1;
}
