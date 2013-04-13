#ifndef MEMPOOL_HPP_
#define MEMPOOL_HPP_

#define MEM_NEW 1
#define MEM_COPY 2
struct MemPool {
	char *buf;
	int nowBuf, nextBuf, szBuf;
	int *listener;
	int nowListener, szListener;

	MemPool();
	MemPool(int szBuf, int szListener=1) ;
	~MemPool();

	inline int newListener(); // return enw listener id
	int getNext(int listener_id, char *buf, int MEMoption=(MEM_NEW | MEM_COPY)) ; // return length

	inline char *readyBlock(int len);
	inline void doneBlock();
};


inline int MemPool::newListener() {
	listener[nowListener] = nowBuf;
	return nowListener ++;
}

inline char *MemPool::readyBlock(int len) {
	if (szBuf - nowBuf < sizeof(int)) nowBuf = 0;
	*(int *)(buf + nowBuf) = len;
	nextBuf = nowBuf + sizeof(int);
	if (szBuf - nextBuf < len) nextBuf = 0;

	char *res = buf + nextBuf;
	nextBuf += len;
	return res;
}

inline void MemPool::doneBlock() {
	nowBuf = nextBuf;
}


#endif//MEMPOOL_HPP_
