#include "Data.h"

Data::Data()
{
    timeStamp = 0;
    heading = 0;
    rateOfTurn = 0;
    rollAngle = 0;
    pitchAngle = 0;
}

Data::~Data()
{

}

//getters
unsigned long Data::getTimeStamp()
{
    return timeStamp;
}
unsigned int  Data::getHeading()
{
    return heading;
}
float  Data::getRateOfTurn()
{
    return rateOfTurn;
}
float  Data::getRollAngle()
{
    return rollAngle;
}
float  Data::getPitchAngle()
{
    return pitchAngle;
}
//setters
void Data::setTimeStamp()
{
    timeStamp = millis();
}
void Data::setHeading(unsigned int headingValue)
{
    heading = headingValue;
}
void Data::setRateOfTurn(float rateOfTurnValue)
{
    rateOfTurn = rateOfTurnValue;
}
void Data::setRollAngle(float rollAngleValue)
{
    rollAngle = rollAngleValue;
}
void Data::setPitchAngle(float pitchAngleValue)
{
    pitchAngle = pitchAngleValue;
}
