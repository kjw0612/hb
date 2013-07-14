#ifndef rawdatareader_h__
#define rawdatareader_h__

#include <exception>
#include <memory.h>

class RawDataReader{
public:

	~RawDataReader()
	{
		delete[] msg;
		if (fp)
			fclose(fp);
	}

	RawDataReader() : fp(0), rt(0), sz(0), len(0), msg(0), offset(0) {}

	RawDataReader(const std::string& filename)
		: fp(0), rt(0), sz(0), len(0), msg(0), offset(0) 
	{
		fopen_s(&fp,filename.c_str(),"rb");
		if (fp==0){
			throw std::runtime_error("filename " + filename + " seems to be wrong");
		}
		const int msgbuf_size = 3001;
		msg = new char[msgbuf_size];
		memset(msg,0,msgbuf_size);
	}

	inline void fGetData(char *p, int len, FILE *fp ){
		offset += len;

		fread(p,sizeof(char),len,fp);
			/*
		while (len -->0 && !feof(fp) ){
			*p++ = fgetc(fp);
		}*/
	}

	void seek(long long pos){
#ifdef _POSIX_
		fseek64(fp, pos, SEEK_SET);
#else
		_fseeki64(fp, pos, SEEK_SET);
#endif
		offset = prevoffset = pos;
	}

	inline bool empty(){
		return (fp==NULL) || (feof(fp));
	}

	bool next(){
		if (empty()){
			return false;
		}
		else{
			prevoffset = offset;
			fGetData((char *)&rt, 8, fp);
			fGetData((char *)&sz, 4, fp);
			memset(msg,0,sizeof(msg));
			fGetData(msg, sz, fp);
			return true;
		}
	}

	FILE *fp;
	long long rt;
	int sz, len;
	char *msg;
	long long offset;
	long long prevoffset;

};


#endif // rawdatareader_h__
