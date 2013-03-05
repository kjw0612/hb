#include <string>
#include <utility>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <cstdlib>

#include "tri_logger.hpp"

void finalize()
{
    TRI_LOG_STR("atexit()->finalize()->EXIT_SUCCESS");
}

template < typename T, typename U >
std::ostream & operator<< ( std::ostream & os, std::pair < T, U > const & p )
{
    os << "( " << p.first << ", " << p.second << " )";
    return os;
}

int main ()
{
    std::atexit(finalize);
    double const a = 1.0;
    std::string const str = "test";
    std::pair < std::string, double > p ( "PI", 3.1415 );
    std::pair < unsigned long, std::string > q ( 10, "EUR" );

    TRI_LOG_STR ("Trivial logger in main().");
    TRI_LOG (a);
    TRI_LOG (str);
    TRI_LOG (p);
    if ( 1 ) TRI_LOG (q); else TRI_LOG(str);

    TRI_LOG_OFF();
    TRI_LOG_STR ("Trivial logger in main().");

    TRI_LOG_ON();
    TRI_LOG_STR("Test of the TRI_LOG_FN macro.");
    TRI_LOG_FN (str);

}
