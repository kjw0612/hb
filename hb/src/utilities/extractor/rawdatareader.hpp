#ifndef rawdatareader_h__
#define rawdatareader_h__


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
		msg = new char[3001];
	}

	inline void fGetData(char *p, int len, FILE *fp ){
		offset += len;
		while (len -->0 && !feof(fp) ){
			*p++ = fgetc(fp);
		}
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
