#ifndef IMU
#define IMU

#include "Data.h"
#include "RTIMUSettings.h"
#include "RTIMU.h"
#include "RTFusionRTQF.h"

class Imu{

    private:

    //raw imu access
    RTIMU *imu;
    //fusion object for interpreting raw data
    RTFusionRTQF fusion;
    //imu config object
    RTIMUSettings settings;

    public:

    Imu();
    ~Imu();

    //initialize the IMU device
    bool setupDevice();
    //record IMU values in the data container
    void updateValues(Data& data);
};

#endif


