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

	int newListener(); // return enw listener id
	int getNext(int listener_id, char *buf, int MEMoption=(MEM_NEW | MEM_COPY)) ; // return length

	char *readyBlock(int len);
	void doneBlock();
};

#endif//MEMPOOL_HPP_
