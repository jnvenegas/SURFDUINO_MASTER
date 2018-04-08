#include "Imu.h"

// magnetic deviation; if it's East, it's negative 
// local adjustment set for New York City(13W NYC)
const static float magneticDeviation = 13.0;        

//helps convert raw data into compass heading
const static float radiansToDegrees = 180.0f / M_PI;       

Imu::Imu()
{}

Imu::~Imu()
{}

bool Imu::setupDevice()
{
     //initialize IMU which communicates on I2C
    Wire.begin();
    imu = RTIMU::createIMU(&settings);
    imu->IMUInit();
    imu->getCalibrationValid();
    fusion.setSlerpPower(0.02);
    fusion.setGyroEnable(true);
    fusion.setAccelEnable(true);
    fusion.setCompassEnable(true);
    return true;
}

void Imu::updateValues(Data& data)
{
    //read the raw data into the RTIMULib object
    imu->IMURead();
    fusion.newIMUData(imu->getGyro(), imu->getAccel(), imu->getCompass(), imu->getTimestamp());
    //turn raw IMU data into an actual compass heading
    RTVector3 poseData = fusion.getFusionPose();
    RTVector3 gyroData = imu->getGyro();
    float roll = poseData.y() * -1 * radiansToDegrees;        // negative is left roll
    float yawValue = poseData.z() * radiansToDegrees;         // negative is to the left of 270 magnetic
    float rateOfTurnValue = gyroData.z() * radiansToDegrees;  // negative is to left
    float compassHeadingValue = yawValue - 90;                // 0 yawValue = 270 magnetic; converts to mag degrees
    float pitchValue = poseData.x() * radiansToDegrees;
    if (yawValue < 90 && yawValue >= -179.99) {
    compassHeadingValue = yawValue + 270;
    }

    // calculate true heading that incoporates local adjustment defined at the top of this file
    float adjustedCompassHeadingValue = compassHeadingValue - magneticDeviation;         
    if (adjustedCompassHeadingValue > 360) {
    adjustedCompassHeadingValue = adjustedCompassHeadingValue - 360;
    }
    if (adjustedCompassHeadingValue < 0.0) {
    adjustedCompassHeadingValue = adjustedCompassHeadingValue + 360;
    }

    //set values in data object
    data.setHeading(adjustedCompassHeadingValue);
    data.setRateOfTurn(rateOfTurnValue);
    data.setRollAngle(roll);
    data.setPitchAngle(pitchValue);
}
