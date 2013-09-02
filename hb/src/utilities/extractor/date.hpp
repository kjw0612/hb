#ifndef DATE_H
#define DATE_H

#include <iostream>
#include <string>

class Date
{
public:
	//  friend functions
	friend std::ostream& operator<<(std::ostream&,const Date&);
	friend std::istream& operator>>(std::istream&,Date&);
	friend Date operator+(const Date&,int);
	friend Date operator+(int,const Date&);
	//  constructors
	Date() :  month(1), day(1), year(2000) { }
	Date(int yyyy, int mm, int dd) : year(yyyy), month(mm), day(dd){ }
	Date(const std::string& yyyymmdd)
	{
		int yyyy = atoi(yyyymmdd.substr(0,4).c_str());
		int mm = atoi(yyyymmdd.substr(4,2).c_str());
		int dd = atoi(yyyymmdd.substr(6,2).c_str());
		(*this) = Date(yyyy,mm,dd);
	}
	Date(const Date &old) : month(old.month), day(old.day),
		year(old.year) { }
	const std::string str() const{
		char ret[9] = "";
		sprintf_s(ret,8,"%04d%02d%02d",year,month,day);
		return ret;
	}
	//  mutators
	void setDate(int mm,int dd,int yy=2003) { month = mm; day = dd;
	year = yy; }
	//  accessors
	void print() const;
	int end_of_month(int) const;
	//  comparison operators (accessors)
	int operator==(const Date &) const;
	int operator!=(const Date &) const;
	int operator<(const Date &) const;
	int operator<=(const Date &) const;
	int operator>(const Date &) const;
	int operator>=(const Date &) const;
private:
	int month;
	int day;
	int year;
	static int days[];
	void increment();
};


int leap_year(const int);

#endif