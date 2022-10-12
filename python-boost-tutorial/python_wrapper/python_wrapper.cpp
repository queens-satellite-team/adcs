#include "demo.h"
#include <boost/python.hpp>

BOOST_PYTHON_MODULE(python_demo)
{
    using namespace boost::python;
    def("greet", greet);
}