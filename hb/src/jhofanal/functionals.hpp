#ifndef functionals_h__
#define functionals_h__

template<class T>
vector<T> operator+(const vector<T>& lhs, const vector<T>& rhs) {
	assert(lhs.size()==rhs.size());
	vector<T> ret = lhs;
	for (int i=0;i<(int)ret.size();++i) ret[i] = ret[i] + rhs[i];
	return ret;
}

template<class T>
vector<T> operator+(const vector<T>& lhs, int offset) {
	assert(offset < (int)lhs.size());
	vector<T> ret(lhs.begin()+offset,lhs.end());
	return ret;
}

template<class T, class T2>
vector<T> operator-(const vector<T>& lhs, const vector<T2>& rhs) {
	assert(lhs.size()==rhs.size());
	vector<T> ret = lhs;
	for (int i=0;i<(int)ret.size();++i) ret[i] = ret[i] - rhs[i];
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

template<class T, class T2>
vector<T> batchMultip(const vector<T>& lhs, const vector<T2>& rhs) {
	assert(lhs.size()==rhs.size());
	vector<T> ret = lhs;
	for (int i=0;i<(int)ret.size();++i) ret[i] *= rhs[i];
	return ret;
}

template<class T>
vector<vector<T> > transpose(const vector<vector<T> >& rhs){
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

#endif // functionals_h__
