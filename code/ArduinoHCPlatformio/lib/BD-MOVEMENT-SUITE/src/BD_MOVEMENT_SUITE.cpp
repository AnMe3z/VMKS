/*
    FILE: BD-MOVEMENT-SUITE.h
    DATA: 18/04/23
    VERSION: 0.1
    AUTHOR: Andrey Ezhkov - Budget Dynamics

    Library created for ease of use for the MECHKA robot dog
*/

#include "BD_MOVEMENT_SUITE.h"
#include "HCPCA9685.h"

BD_MOVEMENT_SUITE::BD_MOVEMENT_SUITE(HCPCA9685 HCPCA9685)
{
    _HCPCA9685 = HCPCA9685;
}