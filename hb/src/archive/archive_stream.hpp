/*
 * archive_stream.hpp
 *
 *  Created on: 2013. 4. 9.
 *      Author: koval
 */

#ifndef ARCHIVE_STREAM_HPP_
#define ARCHIVE_STREAM_HPP_

namespace hb{
/*
	template<class _T1, class _T2>
	struct pair
	{
		pair() {}
		pair(const _T1& __first, const _T2& __second) : first(__first),second(__second){}
		_T1 first;
		_T2 second;
	};
	template<class _T1, class _T2>
	inline hb::pair<_T1, _T2> make_pair(const _T1& __first, const _T2& __second){
		return hb::pair<_T1, _T2>(__first, __second);
	}

	class archive_stream{
	public:
		archive_stream(){}
		hb::pair<int, void *> getnext(){
			return hb::make_pair(0, 0);
		}
	};
	*/


	inline void fGetData(char *p, int len, FILE *fp ){
		while (len -->0 && !feof(fp) ){
			*p++ = fgetc(fp);
		}
	}

	class archive_stream{
	public:
		archive_stream(const char * filename, int MEMPOOL_SIZE = 5000001, int MAX_BUF_SIZE = 2001){
			fp = fopen(filename,"rb");
			msgbuf = new char[MAX_BUF_SIZE];
			mp = new MemPool(MEMPOOL_SIZE); rt = 0; sz = 0;
		}
		archive_stream(const char * filename, MemPool* __mp, int MAX_BUF_SIZE = 2001){
			fp = fopen(filename,"rb");
			msgbuf = new char[MAX_BUF_SIZE];
			mp = __mp; rt = 0; sz = 0;
		}
		~archive_stream(){
			fclose(fp);
			delete[] msgbuf;
			delete mp;
		}
		MemPool * getMemPool(){
			return mp;
		}
		int base_next(){
			if (!fp || !feof(fp))
				return 0;
			fGetData((char *)&rt, 8, fp);
			fGetData((char *)&sz, 4, fp);
			memset(msgbuf,0,sizeof(msgbuf));
			fGetData(msgbuf, sz, fp);
			return sz;
		}
		FILE *fp;
		char *msgbuf;
		long long rt;
		int sz;
		MemPool * mp;
	};

}


#endif /* ARCHIVE_STREAM_HPP_ */
