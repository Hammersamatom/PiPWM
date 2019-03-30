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
const int tempHolderSize = 5;
const char tempPath[]    = {"/sys/class/thermal/thermal_zone0/temp"};

// Presetting some variables to avoid problems.
int tempHolder[tempHolderSize] = {tempHolderSize};
int avgTemp                    = 0;
int minTemp                    = 0;
int maxTemp                    = 0;
int avgRun                     = 0;



void setPWM(int onTime, int offTime)
{
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

    // Jank as hell way of setting the different array values 
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
    char val[4];

    ifstream temperatureFile(tempPath);

    if (!temperatureFile.is_open())
    {
        printf("/sys/ file cannot be found.");
    }

    // Read the values in the temperature file, up to a proposed length.
    temperatureFile.read(val,4);
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
	// Clear screen initially
	printf("\033c");

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

        int Difference = maxTemp - minTemp;
        float Percent  = (float)(avgTemp - minTemp)/Difference;

	int onTime = Percent * Scalar;
	int offTime = Scalar - onTime;


        printf("%f %d %d %d %d\n", Percent, avgTemp, minTemp, maxTemp, Difference);

	setPWM(onTime, offTime);
    }

    return 0;
}
