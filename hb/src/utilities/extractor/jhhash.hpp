#ifndef jhhash_hpp
#define jhhash_hpp
inline long long JHHash(const char * str, int n)
{
    return (((long long)*(long long *)str) << (8-n)*8);
}
#endif