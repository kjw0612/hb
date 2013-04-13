/*
 * archive_stream.hpp
 *
 *  Created on: 2013. 4. 9.
 *      Author: koval
 */

#ifndef ARCHIVE_STREAM_HPP_
#define ARCHIVE_STREAM_HPP_

namespace hb{
	template<class _T1, class _T2>
	struct pair
	{
		pair() {}
		pair(const _T1& __first, const _T2& __second) : first(__first),second(__second){}
		_T1 first;
		_T2 second;
	};
	template<class _T1, class _T2>
	inline hb::pair<_T1, _T2> make_pair(const _T1& __first, const _T2& __second){
		return hb::pair<_T1, _T2>(__first, __second);
	}

	class archive_stream{
	public:
		archive_stream(){}
		hb::pair<int, void *> getnext(){
			return hb::make_pair(0, 0);
		}
	};
}


#endif /* ARCHIVE_STREAM_HPP_ */
