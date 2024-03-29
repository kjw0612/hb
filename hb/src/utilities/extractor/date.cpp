#include "date.hpp"
#include <iostream>

int leap_year(const int);

//******************************************************************//
//  initialize array that holds the number of days each month

int Date::days[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

//******************************************************************//
//  return true if the date is the last day of the month

int Date::end_of_month(int dd) const
{
	if (month == 2 && leap_year(year))
		return dd == 29;
	else
		return dd == days[month];
}

//******************************************************************//
//  move the date to the following day

void Date::increment()
{
	if (end_of_month(day) && month == 12)
	{
		day = 1;
		month = 1;
		year++;
	}
	else
		if (end_of_month(day))
		{
			day = 1;
			month++;
		}
		else
			day++;
}

//******************************************************************//
//  test whether two dates are equal

int Date::operator==(const Date& second) const
{
	if (day == second.day && month == second.month
		&& year == second.year)
		return 1;
	else
		return 0;
}

//******************************************************************//
//  test whether two dates are unequal

int Date::operator!=(const Date& second) const
{
	if (day != second.day || month != second.month
		|| year != second.year)
		return 1;
	else
		return 0;
}

//******************************************************************//
//  test whether the first date is less than the second date

int Date::operator<(const Date& second) const
{
	if (year < second.year)
		return 1;
	if (year == second.year && month < second.month)
		return 1;
	if (year == second.year && month == second.month
		&& day < second.day)
		return 1;
	return 0;
}

//******************************************************************//
//  test whether first date is less than or equal to second date

int Date::operator<=(const Date& second) const
{
	if (year < second.year)
		return 1;
	if (year == second.year && month < second.month)
		return 1;
	if (year == second.year && month == second.month
		&& day <= second.day)
		return 1;
	return 0;
}

//******************************************************************//
//  test whether the first date is greater than the second date

int Date::operator>(const Date& second) const
{
	if (year > second.year)
		return 1;
	if (year == second.year && month > second.month)
		return 1;
	if (year == second.year && month == second.month
		&& day > second.day)
		return 1;
	return 0;
}

//******************************************************************//
//  test whether first date is greater than or equal to second date

int Date::operator>=(const Date& second) const
{
	if (year > second.year)
		return 1;
	if (year == second.year && month > second.month)
		return 1;
	if (year == second.year && month == second.month
		&& day >= second.day)
		return 1;
	return 0;
}

//******************************************************************//
//  print the invoking object

void Date::print() const
{
	std::cout << month << "/" << day << "/" << year;
}

//******************************************************************//
//  determine whether the given year is a leap year

int leap_year(const int yy)
{
	if (yy % 400 == 0 || (yy % 100 != 0 && yy % 4 == 0))
		return 1;     //  leap year
	else
		return 0;     //  not a leap year
}

//******************************************************************//
//  overloaded operator << to allow output of dates with ostream object

std::ostream& operator<<(std::ostream& ostr, const Date& d)
{
	ostr << d.month << "/" << d.day << "/" << d.year;
	return ostr;
}

//******************************************************************//
//  overloaded operator >> to allow input of dates with istream object

std::istream& operator>>(std::istream& istr, Date& d)
{
	istr >> d.month;
	istr.ignore(1);
	istr >> d.day;
	istr.ignore(1);
	istr >> d.year;
	return istr;
}

//******************************************************************//
//  add a specific number of days and the invoking object

Date operator+(int numdays,const Date& dd)
{
	Date newdate = dd;
	for (int i = 1 ; i <= numdays ; i++)
		newdate.increment();
	return newdate;
}

//******************************************************************//
//  add the invoking object and a specific number of days

Date operator+(const Date& dd,int numdays)
{
	Date newdate = dd;
	for (int i = 1 ; i <= numdays ; i++)
		newdate.increment();
	return newdate;
}
