#ifndef rawdatareader_h__
#define rawdatareader_h__


class RawDataReader{
public:

	~RawDataReader()
	{
		delete[] msg;
		fclose(fp);
	}

	RawDataReader() : fp(0), rt(0), sz(0), len(0), msg(0) {}

	RawDataReader(const std::string& filename)
	{
		fp = 0; rt = 0; sz = 0; len = 0;
		fopen_s(&fp,filename.c_str(),"rb");
		msg = new char[3001];
	}

	inline void fGetData(char *p, int len, FILE *fp ){
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

};


#endif // rawdatareader_h__
