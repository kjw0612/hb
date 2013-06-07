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

const int FNV_32_PRIME = ((int)0x01000193);

int fnv_32_buf(const void *buf, size_t len, int hval)
{
    unsigned char *bp = (unsigned char *)buf;	/* start of buffer */
    unsigned char *be = bp + len;		/* beyond end of buffer */
    while (bp < be) {
		/* multiply by the 32 bit FNV magic prime mod 2^32 */
	#if defined(NO_FNV_GCC_OPTIMIZATION)
		hval *= FNV_32_PRIME;
	#else
		hval += (hval<<1) + (hval<<4) + (hval<<7) + (hval<<8) + (hval<<24);
	#endif
		/* xor the bottom with the current octet */
		hval ^= (int)*bp++;
    }
    /* return our new hash value */
    return hval;
}

#define COPY_STR(dest, src) memcpy(dest, src, sizeof(src))
#define ATOI_LEN(src) atoi_len(src, sizeof(src))
#define ATOF_LEN(src) atof_len(src, sizeof(src))
#define ATOLL_LEN(src) atoll_len(src, sizeof(src))
#define ATOHASH_LEN(src) fnv_32_buf(src, sizeof(src), 0)
#define ATOI_LEN_SIGN(sign, src) ((sign=='-' ? -1 : 1) * atoi_len(src, sizeof(src)))
#define ATOLL_LEN_SIGN(sign, src) ((sign=='-' ? -1 : 1) * atoll_len(src, sizeof(src)))
#define VECTOR_APPEND(avec, bvec) std::copy(avec.begin(),avec.end(),std::back_inserter(bvec))
#define STRCMPI_FRONT(astr, bstr) _strnicmp(astr.c_str(), bstr.c_str(), std::min(astr.length(),bstr.length()))

#endif // macros_h__
