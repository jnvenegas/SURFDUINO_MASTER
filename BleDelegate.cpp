#include "BleDelegate.h"

BleDelegate::BleDelegate()
{
    maxMph = 0;
    maxTakeOffAngle = 0;
    maxRideLength = 0;
    lastRideMaxMph = 0;
    lastTakeOffAngle = 0;
    lastRideLength = 0;
}

BleDelegate::~BleDelegate()
{

}

void BleDelegate::inspectData(Data& msrData, SlaveDataStruct& slvData, uint8_t rideCount)
{

}

void BleDelegate::processCommand( uint8_t i2cAddr, SlaveDataStruct& slvData, bool& saveDataFlag)
{   
    char sendBuffer[100];

    if(slvData.bleCmd == BLE_SAVE_DATA_ON)
    {
        saveDataFlag = true;
        sprintf(sendBuffer, "sd:%d", saveDataFlag);
    }
    else if(slvData.bleCmd == BLE_SAVE_DATA_OFF)
    {
        saveDataFlag = false;
        sprintf(sendBuffer, "sd:%d", saveDataFlag);
    }
    else if(slvData.bleCmd == BLE_SYS_STATUS)
    {
        //power calculation
        float measuredvbat = analogRead(A7);
        measuredvbat *= 2;
        measuredvbat *= 3.3; 
        //in volts
        measuredvbat /= 1024; 
        //format response
        sprintf(sendBuffer, "sd:%d,pwr:%.1f,gps:%d", saveDataFlag,measuredvbat,slvData.isGpsLocked);
    }
    else if(slvData.bleCmd == BLE_STATS_REPORT)
    {
        sprintf(sendBuffer, "rc:%d,mmph:%d,mta:%d,mrl:%d,lmph:%d,lta:%d,lrl:%d",
                            rideCount,maxMph,maxTakeOffAngle,maxRideLength,
                            lastRideMaxMph,lastTakeOffAngle,lastRideLength);                         
    }
    else{return;}

    //mark command as completed so we dont reprocess it next loop
    slvData.bleCmd = '*';
    //prepare to send
    Wire.beginTransmission(i2cAddr);
    //write to i2c buffers
    Wire.write(sendBuffer);
    //send data
    Wire.endTransmission(); 
}
