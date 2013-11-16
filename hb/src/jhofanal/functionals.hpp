#ifndef functionals_h__
#define functionals_h__
#include "settings.hpp"

template <class Type>
class Null {
public:
	Null() {}
	operator Type() const { return std::numeric_limits<Type>::max(); }
};

template<class T>
inline vector<T> operator+(const vector<T>& lhs, const vector<T>& rhs) {
	assert(lhs.size()==rhs.size());
	vector<T> ret = lhs;
	for (int i=0;i<(int)ret.size();++i) ret[i] = ret[i] + rhs[i];
	return ret;
}

template<class T>
inline vector<T> operator+(const vector<T>& lhs, int offset) {
	assert(offset < (int)lhs.size());
	vector<T> ret(lhs.begin()+offset,lhs.end());
	return ret;
}

template<class T, class T2>
inline vector<T> operator-(const vector<T>& lhs, const vector<T2>& rhs) {
	assert(lhs.size()==rhs.size());
	vector<T> ret = lhs;
	for (int i=0;i<(int)ret.size();++i) ret[i] = ret[i] - rhs[i];
	return ret;
}

template<class T, class T2>
inline T operator*(const vector<T>& lhs, const vector<T2>& rhs) {
	T ret = 0;
	for (int i=0;i<(int)lhs.size();++i) ret += (lhs[i] * rhs[i]);
	return ret;
}

template<class T, class T2>
inline vector<T> operator*(const vector<T>& lhs, const T2& rhs) {
	vector<T> ret = lhs;
	for (int i=0;i<(int)ret.size();++i) ret[i] = (T)(ret[i]*rhs);
	return ret;
}

template<class T, class T2>
inline vector<T> operator/(const vector<T>& lhs, const T2& rhs) {
	return lhs * ((T2)1/rhs);
}

template<class T, class T2>
inline vector<T> batchMultip(const vector<T>& lhs, const vector<T2>& rhs) {
	assert(lhs.size()==rhs.size());
	vector<T> ret = lhs;
	for (int i=0;i<(int)ret.size();++i) ret[i] *= rhs[i];
	return ret;
}

template<class T>
inline vector<vector<T> > transpose(const vector<vector<T> >& rhs){
	vector<vector<T> > ret(rhs[0].size(),vector<T>(rhs.size()));
	for (int i=0;i<(int)ret.size();++i){
		for (int j=0;j<(int)ret[i].size();++j){
			ret[i][j] = rhs[j][i];
		}}
	return ret;
}

inline void functionals_test(){
	vector<int> atest_(5,3);
	vector<int> btest_ = atest_ + atest_, ctest_ = atest_ - atest_;
	vector<int> dtest_ = atest_ * 2.0, etest_ = atest_ / 2.0;
	int vecmultiply = atest_ * atest_;
}

inline unsigned int rand32(){
	const unsigned int rmax = 65536;
	unsigned int apart = rand()%rmax, bpart = rand()%rmax;
	return apart * rmax + bpart;
}

inline double logisticFunction(double val){
	return 1 / (1 + exp(-val));
}

inline double logisticDerivative(double val){
	double a = logisticFunction(val);
	return a * (1-a);
}

#define TRIM_SPACE " \t\n\v"
namespace ospace {
	inline std::string trim(std::string& s,const std::string& drop = TRIM_SPACE){
		std::string r=s.erase(s.find_last_not_of(drop)+1);
		return r.erase(0,r.find_first_not_of(drop));
	}
	inline std::string rtrim(std::string s,const std::string& drop = TRIM_SPACE){
		return s.erase(s.find_last_not_of(drop)+1);
	}
	inline std::string ltrim(std::string s,const std::string& drop = TRIM_SPACE){
		return s.erase(0,s.find_first_not_of(drop));
	}
}

inline int strcmpitr(const string& a, const string& b){
	string a_(a), b_(b);
	ospace::trim(a_), ospace::trim(b_);
	return _strcmpi(a_.c_str(), b_.c_str());
}
#undef TRIM_SPACE

inline string filepathdatestr(const string& datestr){
	string ret = "D:\\SEOUL_EX_T\\" + datestr + "\\KR4101H90008_" + datestr + ".csv";
	return ret;
}

inline string filepathdatestr_new(const string& datestr){
	string ret = "D:\\SEOUL_EX_SIG1\\" + datestr + "\\KR4101H90008_" + datestr + ".csv";
	return ret;
}


template<class T>
inline void initPtr(shared_ptr<T>& rhs){
	if (rhs==NULL)
		rhs = shared_ptr<T>(new T());
}

template<class T>
inline int sign(T rhs){
	if (rhs > 0) return 1;
	else if (rhs < 0) return -1;
	else return 0;
}

