#ifndef RANGE_QUERY_TREE_H
#define RANGE_QUERY_TREE_H
#include <vector>

template <class T>
class range_query_tree{
private:
	inline void _update(int idx, const T& val, int root, int l, int r){
		if(idx < l || r < idx) return;  
        if(l == r) {  
            arr[root] = val; 
            return;  
        }  
        int m = (l+r)/2;  
        _update(idx, val, root*2, l, m);  
        _update(idx, val, root*2+1, m+1, r);  
        arr[root] = fun(arr[root*2], arr[root*2+1]); 
	}

	inline T _read(int from, int to, int root, int l, int r) const {
		if (from>r || to<l) return DefVal;
		if (from==l && to==r) return arr[root];

		int m=(l+r)/2;
		return fun( _read(from, std::min(m,to), root*2, l, m), 
			_read(std::max(from,m+1),to, root*2+1, m+1, r));
	}

public:

	range_query_tree() : min(0), max(0), N(0) {}

	range_query_tree(int min, int max, T const & (*fun) (T const &, T const &), T DefVal )
		: min(min), max(max), N(max-min+1), fun(fun), DefVal(DefVal), arr((max-min+1)*4+1,DefVal){}

	inline T read(int from, int to) const {
		return _read(from-min+1, to-min+1, 1, 0, N-1);
	}

	inline T read(int idx) const {
		return read(idx, idx);
	}

	inline void update(int idx, T val){
		_update(idx-min+1, val, 1, 0, N-1);
	}

	int min,max,N;
	T const & (*fun) (T const &, T const &);
	T DefVal;
	std::vector<T> arr;
};



#endif // !RANGE_QUERY_TREE_H
