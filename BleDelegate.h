#include <Arduino.h>
#include <Wire.h>
#include "Data.h"
#include "SlaveData.h"

#ifndef BLEDEL
#define BLEDEL

//BLE commands
static const char BLE_SAVE_DATA_ON  = 'O';
static const char BLE_SAVE_DATA_OFF = 'F';
static const char BLE_SYS_STATUS    = 'S';
static const char BLE_STATS_REPORT  = 'R';

class BleDelegate{

    private:
    
    float maxMph;
    float maxTakeOffAngle;
    float maxRideLength;

    float lastRideMaxMph;
    float lastTakeOffAngle;
    float lastRideLength;

    uint8_t rideCount;

    public:

    BleDelegate();
    ~BleDelegate();

    //updates data
    void inspectData(Data& msrData, SlaveDataStruct& slvData, uint8_t rideCount);
    void processCommand( uint8_t i2cAddr, SlaveDataStruct& slvData, bool& saveDataFlag);
    void bleResponse(char * arrayOfChars);
};

#endif
