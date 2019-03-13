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

// Presetting some variables to avoid problems.
float Percent     = 100;
float runTemp     = 0;
float minTemp     = 0;
float maxTemp     = 0;
float Difference  = 100;
int onTime        = Scalar;

void setPWM()
{
    digitalWrite(PWMpin, 1);
    this_thread::sleep_for(chrono::microseconds(onTime));
    digitalWrite(PWMpin, 0);
    this_thread::sleep_for(chrono::microseconds(Scalar - onTime));
}

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

int main()
{
    wiringPiSetup();

    pinMode(PWMpin, OUTPUT);

    while(1)
    {
        runTemp = getTemp();

        if (minTemp == 0 && maxTemp == 0)
        {
            minTemp = runTemp;
            maxTemp = runTemp + 1;
        }

        if (runTemp < minTemp)
            minTemp = runTemp;
        if (runTemp > maxTemp)
            maxTemp = runTemp;

        Difference = maxTemp - minTemp;
        Percent    = (runTemp - minTemp)/Difference;
        onTime     = Scalar * Percent;
        
        setPWM();
    }

    return 0;
}