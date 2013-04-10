#include "mempool.hpp"
#include <assert.h>
#include <memory.h>
#include <cstddef>

MemPool::MemPool() {
	buf = NULL;
	listener = NULL;
}

MemPool::MemPool(int szbuf, int szlistener)
	: szBuf(szbuf),szListener(szlistener) {
	buf = new char[szBuf];
	listener = new int[szListener];

	assert (buf != NULL && listener != NULL) ;
	nowBuf = nowListener = 0;
}

MemPool::~MemPool() {
	if (buf != NULL) delete[] buf;
	if (listener != NULL) delete[] listener;
}

int MemPool::newListener() {
	listener[nowListener] = nowBuf;
	return nowListener ++;
}

int MemPool::getNext(int lid, char *res, int options) {
	int& now = listener[lid];
	if (now == nowBuf) return 0;
	int& len = *(int *)(buf + now);
	if (options & MEM_NEW) res = new char[len];
	int pos = now + sizeof(int);
	if (szBuf - pos < len) pos = 0;

	if (options & (MEM_COPY | MEM_NEW)) memcpy(res,buf+pos, len);
	else res = buf + pos;
	now = pos + len;
	if (szBuf - now < sizeof(int)) now = 0;
	return len;	
}

char *MemPool::readyBlock(int len) {
	if (szBuf - nowBuf < sizeof(int)) nowBuf = 0;
	*(int *)(buf + nowBuf) = len;
	nextBuf = nowBuf + sizeof(int);
	if (szBuf - nextBuf < len) nextBuf = 0;

	char *res = buf + nextBuf;
	nextBuf += len;
	return res;
}

void MemPool::doneBlock() {
	nowBuf = nextBuf;
}
