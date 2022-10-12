#include "interface.hpp"
#include <boost/python.hpp>

using namespace boost::python;

BOOST_PYTHON_MODULE(adcs_interface)
{
    class_<ADCS_device>("ADCS_device");

    class_<Actuator, bases<ADCS_device>>("Actuator", init<uint32_t>())
        .def("set_output",                      &Actuator::set_output)
        .def("sim_get_current_accelerations",   &Actuator::sim_get_current_accelerations)
        .def("sim_get_current_velocities",      &Actuator::sim_get_current_velocities);

    class_<reaction_wheel, bases<Actuator>>("reaction_wheel",  init<uint32_t>());

}
