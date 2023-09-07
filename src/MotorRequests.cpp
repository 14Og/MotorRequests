#include "MotorRequests.h"
#include <sstream>
#include <iostream>
#include <string>



MotorRequests::MotorRequests()
{
    this->_IpAddr = "10.42.0.150";

}


MotorRequests::MotorRequests(string &ipAddr)
{
    this->_IpAddr = ipAddr;

}


RequestsError MotorRequests::StartSession()
{

    curl_global_init(CURL_GLOBAL_ALL);
    string start_url = "http://" + this->_IpAddr 
    + ":7125/printer/gcode/script?script=G28%20X0%20Y0";
    string kinematics_url = "http://" + this->_IpAddr 
    + ":7125/printer/gcode/script?script=G91";
    this->temp_sync_url = "http://" + this->_IpAddr + ":7125/printer/gcode/script?script=M114";
    RequestsError check_request = this->SendRequest(start_url); // else try to send first request
    if (check_request == Succeed)
    {
        this->_Xval = 0;
        this->_Yval = 0;
        this->SendRequest(kinematics_url); // handle this request and check the result, if OK then try to send one more request
    }
    else return check_request; // if first one caused error then stop
    return check_request; // else return state of last request (kinematics choose)

}

RequestsError MotorRequests::SendRequest(string &url)
{
    CURLcode res;
    this->handler = curl_easy_init();
    if (this->handler == nullptr) return RequestsError::ServerError; //if no CURL initialized then stop 
    curl_easy_setopt(this->handler, CURLOPT_CONNECTTIMEOUT, 10L);
    curl_easy_setopt(this->handler, CURLOPT_TIMEOUT, 10L);
    curl_easy_setopt(this->handler, CURLOPT_URL, url.c_str());                                
    res = curl_easy_perform(this->handler);

    if (res != CURLE_OK) return RequestsError::CommandError;
    std::cout << "\n MAIN REQUEST MADE: \t" + url + "\n";
    curl_easy_setopt(this->handler, CURLOPT_URL, this->temp_sync_url.c_str());
    res = curl_easy_perform(this->handler);
    if (res != CURLE_OK) return RequestsError::CommandError;
    std::cout << "\nSYNC REQUEST MADE\n";
    curl_easy_cleanup(this->handler);
    return RequestsError::Succeed;  

}

void MotorRequests::EndSession()
{
    if (this->handler != nullptr)
    {
        curl_global_cleanup();
        std::cout << "\nENDED SESSION\n";
    }
}

MotorRequests::~MotorRequests()
{   
    // this->MotorsStop();
    this->EndSession();
    std::cout << "DESTRUCTOR CALLED\n";
}



RequestsError MotorRequests::MotorsStop()
{
    this->current_url = "http://" + this->_IpAddr + ":7125/printer/gcode/script?script=M84";
    RequestsError ret = this->SendRequest(this->current_url);
    if (ret == Succeed) this->current_url = "";
    return ret;  
}

RequestsError MotorRequests::FirmwareRestart()
{
    this->current_url = "http://" + this->_IpAddr + ":7125/printer/gcode/firmware_restart";
    RequestsError ret = this->SendRequest(this->current_url);
    if (ret == Succeed) this->current_url = "";
    return ret;
}

RequestsError MotorRequests::EmergencyStop()
{
    this->current_url = "http://" + this->_IpAddr + ":7125/printer/gcode/script?script=M112";
    RequestsError ret = this->SendRequest(this->current_url);
    if (ret == Succeed) this->current_url = "";
    return ret;
}

RequestsError MotorRequests::IncreaseXval()
{
    this->current_url = "http://" + this->_IpAddr + ":7125/printer/gcode/script?script=G1%20X+10";
    RequestsError ret = this->SendRequest(this->current_url);
    if (ret == Succeed)
    {
        this->_Xval += 10;
        this->current_url = "";
    }
    return ret;
}

RequestsError MotorRequests::IncreaseYval()
{
    this->current_url = "http://" + this->_IpAddr + ":7125/printer/gcode/script?script=G1%20Y+10";
    RequestsError ret = this->SendRequest(this->current_url);
    if (ret == Succeed)
    {
        this->_Yval += 10;
        this->current_url = "";
    }
    return ret;
}

RequestsError MotorRequests::DecreaseXval()
{
    this->current_url = "http://" + this->_IpAddr + ":7125/printer/gcode/script?script=G1%20X-10";
    RequestsError ret = this->SendRequest(this->current_url);
    if (ret == Succeed)
    {
        this->_Yval -= 10;
        this->current_url = "";
    }
    return ret;

}

RequestsError MotorRequests::DecreaseYval()
{
    this->current_url = "http://" + this->_IpAddr + ":7125/printer/gcode/script?script=G1%20Y-10";
    RequestsError ret = this->SendRequest(this->current_url);
    if (ret == Succeed)
    {
        this->_Yval -= 10;
        this->current_url = "";
    }
    return ret;

}


RequestsError MotorRequests::SetXval(int8_t position)
{
    if (position < MIN_ANGLE | position > MAX_ANGLE) return RequestsError::CommandError;
    int8_t rel_coord_pos = position - this->_Xval;
    this->current_url = (rel_coord_pos > 0) ? "http://" + this->_IpAddr + 
    ":7125/printer/gcode/script?script=G1%20X+" + to_string(static_cast<int>(rel_coord_pos)) : 
    "http://" + this->_IpAddr + ":7125/printer/gcode/script?script=G1%20X" + to_string(static_cast<int>(rel_coord_pos));
    RequestsError ret = this->SendRequest(this->current_url);
    if (ret == Succeed)
    {
        this->_Xval = position;
        this->current_url = "";
    }
    return ret;
}

RequestsError MotorRequests::SetYval(int8_t position)
{
    if (position < MIN_ANGLE | position > MAX_ANGLE) return RequestsError::CommandError;
    int8_t rel_coord_pos = position - this->_Yval;
    this->current_url = (rel_coord_pos > 0) ? "http://" + this->_IpAddr + 
    ":7125/printer/gcode/script?script=G1%20Y+" + to_string(static_cast<int>(rel_coord_pos)) : 
    "http://" + this->_IpAddr + ":7125/printer/gcode/script?script=G1%20Y" + to_string(static_cast<int>(rel_coord_pos));
    RequestsError ret = this->SendRequest(this->current_url);
    if (ret == Succeed)
    {
        this->_Yval = position;
        this->current_url = "";
    }
    return ret;
}

RequestsError MotorRequests::SetCommand(RequestCommands command)
{
    switch (command)
    {
    case motors_stop:
    {
        return this->MotorsStop();
        break;
    }
    case firmware_restart:
    {
        return this->FirmwareRestart();
        break;
    }
    case emergency_stop:
    {
        return this->EmergencyStop();
        break;
    }

    case increase_x_val:
    {
        return this->IncreaseXval();
        break;
    }
    
    case increase_y_val:
    {
        return this->IncreaseYval();
        break;
    }

    case decrease_x_val:
    {
        return this->DecreaseXval();
        break;
    }

    case decrease_y_val:
    {
        return this->DecreaseYval();
        break;
    }
    default:
    {

        return RequestsError::CommandError;
    }  
    }
}

RequestsError MotorRequests::SetCommand(RequestCommands command, int8_t position)
{
    switch (command)
    {
    case set_x_val:
    {
        return this->SetXval(position);
        break;
    }
    case set_y_val:
    {
        return this->SetYval(position);
        break;
    }
    default:
    {
        return RequestsError::CommandError;
    }   
    }
}



