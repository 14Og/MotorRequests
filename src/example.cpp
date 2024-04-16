#include "MotorRequests.h"




int main()
{
std::string ip = "10.42.0.150";
std::string gcode = ":7125/printer/gcode/script?script=";
MotorRequests cbs_stand(ip, gcode);
    cbs_stand.StartSession(); 
    cbs_stand.GridCalibration(0,100);
    cbs_stand.GridCalibration(100,100);
    cbs_stand.GridCalibration(100,0);
    cbs_stand.GridCalibration(45, 45);
    cbs_stand.SetCommand(zero_elevation);
    cbs_stand.SetCommand(zero_azimuth);
    cbs_stand.SetCommand(increase_elevation_val, 360);
    cbs_stand.SetCommand(increase_elevation_val, 100);
    cbs_stand.SetCommand(home);
    return 0;
}