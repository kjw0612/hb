#ifndef MEMPOOL_HPP_
#define MEMPOOL_HPP_

#define MEM_NEW 1
#define MEM_COPY 2
struct MemPool {
	char *buf;
	int nowBuf, nextBuf, szBuf, szMaxAlloc;
	int *listener;
	int nowListener, szListener;

	MemPool();
	MemPool(int szBuf, int szMaxAlloc, int szListener=1) ;
	~MemPool();

	inline int newListener(); // return enw listener id
	int getNext(int listener_id, char *buf, int MEMoption=(MEM_NEW | MEM_COPY)) ; // return length

	inline char *readyBlock();
	inline void doneBlock(int len);
};


inline int MemPool::newListener() {
	listener[nowListener] = nowBuf;
	return nowListener ++;
}

inline char *MemPool::readyBlock() {
	if (szBuf - nowBuf < sizeof(int)) nowBuf = 0;

	nextBuf = nowBuf + sizeof(int);
	if (szBuf - nextBuf < szMaxAlloc) nextBuf = 0;

	char *res = buf + nextBuf;
	nextBuf += szMaxAlloc;
	return res;
}

inline void MemPool::doneBlock(int len) {
	*(int *)(buf + nowBuf) = len;
	nowBuf = nextBuf - szMaxAlloc + len;
}


#endif//MEMPOOL_HPP_
