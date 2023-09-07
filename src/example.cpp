#include "MotorRequests.h"




int main()
{
string ip = "192.168.0.5";
MotorRequests cbs_stand;
cbs_stand.StartSession();
cbs_stand.SetCommand(RequestCommands::increase_x_val);
cbs_stand.SetCommand(RequestCommands::increase_y_val);
cbs_stand.SetCommand(RequestCommands::set_x_val, 30);
return 0;
}