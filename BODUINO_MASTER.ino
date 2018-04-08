#include <Arduino.h>
#include <SD.h>
#include <SPI.h>
#include "Imu.h"
#include "SlaveData.h"
#include "Data.h"

//size of i2c packet
#define SIZE_SLV_DATA 32

//application constants
#define LOGGING_SPEED 5
//GPS node constants
#define I2C_SLAVE_ADDRESS 79
//SD card CS
#define SD_CHIP_SELECT 4

//file handle
File file;
//flag to tell if we created a directory for the session
bool createdFile = false;
//file path for sd writing
char currentFilePath[11] = {'\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'};

//application sensor and device objects
Imu imuObj;
SlaveDataStruct slvData;
Data dataObj;

//flow control
bool runLogger = false;

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
    uint8_t dataBuffer[SIZE_SLV_DATA];
    uint8_t dataBufferIdx = 0;

        while (Wire.available()) 
        {
            dataBuffer[dataBufferIdx] = Wire.read(); 
            dataBufferIdx++;
        }
        
        memcpy(&slvData.dateString,dataBuffer,7);
        memcpy(&slvData.timeString,dataBuffer+7,7);
        memcpy(&slvData.mph,dataBuffer+14,4);
        memcpy(&slvData.latitude,dataBuffer+18,4);
        memcpy(&slvData.longitude,dataBuffer+22,4);
        memcpy(&slvData.bleCmd,dataBuffer+26,1);
        memcpy(&slvData.isGpsLocked,dataBuffer+27,1);
   

    //always read/update from the imu, keeps it's delta calculations accurate
    imuObj.updateValues(dataObj);

    //set milliseconds based timestamp
    dataObj.setTimeStamp();

    //as soon as gps locked create directory on SD for this session
    if( slvData.isGpsLocked && !createdFile)
    {
        //create file path
        sprintf(currentFilePath,"%s.log",slvData.dateString);
        //and open it
        //file = SD.open(currentFilePath,FILE_WRITE);
        //print header
        //file.println("mph,heading,rateOfTurn,roll,pitch,latitude,longitude,millis,time");  
        //flag file as created
        //createdFile = true; 
    }

    //if the gps indicates we are recordMode a wave (based on speed) then log data
    // if ( gpsLocked && (dataObjArray[dataObjArrayIdx].getMph() >= LOGGING_SPEED )) 
    if (slvData.isGpsLocked)
    {   
        //recording data
        recordMode = true;
        /*
        //record all values
        file.print(slvData.mph);
        file.print(",");
        file.print(dataObj.getHeading());
        file.print(",");
        file.print(dataObj.getRateOfTurn());
        file.print(",");
        file.print(dataObj.getRollAngle());
        file.print(",");
        file.print(dataObj.getPitchAngle());
        file.print(",");
        file.print(slvData.latitude);
        file.print(",");
        file.print(slvData.longitude);
        file.print(",");
        file.print(dataObj.getTimeStamp());
        file.print(",");
        file.print(slvData.timeString);*/

        Serial.print(slvData.mph);
        Serial.print(",");
        Serial.print(dataObj.getHeading());
        Serial.print(",");
        Serial.print(dataObj.getRateOfTurn());
        Serial.print(",");
        Serial.print(dataObj.getRollAngle());
        Serial.print(",");
        Serial.print(dataObj.getPitchAngle());
        Serial.print(",");
        Serial.print(slvData.latitude);
        Serial.print(",");
        Serial.print(slvData.longitude);
        Serial.print(",");
        Serial.print(dataObj.getTimeStamp());
        Serial.print(",");
        Serial.print(slvData.bleCmd);
        Serial.print(",");
        Serial.println(slvData.timeString);
       
        
                Serial.println();

    }
    else
    {
      Serial.print(".");
        //if we were recording a ride and now we are below the
        //trigger speed - then reset record flag and update ride #
        if (recordMode)
        {
            //reset record flag
            recordMode = false;
            //commit all data to flash
            //file.close();
        }

    }
    delay(200);
}
