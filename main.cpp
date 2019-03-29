#include <cstdio>
#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>
#include <cmath>
#include <csignal>

#include <wiringPi.h>

using namespace std;

// Defines for pins, etc.
#define PWMpin 0

// These do not change. Scalar might be changeable to support different frequencies. 
const int16_t Scalar         = 100;
const int16_t tempHolderSize = 10;
const char tempPath[]    = {"/sys/class/thermal/thermal_zone0/temp"};

// Presetting some variables to avoid problems.
int16_t tempHolder[tempHolderSize] = {tempHolderSize};
int16_t runTemp                    = 0;
int16_t avgTemp                    = 0;
int16_t minTemp                    = 0;
int16_t maxTemp                    = 0;
int16_t avgRun                     = 0;



void setPWM(float percentIn)
{
    int16_t onTime  = percentIn * Scalar;
    int16_t offTime = Scalar - onTime;

    digitalWrite(PWMpin, 1);
    this_thread::sleep_for(chrono::milliseconds(onTime));
    digitalWrite(PWMpin, 0);
    this_thread::sleep_for(chrono::milliseconds(offTime));
}

int16_t avgTheTemp(int16_t inVal)
{
    // This number is set to 0, allows for numbers to be added and averaged 
    int16_t unDivNum = 0;

    // Presets the entire tempHolder array to the current inVal to gurantee unDivNum !< minTemp
    if (tempHolder[0] == tempHolderSize)
    {
        for (int16_t i = 0; i < tempHolderSize; i++)
        {
            tempHolder[i] = inVal;
        }
    }
    
    tempHolder[avgRun] = inVal;
    avgRun++;
    if (avgRun == tempHolderSize)
    {
        avgRun = 0;
    }

    for (int16_t i = 0; i < tempHolderSize; i++)
    {
        unDivNum += tempHolder[i];
    }

    for (int16_t i = 0; i < tempHolderSize; i++)
    {
        printf("%d: %d\n", i, tempHolder[i]);
    }

    return ceil(unDivNum/tempHolderSize);
}

int16_t getTemp()
{
    char *val = new char[5];

    ifstream temperatureFile(tempPath);

    if (!temperatureFile.is_open())
    {
        printf("/sys/ file cannot be found.");
    }
    
    temperatureFile.read(val, 5);
    temperatureFile.close();

    // The output of the /sys/ file is five char. We cast to float, divide until two decimal places. Round up, divide by ten to get correct value.
    return atof(val)/10;
}

int main()
{
    wiringPiSetup();

    pinMode(PWMpin, OUTPUT);

    while(1)
    {
        runTemp = getTemp();

        if (minTemp == maxTemp)
        {
            minTemp = runTemp;
            maxTemp = runTemp + 1;
        }
        if (runTemp < minTemp)
        {
            minTemp = runTemp;
        }
        if (runTemp > maxTemp)
        {
            maxTemp = runTemp;
        }

        avgTemp = avgTheTemp(runTemp);

        int16_t Difference = maxTemp - minTemp;
        float Percent  = (float)(avgTemp - minTemp)/Difference;

        printf("%f %d %d %d %d %d\n", Percent, avgTemp, runTemp, minTemp, maxTemp, Difference);
        setPWM(Percent);

        printf("\033c");
    }

    return 0;
}