#include "MotorRequests.h"
#include <sstream>
#include <iostream>
#include <string>



MotorRequests::MotorRequests()
{
    this->_IpAddr = "192.168.0.5";
    this->StartSession();
}


MotorRequests::MotorRequests(string &ipAddr)
{
    this->_IpAddr = ipAddr;
    this->StartSession();
}

void MotorRequests::StartSession()
{

std::cout << "SESSION STARTED\n";
std::cout << "IP ADDRR: " + this->_IpAddr  + "\n";

}

MotorRequests::~MotorRequests()
{   
    this->MotorsStop();
    this->EndSession();
    std::cout << "DESTRUCTOR CALLED\n";
}

RequestsError MotorRequests::SetCommand(RequestCommands &command)
{
    switch (command)
    {
    case motors_stop:
    {
        this->MotorsStop();
        break;
    }
    case firmware_restart:
    {
        this->FirmwareRestart();
        break;
    }
    case emergency_stop:
    {
        this->EmergencyStop();
        break;
    }

    case increase_x_val:
    {
        this->IncreaseXval();
        break;
    }
    
    case increase_y_val:
    {
        this->IncreaseYval();
        break;
    }

    case decrease_x_val:
    {
        this->DecreaseXval();
        break;
    }

    case decrease_y_val:
    {
        this->DecreaseYval();
        break;
    }
    default:
    {
        std::cout << "CommandError";
        return RequestsError::CommandError;
    }  
    }
}

RequestsError MotorRequests::SetCommand(RequestCommands &command, int8_t value)
{
    switch (command)
    {
    case set_x_val:
    {
        this->SetXval(value);
        break;
    }
    case set_y_val:
    {
        this->SetYval(value);
        break;
    }
    default:
    {
        std::cout << "CommandError";
        return RequestsError::CommandError;
    }   
    }
}

int main()
{
string ip = "192.168.0.5";
MotorRequests test(ip);


}