#include <Windows.h>

inline std::vector<std::string> filenames(const std::string& dirpath, int topN = -1){
	std::vector<std::string> ret;
	WIN32_FIND_DATA search_data;
	memset(&search_data, 0, sizeof(WIN32_FIND_DATA));
	HANDLE handle = FindFirstFile(dirpath.c_str(), &search_data);
	while(handle != INVALID_HANDLE_VALUE)
	{
		if (topN--==0){
			break;
		}
		ret.push_back(search_data.cFileName);
		if(FindNextFile(handle, &search_data) == FALSE)
			break;
	}
	//Close the handle after use or memory/resource leak
	FindClose(handle);
	return ret;
}

inline void check_and_create_directory(const std::string& dirpath){
	std::vector<std::string> files = filenames(dirpath + "*");
	if (files.size()==0)
		system(("mkdir " + dirpath).c_str());
}

template <class Iterator, class T2>
inline void setMinMax(Iterator& vs_begin, Iterator& vs_end, T2 mint, T2 maxt){
	T2 real_min = *std::min_element(vs_begin,vs_end);
	T2 real_max = *std::max_element(vs_begin,vs_end);
	for (Iterator it=vs_begin;it!=vs_end;++it){
		T2 val = *(it);
		if ((real_max - real_min)==0)
			val = 0;
		else{
			val = (val - real_min)/(real_max - real_min); // [0 to 1]
			val = mint * (1-val) + maxt * val; // [mint to maxt]
		}
		*(it) = val;
	}
}

#define white_space(c) ((c) == ' ' || (c) == '\t')
#define valid_digit(c) ((c) >= '0' && (c) <= '9')

inline double atofast (const char *p)
{
	int frac;
	double sign, value, scale;

	// Skip leading white space, if any.

	while (white_space(*p) ) {
		p += 1;
	}

	// Get sign, if any.

	sign = 1.0;
	if (*p == '-') {
		sign = -1.0;
		p += 1;

	} else if (*p == '+') {
		p += 1;
	}

	// Get digits before decimal point or exponent, if any.

	for (value = 0.0; valid_digit(*p); p += 1) {
		value = value * 10.0 + (*p - '0');
	}

	// Get digits after decimal point, if any.

	if (*p == '.') {
		double pow10 = 10.0;
		p += 1;
		while (valid_digit(*p)) {
			value += (*p - '0') / pow10;
			pow10 *= 10.0;
			p += 1;
		}
	}

	// Handle exponent, if any.

	frac = 0;
	scale = 1.0;
	if ((*p == 'e') || (*p == 'E')) {
		unsigned int expon;

		// Get sign of exponent, if any.

		p += 1;
		if (*p == '-') {
			frac = 1;
			p += 1;

		} else if (*p == '+') {
			p += 1;
		}

		// Get digits of exponent, if any.

		for (expon = 0; valid_digit(*p); p += 1) {
			expon = expon * 10 + (*p - '0');
		}
		if (expon > 308) expon = 308;

		// Calculate scaling factor.

		while (expon >= 50) { scale *= 1E50; expon -= 50; }
		while (expon >=  8) { scale *= 1E8;  expon -=  8; }
		while (expon >   0) { scale *= 10.0; expon -=  1; }
	}

	// Return signed and scaled floating point result.

	return sign * (frac ? (value / scale) : (value * scale));
}

inline double atofremblank(const char *p, int len)
{
	int c = 0;
	char buf[50] = "";
	for (int i=0;i<len;++i){
		if (('0' <= p[i] && p[i] <= '9') || p[i] == '.')
			buf[c++] = p[i];
	}
	return atofast(buf);
}

template <class elem_type>
set<elem_type> set_union(const set<elem_type>& lhs, const set<elem_type>& rhs)
{
	set<elem_type> ret = lhs;
	for (set<elem_type>::const_iterator it = rhs.begin();it!=rhs.end();++it)
		ret.insert(*it);
	return ret;
}

template <class elem_type>
set<elem_type> set_intersection(const set<elem_type>& lhs, const set<elem_type>& rhs)
{
	set<elem_type> ret;
	for (set<elem_type>::const_iterator it = lhs.begin();it!=lhs.end();++it){
		if (rhs.find(*it)!=rhs.end())
			ret.insert(*it);
	}
	return ret;
}

template <class elem_type>
vector<vector<elem_type> > splitvec(const vector<elem_type>& lhs, int max_buckets)
{
	vector<vector<elem_type> > ret;
	for (int i=0;i<(int)lhs.size();++i){
		if (ret.size()==0 || ret.back().size()==max_buckets)
			ret.push_back(vector<elem_type>());
		ret.back().push_back(lhs[i]);
	}
	return ret;
}

#endif // functionals_h__
