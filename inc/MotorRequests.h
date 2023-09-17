#include <curl/curl.h>
#include <string>
#include <sstream>
#include <iostream>
#include <chrono>
#include <thread>
#include <cmath>

#define MAX_AZIMUTH_ANGLE 90
#define MIN_AZIMUTH_ANGLE -90
#define MAX_ELEVATION_ANLGE 360
#define MIN_ELEVATION_ANGLE 0


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
     motors_stop,
     firmware_restart,
     emergency_stop,
     set_azimuth_val,
     set_elevation_val,
     increase_azimuth_val,
     increase_elevation_val,
     decrease_azimuth_val,
     decrease_elevation_val,
     zero_azimuth,
     zero_elevation

}; //then put bytes-like values to call from Qt form


class MotorRequests
{

protected: 
    string _IpAddr;
    int16_t _azimuthVal, _elevationVal;
    CURL *handler;
    string current_url;

    void EndSession();
    void CreateSyncDelay(int16_t shifting);
    RequestsError SendRequest(string &request);
    RequestsError MotorsStop();
    RequestsError FirmwareRestart();
    RequestsError EmergencyStop();
    RequestsError SetAzimuthVal(int16_t position);
    RequestsError SetElevationVal(int16_t position);
    RequestsError IncreaseAzimuthVal();  // X - AZIMUTH
    RequestsError IncreaseElevationVal();  //  Y - ELEVATION
    RequestsError DecreaseAzimuthVal();
    RequestsError DecreaseElevationVal();
    RequestsError ZeroAzimuth();
    RequestsError ZeroElevation();


public:
    MotorRequests();
    MotorRequests(string &ipAddr); // should start CURL session and set motors to home position
    ~MotorRequests(); // should stop motors and end curl session
    RequestsError StartSession();
    RequestsError SetCommand(const RequestCommands command);
    RequestsError SetCommand(const RequestCommands command, const int16_t value);
    int16_t GetXval()
    {
        return this->_azimuthVal;
    }
    int16_t GetYval()
    {
        return this->_elevationVal;
    }

    
};  // class MotorRequests