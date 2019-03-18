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
const int Scalar         = 100;
const int tempHolderSize = 10;
const char tempPath[]    = {"/sys/class/thermal/thermal_zone0/temp"};

// Presetting some variables to avoid problems.
int tempHolder[tempHolderSize] = {tempHolderSize};
int runTemp                    = 0;
int avgTemp                    = 0;
int minTemp                    = 0;
int maxTemp                    = 0;
int avgRun                     = 0;



void setPWM(float percentIn)
{
    int onTime  = percentIn * Scalar;
    int offTime = Scalar - onTime;

    digitalWrite(PWMpin, 1);
    this_thread::sleep_for(chrono::milliseconds(onTime));
    digitalWrite(PWMpin, 0);
    this_thread::sleep_for(chrono::milliseconds(offTime));
}

int avgTheTemp(int inVal)
{
    // This number is set to 0, allows for numbers to be added and averaged 
    int unDivNum = 0;

    // Presets the entire tempHolder array to the current inVal to gurantee unDivNum !< minTemp
    if (tempHolder[0] == tempHolderSize)
    {
        for (int i = 0; i < tempHolderSize; i++)
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

    for (int i = 0; i < tempHolderSize; i++)
    {
        unDivNum += tempHolder[i];
    }

    for (int i = 0; i < tempHolderSize; i++)
    {
        printf("%d: %d\n", i, tempHolder[i]);
    }

    return ceil(unDivNum/tempHolderSize);
}

int getTemp()
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
    return atof(val);
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

        int Difference = maxTemp - minTemp;
        float Percent  = (float)(avgTemp - minTemp)/Difference;

        printf("%f %d %d %d %d %d\n", Percent, avgTemp, runTemp, minTemp, maxTemp, Difference);
        setPWM(Percent);

        printf("\033c");
    }

    return 0;
}