#include <curl/curl.h>
#include <string>
#include <iostream>
#include <sstream>
#include <cmath>
#include <unistd.h>

#include "jsoncons/json.hpp"

using namespace jsoncons;

#define MAX_ELEVATION_ANGLE 360
#define MIN_ELEVATION_ANGLE -360
#define MAX_AZIMUTH_ANGLE 361
#define MIN_AZIMUTH_ANGLE -361




enum RequestCommands : uint8_t {
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
};  //  enum RequestCommands


class MotorRequests {

private: 

    std::string _ipAddr{0};
    std::string _gcodeTemplate{0};
    float _azimuthVal, _elevationVal{0};
    CURL* _handler{nullptr};
    std::string _currentUrl{0};
    json _lastDumpedRequest{0};
    uint16_t _vel{0};
    uint16_t _acc{0};
    bool _isBusy{false};


    void EndSession();
    void ParseKinematicParams();
    void PrintStatus();
    void PrintError();
    void SendRequest(std::string &url, bool verbose=true);
    void MotorsStop();
    bool CheckRequest();
    template<typename ...T>
    std::string CreateUrl(const T&... strings) {
        std::stringstream stream;
        (stream << "http://" << _ipAddr << _gcodeTemplate << ... << strings);
        return stream.str();
    }
    void FirmwareRestart();
    void EmergencyStop();
    void SetElevationVal(float position);
    void SetAzimuthVal(float position);
    void IncreaseElevationVal(float position);  //  Y - elevation
    void IncreaseAzimuthVal(float position);  //  X - azimuth
    void DecreaseAzimuthVal(float position);
    void DecreaseElevationVal(float position);
    void ZeroAzimuth();
    void ZeroElevation();

    bool Probe();

public:

    MotorRequests();
    MotorRequests(std::string &ipAddr, std::string& gcodeTemplate);
    ~MotorRequests();
    void StartSession();
    void SetCommand(const RequestCommands command);
    void SetCommand(const RequestCommands command, const float value);
    void GridCalibration(const float azimuth, const float elevation);
    void GetCurrentPositionRequest();

    inline float GetAzVal() {
        return _azimuthVal;
    }   
    inline float GetElVal() {
        return _elevationVal;
    }
    inline bool GetStatus() {
        return _isBusy;
    }

};  // class MotorRequests