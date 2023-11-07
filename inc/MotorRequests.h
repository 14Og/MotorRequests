#include <curl/curl.h>
#include <string>
#include <sstream>
#include <iostream>
#include <chrono>
#include <thread>
#include <cmath>
#include <unistd.h>

#define MAX_ELEVATION_ANGLE 90
#define MIN_ELEVATION_ANGLE 0
#define MAX_AZIMUTH_ANGLE 360
#define MIN_AZIMUTH_ANGLE 0


using std::string;
using std::to_string;
using std::chrono::nanoseconds;
using std::this_thread::sleep_for;



enum RequestsError : int16_t   
{
    CommandError,
    ServerError,
    Succeed
}; 

enum RequestCommands : uint8_t
{
     motors_stop = 0,
     firmware_restart,
     emergency_stop,
     set_azimuth_val,
     set_elevation_val,
     increase_azimuth_val,
     increase_elevation_val,
     decrease_azimuth_val,
     decrease_elevation_val,
     zero_azimuth,
     zero_elevation,
     relative_coordinates,
     absolute_cooridinates

}; //then put bytes-like values to call from Qt form


class MotorRequests
{

protected: 
    string _IpAddr;
    float _azimuthVal, _elevationVal;
    CURL *handler;
    string current_url;
    int16_t vel;
    int16_t acc;
    bool isAbsolute{false};

    void EndSession();
    void CreateSyncDelay(float shifting);
    inline void CreateSyncDelay(useconds_t milis) {
        usleep(milis*1000);
    }
    void ParseKinematicParams();
    RequestsError SendRequest(string &request);
    RequestsError MotorsStop();
    RequestsError FirmwareRestart();
    RequestsError EmergencyStop();
    RequestsError SetElevationVal(float position);
    RequestsError SetAzimuthVal(float position);
    RequestsError IncreaseElevationVal(float position);  //  Y - elevation
    RequestsError IncreaseAzimuthVal(float position);  //  X - azimuth
    RequestsError DecreaseAzimuthVal(float position);
    RequestsError DecreaseElevationVal(float position);
    RequestsError ZeroAzimuth();
    RequestsError ZeroElevation();
    void ChangeCoordinates(RequestCommands coordinatesType);

public:

    MotorRequests();
    MotorRequests(string &ipAddr); // should start CURL session and set motors to home position
    ~MotorRequests(); // should stop motors and end curl session
    RequestsError StartSession();
    RequestsError SetCommand(const RequestCommands command);
    RequestsError SetCommand(const RequestCommands command, const float value, bool isAbsolute);
    RequestsError GridLogging(const float elevation, const float azimuth);
    inline float GetAzVal()
    {
        return this->_azimuthVal;
    }
    inline float GetElVal()
    {
        return this->_elevationVal;
    }

    
};  // class MotorRequests