#include <curl/curl.h>
#include <string>


#define MAX_ANGLE 90
#define MIN_ANGLE -90

using std::string;
using std::to_string;

enum RequestsError : int16_t   
{
    CommandError = 100,
    ServerError = 400,
    Succeed = 200
}; 

enum RequestCommands : uint8_t
{
     motors_stop  = 1,
     firmware_restart = 0,
     emergency_stop = 112,
     set_x_val = 2,
     set_y_val = 3,
     increase_x_val = 4,
     increase_y_val = 5,
     decrease_x_val = 6,
     decrease_y_val = 7

}; //then put bytes-like values to call from Qt form


class MotorRequests
{

protected: 
    string _IpAddr;
    int8_t _Xval, _Yval;
    CURL *handler;
    string current_url;

    void EndSession();
    void CreateCommandDelay(int8_t shifting);
    RequestsError SendRequest(string &request);
    RequestsError MotorsStop();
    RequestsError FirmwareRestart();
    RequestsError EmergencyStop();
    RequestsError SetXval(int8_t position);
    RequestsError SetYval(int8_t position);
    RequestsError IncreaseXval();
    RequestsError IncreaseYval();
    RequestsError DecreaseXval();
    RequestsError DecreaseYval();


public:
    MotorRequests();
    MotorRequests(string &ipAddr); // should start CURL session and set motors to home position
    ~MotorRequests(); // should stop motors and end curl session
    RequestsError StartSession();
    RequestsError SetCommand(RequestCommands command);
    RequestsError SetCommand(RequestCommands command, int8_t value);
    int8_t GetXval()
    {
        return this->_Xval;
    }
    int8_t GetYval()
    {
        return this->_Yval;
    }

    
};  // class MotorRequests