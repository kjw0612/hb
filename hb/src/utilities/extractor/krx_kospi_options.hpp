#ifndef KRX_KOSPI_OPTIONS_HPP
#define KRX_KOSPI_OPTIONS_HPP

#include "jshash.hpp"

enum eKrxOptionsHeader{
    t_KrxOptionImpvol = 1315423911,// JSHash(,0) 
    t_KrxOptionSen = 1315423911,// JSHash(,0) 
};
inline eKrxOptionsHeader getTypeKrxOptionsHeader(const char *str){
    return (eKrxOptionsHeader) JSHash(str,0);
}


struct KrxOptionImpvol{
	char datatype[2];
	char infotype[2];
	char markettype[1];
	unsigned char calliv[10];
	unsigned char putiv[10];
	unsigned char repiv[10];
	unsigned char histv[10];
	char ff[1];
};
#endif