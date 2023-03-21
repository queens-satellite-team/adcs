#include "DetumblingModeController.h"
#include <iostream>
#include "time.h"

float getXVel();
float getYVel();
float getZVel();

void bDot(float totAngVel, float xVel, float yVel, float zVel, float timeStep) {

    std::cout << "Hello, World!" << std::endl;

    float xDot, yDot, zDot = 0.0;
    float newXVel, newYVel, newZVel = 0.0;
    float wait = 0.0;
    float minAngVel = 0.008;
    float control[3];
    int xI, yI, zI = 350; //350 mA per magnetic dipole moment

    time_t my_time = time(NULL);

    printf("%s", ctime(&my_time));
//    while(totAngVel > minAngVel) {
//        float lastTime = 0;
//        if(time > lastTime+wait) {
//            newXVel = getXVel();
//            newYVel = getYVel();
//            newZVel = getZVel();
//        }
//        xDot = (xVel - newXVel) / timeStep;
//        yDot = (yVel - newYVel) / timeStep;
//        zDot = (zVel - newYVel) / timeStep;
//
//        control = {-xDot*xI, -yDot*yI, -zDot*zI};
//        magnetorquers(control)
//    }
}

float getXVel(){

}
