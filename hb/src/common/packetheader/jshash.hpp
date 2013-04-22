#ifndef jshash_hpp
#define jshash_hpp
inline unsigned int JSHash(const char * str, int n)
{
    unsigned int hash = 1315423911;
    for(int i = 0; i < n; i++)
    {
        hash ^= ((hash << 5) + str[i] + (hash >> 2));
    }
    return hash;
}
#endif