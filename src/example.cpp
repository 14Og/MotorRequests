#include "MotorRequests.h"




int main()
{
string ip = "192.168.0.5";
MotorRequests cbs_stand(ip);
    cbs_stand.StartSession();

    cbs_stand.SetCommand(RequestCommands::increase_azimuth_val);
    cbs_stand.SetCommand(RequestCommands::increase_elevation_val);
    cbs_stand.SetCommand(RequestCommands::decrease_azimuth_val);
    cbs_stand.SetCommand(RequestCommands::decrease_elevation_val);
    cbs_stand.SetCommand(RequestCommands::set_azimuth_val, 30);
    cbs_stand.SetCommand(RequestCommands::set_elevation_val, 130);
    cbs_stand.SetCommand(RequestCommands::firmware_restart);
return 0;
}