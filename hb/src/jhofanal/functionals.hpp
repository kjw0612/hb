#ifndef functionals_h__
#define functionals_h__

template<class T>
vector<T> operator+(const vector<T>& lhs, const vector<T>& rhs) {
	assert(lhs.size()==rhs.size());
	vector<T> ret = lhs;
	for (int i=0;i<(int)ret.size();++i) ret[i] += rhs[i];
	return ret;
}

template<class T>
vector<T> operator-(const vector<T>& lhs, const vector<T>& rhs) {
	assert(lhs.size()==rhs.size());
	vector<T> ret = lhs;
	for (int i=0;i<(int)ret.size();++i) ret[i] -= rhs[i];
	return ret;
}

template<class T, class T2>
T operator*(const vector<T>& lhs, const vector<T2>& rhs) {
	T ret = 0;
	for (int i=0;i<(int)lhs.size();++i) ret += (lhs[i] * rhs[i]);
	return ret;
}

template<class T, class T2>
vector<T> operator*(const vector<T>& lhs, const T2& rhs) {
	vector<T> ret = lhs;
	for (int i=0;i<(int)ret.size();++i) ret[i] = (T)(ret[i]*rhs);
	return ret;
}

template<class T, class T2>
vector<T> operator/(const vector<T>& lhs, const T2& rhs) {
	return lhs * ((T2)1/rhs);
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

#endif // functionals_h__
