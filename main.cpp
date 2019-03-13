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
const int Scalar    = 100;
const int tempHSize = 5;

// Presetting some variables to avoid problems.
int tempHolder[5] = {0};
float Percent     = 100;
int runTemp       = 0;
int minTemp       = 0;
int maxTemp       = 0;
int Difference    = 100;
int onTime        = Scalar;

void setPWM()
{
    digitalWrite(PWMpin, 1);
    this_thread::sleep_for(chrono::milliseconds(onTime));
    digitalWrite(PWMpin, 0);
    this_thread::sleep_for(chrono::milliseconds(Scalar - onTime));
}

int avgTemp(int inVal)
{
    int unDivNum = 0;


    if (tempHolder[0] == 0)
    {
        for (int i = 0; i < tempHSize; i++)
        {
            tempHolder[i] = inVal;
        }
    }

    for (int i = 0; i < tempHSize; i++)
    {
        unDivNum += tempHolder[i];
    }


    return ceil(unDivNum/tempHSize);
}

int getTemp()
{
    char *val = new char[5];

    ifstream temperatureFile("/sys/class/thermal/thermal_zone0/temp");

    if (!temperatureFile.is_open())
        cout << "/sys/ file cannot be found." << endl;

    temperatureFile.read(val, 5);
    temperatureFile.close();

    // The output of the /sys/ file is five char. We cast to float, divide until two decimal places. Round up, divide by ten to get correct value.
    return atof(val);
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

        printf("%f %d %d %d %d\n", Percent, runTemp, minTemp, maxTemp, Difference);
    }

    return 0;
}