#ifndef macros_h__
#define macros_h__

long long atoll_len(const void *from, int len){
	char buf[30] = "";
	memcpy(buf, from, len);
	return _atoi64(buf);
}

int atoi_len(const void *from, int len){
	char buf[20] = "";
	memcpy(buf, from, len);
	return atoi(buf);
}

double atof_len(const void *from, int len){
	char buf[20] = "";
	memcpy(buf, from, len);
	return atof(buf);
}


#define COPY_STR(dest, src) memcpy(dest, src, sizeof(src))
#define ATOI_LEN(src) atoi_len(src, sizeof(src))
#define ATOF_LEN(src) atof_len(src, sizeof(src))
#define ATOLL_LEN(src) atoll_len(src, sizeof(src))

#endif // macros_h__
