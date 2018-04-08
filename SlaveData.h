#ifndef SLVSTRUCT
#define SLVSTRUCT
struct SlaveDataStruct
{
    //date
    char dateString[7] = {'\0','\0','\0','\0','\0','\0','\0'};
    char timeString[7] = {'\0','\0','\0','\0','\0','\0','\0'};
    //speed
    float mph;
    //position
    float latitude;
    float longitude;
    //ble command
    char bleCmd;
    //is locked flag
    boolean isGpsLocked;
};
#endif