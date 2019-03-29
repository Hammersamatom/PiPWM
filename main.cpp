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
const short Scalar         = 100;
const short tempHolderSize = 10;
const char tempPath[]    = {"/sys/class/thermal/thermal_zone0/temp"};

// Presetting some variables to avoid problems.
short tempHolder[tempHolderSize] = {tempHolderSize};
short avgTemp                    = 0;
short minTemp                    = 0;
short maxTemp                    = 0;
short avgRun                     = 0;



void setPWM(float percentIn)
{
    short onTime  = percentIn * Scalar;
    short offTime = Scalar - onTime;

    digitalWrite(PWMpin, 1);
    this_thread::sleep_for(chrono::milliseconds(onTime));
    digitalWrite(PWMpin, 0);
    this_thread::sleep_for(chrono::milliseconds(offTime));
}

short avgTheTemp(short inVal)
{
    // This number is set to 0, allows for numbers to be added and averaged 
    short unDivNum = 0;

    // Presets the entire tempHolder array to the current inVal to gurantee unDivNum !< minTemp
    if (tempHolder[0] == tempHolderSize)
    {
        for (short i = 0; i < tempHolderSize; i++)
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

    for (short i = 0; i < tempHolderSize; i++)
    {
        unDivNum += tempHolder[i];
    }

    for (short i = 0; i < tempHolderSize; i++)
    {
        printf("%d: %d\n", i, tempHolder[i]);
    }

    return ceil(unDivNum/tempHolderSize);
}

short getTemp()
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
        avgTemp = avgTheTemp(getTemp());

        if (minTemp == maxTemp)
        {
            minTemp = avgTemp;
            maxTemp = avgTemp + 1;
        }
        if (avgTemp < minTemp)
        {
            minTemp = avgTemp;
        }
        if (avgTemp > maxTemp)
        {
            maxTemp = avgTemp;
        }

        short Difference = maxTemp - minTemp;
        float Percent  = (float)(avgTemp - minTemp)/Difference;

        printf("%f %d %d %d %d\n", Percent, avgTemp, minTemp, maxTemp, Difference);
        setPWM(Percent);

        printf("\033c");
    }

    return 0;
}