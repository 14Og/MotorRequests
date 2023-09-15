#include <unistd.h>
#include "MotorRequests.h"


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
    if (this->handler == nullptr) return RequestsError::ServerError; //  if no CURL initialized then stop 
    curl_easy_setopt(this->handler, CURLOPT_CONNECTTIMEOUT, 10L);  //  set CURL connection timeout to connect to the server
    curl_easy_setopt(this->handler, CURLOPT_TIMEOUT, 10L);  //  set CURL request timer to serve, create and implement request
    curl_easy_setopt(this->handler, CURLOPT_URL, url.c_str()); // setting up url                                
    res = curl_easy_perform(this->handler);  // create request
    if (res != CURLE_OK) return RequestsError::CommandError;  // handle result 
    std::cout << "\nREQUEST DONE: \t" + url + "\n";
    curl_easy_cleanup(this->handler);
    return RequestsError::Succeed;  

}

void MotorRequests::CreateSyncDelay(int16_t shifting)
{
    double velocity = 300 * pow(10, -6);  //  deg per nanosecond
    double accel = 30 * pow(10, -12);  //  deg per nanosecond / nanosecond
    double delay;
    int8_t multiplexer = 3;
    if (shifting <= 100)
    {
        delay =  multiplexer * sqrt(double(shifting)/accel);

    }
    
    else
    {

        delay = multiplexer * ((pow(10, 6) + (double(shifting)-velocity)/velocity));
    }
    usleep(useconds_t(delay));
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
        this->CreateSyncDelay(10);
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
        this->CreateSyncDelay(10);
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
        this->CreateSyncDelay(10);
        this->_Xval -= 10;
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
        this->CreateSyncDelay(10);
        this->_Yval -= 10;
        this->current_url = "";
    }
    return ret;

}


RequestsError MotorRequests::SetXval(int16_t position)
{
    if (position <= MIN_AZIMUTH_ANGLE | position >= MAX_AZIMUTH_ANGLE) return RequestsError::CommandError;
    int16_t rel_coord_pos = position - this->_Xval;
    this->current_url = (rel_coord_pos > 0) ? "http://" + this->_IpAddr + 
    ":7125/printer/gcode/script?script=G1%20X+" + to_string(static_cast<int>(rel_coord_pos)) : 
    "http://" + this->_IpAddr + ":7125/printer/gcode/script?script=G1%20X" + to_string(static_cast<int>(rel_coord_pos));
    RequestsError ret = this->SendRequest(this->current_url);
    if (ret == Succeed)
    {
        this->CreateSyncDelay(abs(rel_coord_pos));
        this->_Xval = position;
        this->current_url = "";
    }
    return ret;
}

RequestsError MotorRequests::SetYval(int16_t position)
{
    if (position < MIN_ELEVATION_ANGLE | position >= MAX_ELEVATION_ANLGE) return RequestsError::CommandError;
    int16_t rel_coord_pos = position - this->_Yval;
    this->current_url = (rel_coord_pos > 0) ? "http://" + this->_IpAddr + 
    ":7125/printer/gcode/script?script=G1%20Y+" + to_string(static_cast<int>(rel_coord_pos)) : 
    "http://" + this->_IpAddr + ":7125/printer/gcode/script?script=G1%20Y" + to_string(static_cast<int>(rel_coord_pos));
    RequestsError ret = this->SendRequest(this->current_url);
    if (ret == Succeed)
    {
        this->CreateSyncDelay(rel_coord_pos);
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

RequestsError MotorRequests::SetCommand(RequestCommands command, int16_t position)
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



