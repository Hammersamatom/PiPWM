#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <chrono>
#include <cmath>
#include <csignal>

#include <wiringPi.h>

using namespace std;

// Defines for pins, etc.
#define PWMpin 0

// These do not change. Scalar might be changeable to support different frequencies. 
const int Scalar  = 100;
const int Divisor = 100;

// Presetting some variables to avoid problems.
float Percent     = 100;
int onTime        = Scalar;
int offTime       = 0;


float getTemp()
{
    char *val = new char[5];

    ifstream temperatureFile("/sys/class/thermal/thermal_zone0/temp");

    if (!temperatureFile.is_open())
        cout << "/sys/ file cannot be found." << endl;

    temperatureFile.read(val, 5);
    temperatureFile.close();

    // The output of the /sys/ file is five char. We cast to float, divide until two decimal places. Round up, divide by ten to get correct value.
    return ceil(atof(val)/100)/10;
}

int main(int argc, char *argv[])
{
    if (argc < 2 || atof(argv[1]) > 100 || atof(argv[1]) < 0)
    {
        cout << "No arguments?" << endl;
        return 1;
    }

    wiringPiSetup();

    pinMode(PWMpin, OUTPUT);

    Percent    = atof(argv[1])/Divisor;
    onTime     = Scalar * Percent;
    offTime    = Scalar - onTime;

    cout << onTime << " " << offTime << endl;

    while(1)
    {
        digitalWrite(PWMpin, 1);
        this_thread::sleep_for(chrono::microseconds(onTime));
        digitalWrite(PWMpin, 0);
        this_thread::sleep_for(chrono::microseconds(offTime));
    }

    return 0;
}