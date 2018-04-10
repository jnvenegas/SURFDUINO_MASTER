#include <SPI.h>
#include <SD.h>
#include "Imu.h"
#include "SlaveData.h"
#include "Data.h"

//size of i2c packet
#define SIZE_SLV_DATA 28
//equivalent size in SAMD M0 adalogger chip
#define SIZE_SLV_DATA_M0 32
//we trigger logging above this speed
#define LOGGING_SPEED 5
//slave node constants
#define I2C_SLAVE_ADDRESS 79
//SD card CS
#define SD_CHIP_SELECT 4

//led states
bool redLed = false;
bool greenLed = false;

//data from slave
SlaveDataStruct slvData;;
//file.
File file;
//imu sensor object (lives on the slave but a direct report of the master via I2c)
Imu imuObj;
//holds IMU data
Data dataObj;
//track if we are or were recording data
bool recordMode = false;

//runs once
void setup() 
{
    Serial.begin(9600);
    Wire.begin();
    imuObj.setupDevice();
    SD.begin(SD_CHIP_SELECT);
}

//runs forever
void loop() {

    //get gps data over I2C
    uint8_t bytes =  Wire.requestFrom(I2C_SLAVE_ADDRESS, SIZE_SLV_DATA);
    uint8_t dataBuffer[SIZE_SLV_DATA_M0];
    uint8_t dataBufferIdx = 0;

    //get 28 bytes from slave node
    while (Wire.available())                            
    {
        dataBuffer[dataBufferIdx] = Wire.read();       
        dataBufferIdx++;                                
    }
    
    //the slave is 8bit and we have to memcpy
    //each variable as the SAMD creates a 32 byte
    //memory space for the data structure
    //if you just memcpy the 28 bytes at once we get errors
    //memcpy takes account for the padding the SAMD does 
    //to keep everything on 2 byte address boundaries
    memcpy(&slvData.dateString,dataBuffer,7);
    memcpy(&slvData.timeString,dataBuffer+7,7);
    memcpy(&slvData.mph,dataBuffer+14,4);
    memcpy(&slvData.latitude,dataBuffer+18,4);
    memcpy(&slvData.longitude,dataBuffer+22,4);
    memcpy(&slvData.bleCmd,dataBuffer+26,1);
    memcpy(&slvData.isGpsLocked,dataBuffer+27,1);
   
    //read/update from the imu
    imuObj.updateValues(dataObj);                                      

    //if the gps indicates we are riding a wave (based on speed) then log data
    // if ( gpsLocked && (dataObjArray[dataObjArrayIdx].getMph() >= LOGGING_SPEED )) 
    if (slvData.isGpsLocked)
    {   
        //recording data
        recordMode = true;
        file = SD.open("abc3.txt",FILE_WRITE);
        file.print(slvData.timeString);
        file.print(",");
        file.print(slvData.mph,4);
        file.print(",");
        file.print(slvData.latitude,4);
        file.print(",");
        file.print(slvData.longitude,4);
        file.print(",");
        file.print(slvData.mph,4);
        file.print(",");
        file.print(dataObj.getHeading());
        file.print(",");
        file.print(dataObj.getRateOfTurn(),4);
        file.print(",");
        file.print(dataObj.getPitchAngle(),4);
        file.print(",");
        file.print(dataObj.getRollAngle(),4);
        file.println();
        file.close();
    }
   else
    {
        Serial.print(".");

        //if we were recording a ride and now we are below the
        //trigger speed - then reset record flag and flus cache to SD
        if (recordMode)
        {
            //reset record flag
            recordMode = false;   
        }
    }
    delay(200);
}

