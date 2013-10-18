#ifndef functionals_h__
#define functionals_h__
#include "settings.hpp"

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

#endif // functionals_h__
