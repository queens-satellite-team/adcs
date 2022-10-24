/** @file main.cpp
 *
 *@details main file to control the adcs simulator
 *
 *@authors Lily de Loe
 *
 *Last Edited
 *2022-10-24
 *
**/
#include <cstdio>

#include <cstdio>
#include "Simulator.hpp"

int main(int argc, char **argv) {
    Simulator simulator("simulator.yaml");
    //call the control code, which will have the loop inside of it
    /*
    while(true){
      //limit the loop speed
      //control loop would also be called here
      simulator.step();
    }
    */
    return 0;
}