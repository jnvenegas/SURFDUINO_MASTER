#include <Arduino.h>

#ifndef DATASTRUCT
#define DATASTRUCT

class Data{

    private:

    //timestamp
    uint16_t timeStamp;
    //board orientation
    unsigned int heading;
    float rateOfTurn;
    float rollAngle;
    float pitchAngle;

    public:

    Data();
    ~Data();

    //getters
    unsigned long getTimeStamp();
    float getRateOfTurn();
    float getRollAngle();
    float getPitchAngle();
    unsigned int getHeading();

    //setters
    void setTimeStamp();
    void setRateOfTurn(float rateOfTurnValue);
    void setRollAngle(float rollAngleValue);
    void setPitchAngle(float pitchAngleValue);
    void setHeading(unsigned int headingValue);
};

#endif