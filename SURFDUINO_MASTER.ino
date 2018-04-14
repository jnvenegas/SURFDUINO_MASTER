#include <SPI.h>
#include <SD.h>
#include "Imu.h"
#include "SlaveData.h"
#include "Data.h"

//size of i2c packet
#define SIZE_SLV_DATA 28
//equivalent size in SAMD M0 adalogger chip
#define SIZE_SLV_DATA_M0 32
//data object size 
uint8_t DATA_OBJ_SZ_M0 = sizeof(Data);
//buffer objects sizes
#define BUFFER_SZ 120
//slave node constants
#define I2C_SLAVE_ADDRESS 79
//SD card CS
#define SD_CHIP_SELECT 4
//we trigger logging above this speed
#define LOGGING_SPEED 5


//data from slave
SlaveDataStruct slvData;
//file
File file;
//imu sensor object (lives on the slave 
//but a direct report of the master via I2c)
Imu imuObj;
//holds IMU data
Data dataObj;
//track if we are or were recording data
bool recordMode = false;
//create file marker
bool setFileName = true;
//file name variable
String fileName;
//how many times we trigger recording
uint8_t instancesOfRecording = 0;

//buffers and related index
Data dataObjBuffer[BUFFER_SZ];
SlaveDataStruct slaveDataBuffer[BUFFER_SZ];
uint8_t bufferIdx = 0;

                        //******************************
                        bool writesd = false;
                        //******************************

//writes buffers to SD card
void writeBuffersToSD()
{
    uint8_t saveBufferIdx = 0;

    file = SD.open(fileName, FILE_WRITE);
    while(saveBufferIdx < bufferIdx)
    {
        file.print(instancesOfRecording);file.print(",");
        file.print(slaveDataBuffer[saveBufferIdx].timeString);file.print(",");
        file.print(slaveDataBuffer[saveBufferIdx].mph,4);file.print(",");
        file.print(dataObjBuffer[saveBufferIdx].getHeading());file.print(",");
        file.print(dataObjBuffer[saveBufferIdx].getRateOfTurn(),4);file.print(",");
        file.print(dataObjBuffer[saveBufferIdx].getPitchAngle(),4);file.print(",");
        file.print(dataObjBuffer[saveBufferIdx].getRollAngle(),4);file.print(",");
        file.print(slaveDataBuffer[saveBufferIdx].latitude,4);file.print(",");
        file.print(slaveDataBuffer[saveBufferIdx].longitude,4);file.println();
        saveBufferIdx++;
    }
    file.close();
    
    bufferIdx = 0;
}

//runs once
void setup() 
{
    //delay 10 seconds to allow slave node to start up before
    //trying to connect over i2c to the chips on the slave
    delay(10000);
                        //*****************************
                        //for debug
                        Serial.begin(9600);
                        //*****************************
    //begin i2c
    Wire.begin();
    //set up IMU over i2c 
    imuObj.setupDevice();
    //and start the sd card
    SD.begin(SD_CHIP_SELECT);
}

//runs forever
void loop() {
                        //*****************************
                        char readd = 'n';
                        readd = Serial.read();
                        if (readd== 'w')
                        {
                        writesd = true;
                        }
                        else if (readd== 's')
                        {
                        writesd = false;
                        }
                        //*****************************

    //get gps data over I2C
    uint8_t bytes =  Wire.requestFrom(I2C_SLAVE_ADDRESS, SIZE_SLV_DATA);
    uint8_t i2cBuffer[SIZE_SLV_DATA_M0];
    uint8_t i2cBufferIdx = 0;

    //get GPS and BLE data from slave node
    while (Wire.available())                            
    {
        i2cBuffer[i2cBufferIdx] = Wire.read();       
        i2cBufferIdx++;                                
    }
    
    //the slave is 8bit and we have to memcpy
    //each variable as the SAMD creates a 32 byte
    //memory space for the data structure
    //if you just memcpy the whole 28 bytes (slave fits the 
    //data on single word bounaries) at once we get errors
    //memcpy takes account for the padding the SAMD does 
    //to keep everything on 2 byte address boundaries
    //so this gets the data in our struct correctly
    memcpy(&slvData.dateString,i2cBuffer,7);
    memcpy(&slvData.timeString,i2cBuffer+7,7);
    memcpy(&slvData.mph,i2cBuffer+14,4);
    memcpy(&slvData.latitude,i2cBuffer+18,4);
    memcpy(&slvData.longitude,i2cBuffer+22,4);
    memcpy(&slvData.bleCmd,i2cBuffer+26,1);
    memcpy(&slvData.isGpsLocked,i2cBuffer+27,1);
   
    //read/update from the imu
    imuObj.updateValues(dataObj);   

    //once gps locked - set file name to current date
    if (setFileName && slvData.isGpsLocked)
    {   
        //filename is utc date from gps
        fileName = slvData.dateString;
        fileName = fileName + ".log";
        setFileName = false;

        //print header
        file = SD.open(fileName, FILE_WRITE);
        file.println("record_id,utc_time,mph,bearing,turn_rate,pitch_angle,roll_angle,latitude,longitude");
        file.close();
    }

                if (slvData.isGpsLocked)
                {   
                    Serial.print("L");
                }
                else
                {
                    Serial.print("U");
                }
    //if the gps indicates we are riding a wave (based on speed) then log data
    //if ( slvData.gpsLocked && (slvData.mph >= LOGGING_SPEED )) 
                             //***********************************
                            if (slvData.isGpsLocked && writesd)
                            //***********************************
    {   
        //open the file once we start recording data   
        if(recordMode == false)
        {
            instancesOfRecording++;
            //--file = SD.open(fileName, FILE_WRITE);
            recordMode = true;
        }
                            //***********************************
                            Serial.print("+");
                            //***********************************
        
        //copy iteration data to buffer
        memcpy(&dataObjBuffer[bufferIdx],&dataObj,DATA_OBJ_SZ_M0);
        memcpy(&slaveDataBuffer[bufferIdx],&slvData,SIZE_SLV_DATA_M0);
        bufferIdx++;
 
    }
   else if (recordMode)
    {
        //if we were recording a ride and now we are below the
        //trigger speed - then reset record flag and flush cache to SD
        
        //reset record flag
        recordMode = false;     
        //write buffers to SD
        writeBuffersToSD();      
    }

    //write buffers to SD if we have filled them
    if (bufferIdx == BUFFER_SZ)
    {
        writeBuffersToSD();
    }

    //0.2 second delay
    delay(200);
}
