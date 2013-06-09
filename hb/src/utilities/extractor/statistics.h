#ifndef STATISTICS_H
#define STATISTICS_H


// See: Chan, Tony F.; Golub, Gene H.; LeVeque, Randall J. (1983). 
// Algorithms for Computing the Sample Variance: Analysis and Recommendations. 
// The American Statistician 37, 242-247.
// 
// Variance(x[1:N]):
// {
//    M[1] = x[1]
//    S[1] = 0.0
// 
//		j=2:N
//		{
//			M[j] = M[j-1] + (1/j)*(x[j]-M[j]-1)
//			S[j] = S[j-1] + ((j-1)/j)*(x[j]-M[j-1])^2
//			Variance = S[j]/j or
//			Variance = S[j]/(j-1)
//		}
// }
//    
// When the buffer of size N is full, not only points are added but points are dropped as well.
// from the window over which the variance is computed. Adaptation to a rolling window gives
// for j=N+1,...: 
//	{ 
//			M[j] = M[j-1] + (1/N)*(x[j]-x[j-N])
//			S[j] = S[j-1] + (N/(N+1))*(x[j]-M[j-1])^2 - (N/(N+1))*(x[j-N]-M[j])^2
//			Variance = S[j]/N or
//			Variance = S[j]/(N-1)
// }


template<typename Sample>
struct rolling_statistics
{
    typedef Sample result_type;

    rolling_statistics(int window_size)
		: window_size_(window_size),
		sum_(0), previous_mean_(0.0) ,	sum_of_squares_(0.0) {}

    void add(const Sample &added_sample)
    {
		sum_ += added_sample;
		rolling_window_.push_back(added_sample);
		size_t nr_samples = rolling_window_.size();
		Sample mean = 0;

        if((int)rolling_window_.size() >= window_size_) // full
        {
			Sample removed_sample = rolling_window_.front();
			sum_ -= removed_sample;
			rolling_window_.pop_front();
			mean = sum_ / static_cast<Sample>(rolling_window_.size());
			Sample weight = static_cast<Sample>(nr_samples)/static_cast<Sample>(nr_samples+1);	
			sum_of_squares_ += weight*(
										(added_sample-previous_mean_)*(added_sample-previous_mean_) -
										(removed_sample-mean)*(removed_sample-mean)
										);
			if (sum_of_squares_ < 0.0)	sum_of_squares_ = 0.0;
        }
		else
		{
			mean = sum_ / static_cast<Sample>(rolling_window_.size());
			Sample weight = static_cast<Sample>(nr_samples-1)/static_cast<Sample>(nr_samples);	
			sum_of_squares_ += weight*(added_sample-previous_mean_)*(added_sample-previous_mean_);				
		}
		previous_mean_ = mean;
    }

    result_type variance() const {
		size_t nr_samples = rolling_window_.size();
		if (nr_samples < 2) return 0;
		return sum_of_squares_/(nr_samples-1);
    }

	result_type mean() const{
		return previous_mean_;
	}

private:
	
	std::deque<Sample> rolling_window_;
	int window_size_;
	Sample sum_;
    Sample previous_mean_;
    Sample sum_of_squares_;
};


#endif // !STATISTICS_H
