/*
    FILE: BD-MOVEMENT-SUITE.h
    DATA: 18/04/23
    VERSION: 0.1
    AUTHOR: Andrey Ezhkov - Budget Dynamics

    Library created for ease of use for the MECHKA robot dog
*/

#ifndef BD_MOVEMENT_SUITE_h
#define BD_MOVEMENT_SUITE_h

// the servo driver
#include<HCPCA9685.h>

class BD_MOVEMENT_SUITE
{
    public:
    BD_MOVEMENT_SUITE(HCPCA9685 HCPCA9685);
    // --- movement
    // complex movements
    void sideStep();
    void walk(int speed);
    void walkTrust(int speed);
    void walkReverse(int speed);
    // basic movements
    void standDown();
    void standUp(double targetHeight);
    void xMovement(double tX);
    void hold();
    void xTestMovement();
    void pitchTestMovement();
    // essential movements
    void inverseKinematicsZTest(int legIndex, double torsoHeight, double x);
    void inverseKinematics(int legIndex, double torsoHeight, double x);
    double cosineTheorem(int a, int b, double c);
    private:
    HCPCA9685 _HCPCA9685();
    //int servoRoms[12][12] = {{72, 180}, {22, 170}, {0, 0}, {10, 140}, {0, 180}, {0, 0}, {0, 70}, {35, 70}, {0, 0}, {10, 60}, {35, 140}, {0, 0}};
    //                      c             c           c            c           c           c           c             c
    int servoRoms[4][6] = {{40, 180, 90, 150, 35, 0}, {3, 120, 85, 135, 75, 0}, {0, 115, 90, 150, 85, 0}, {0, 115, 100, 160, 90, 0}};
    //the legs need to be closed at the start of the program for the angles to be true
    int servoCurrentAngles[4][3] = {{40, 170, 0}, {3, 156, 0}, {0, 150, 0}, {0, 180, 0}};
    int leg[4][3] = {{0, 1, 2}, {3, 4, 5}, {6, 7, 8}, {9, 10, 11}};

    int legLenght = 10;

    double x[4] = {0 ,0, 0, 0};
    double height = 1;
    double y[4] = {0 ,0, 0, 0};
    double z[4] = {0, 0, 0, 0};

};

#endif