#include <unistd.h>
#include <fstream>
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
        this->_azimuthVal = 0;
        this->_elevationVal = 0;
        this->SendRequest(kinematics_url); // handle this request and check the result, if OK then try to send one more request
    }
    else return check_request; // if first one caused error then stop
    this->ParseKinematicParams();
    return check_request; // else return state of last request (kinematics choose)

}

RequestsError MotorRequests::SendRequest(string &url)
{
    CURLcode res;
    this->handler = curl_easy_init();
    std::cout << "url is" << url << "\n";
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


void MotorRequests::ParseKinematicParams()
{
    uint8_t vel_line = 120;
    uint8_t acc_line = 121;
    std::string vel_str;
    std::string acc_str;
    std::string line;
    std::ifstream file ("/home/pi/printer_data/config/printer.cfg");
    for (uint8_t i = 0; i < 122; i++)
    {
        getline(file, line);
        if (i == vel_line) vel_str = line;
        if (i == acc_line) acc_str = line;
    }
    this->vel = std::stoi(vel_str.substr(13, vel_str.length() -13));
    this->acc = std::stoi(acc_str.substr(10, acc_str.length() - 10));

}

void MotorRequests::CreateSyncDelay(float shifting)
{
    double velocity = this->vel * pow(10, -6);  //  deg per microsecond
    double accel = this->acc * pow(10, -12);  //  deg per microsecond / microsecond
    double delay;
    int8_t multiplexer = 3;
    if (shifting <= 100)
    {
        delay =  multiplexer * sqrt(double(shifting)/accel);
    }
    
    else
    {
        delay = multiplexer * ((pow(10, 6) + 
                    (double(shifting)-velocity)/velocity));
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
    this->MotorsStop();
    this->EndSession();
    std::cout << "DESTRUCTOR CALLED\n";
}



RequestsError MotorRequests::MotorsStop()
{
    this->current_url = "http://" + this->_IpAddr + ":7125/printer/gcode/script?script=M84";
    RequestsError ret = this->SendRequest(this->current_url);
    this->current_url = "";
    return ret;  
}

RequestsError MotorRequests::FirmwareRestart()
{
    this->current_url = "http://" + this->_IpAddr + ":7125/printer/gcode/firmware_restart";
    RequestsError ret = this->SendRequest(this->current_url);
    this->current_url = "";
    return ret;
}

RequestsError MotorRequests::EmergencyStop()
{
    this->current_url = "http://" + this->_IpAddr + ":7125/printer/gcode/script?script=M112";
    RequestsError ret = this->SendRequest(this->current_url);
    this->current_url = "";
    return ret;
}


RequestsError MotorRequests::IncreaseAzimuthVal(float position)
{   std::cout << "MOTOR REQUESTS\n"; 
    this->current_url = "http://" + this->_IpAddr + ":7125/printer/gcode/script?script=G1%20X+" + to_string(position);
    RequestsError ret = this->SendRequest(this->current_url);
    if (ret == Succeed)
    {
        this->CreateSyncDelay(abs(position));
        this->_azimuthVal += position;
    }
    this->current_url = "";
    return ret;
}



RequestsError MotorRequests::IncreaseElevationVal(float position)

{     
    this->current_url = "http://" + this->_IpAddr + ":7125/printer/gcode/script?script=G1%20Y+" + to_string(position);
    RequestsError ret = this->SendRequest(this->current_url);
    if (ret == Succeed)
    {
        this->CreateSyncDelay(abs(position));
        this->_elevationVal += position;
    }
    this->current_url = "";
    return ret;
}


RequestsError MotorRequests::DecreaseAzimuthVal(float position)
{
    this->current_url = "http://" + this->_IpAddr + ":7125/printer/gcode/script?script=G1%20X-" + to_string(position);
    RequestsError ret = this->SendRequest(this->current_url);
    if (ret == Succeed)
    {
        this->CreateSyncDelay(5);
        this->_azimuthVal -= position;
    }
    this->current_url = "";
    return ret;

}




RequestsError MotorRequests::DecreaseElevationVal(float position)
{
    this->current_url = "http://" + this->_IpAddr + ":7125/printer/gcode/script?script=G1%20Y-" + to_string(position);
    RequestsError ret = this->SendRequest(this->current_url);
    if (ret == Succeed)
    {
        this->CreateSyncDelay(5);
        this->_elevationVal -= position;
    }
    this->current_url = "";
    return ret;

}


RequestsError MotorRequests::SetAzimuthVal(float position)
{
    if (position < MIN_AZIMUTH_ANGLE | position > MAX_AZIMUTH_ANGLE) return RequestsError::CommandError;
    if (this->isAbsolute) {
        this->current_url = "http://" + this->_IpAddr + 
        ":7125/printer/gcode/script?script=G1%20X" + to_string(position);   
    }
    else {
    float rel_coord_pos = position - this->_azimuthVal;
    this->current_url = (rel_coord_pos > 0) ? "http://" + this->_IpAddr + 
    ":7125/printer/gcode/script?script=G1%20X+" + to_string(rel_coord_pos) : 
    "http://" + this->_IpAddr + ":7125/printer/gcode/script?script=G1%20X" + to_string(rel_coord_pos);
    }
    RequestsError ret = this->SendRequest(this->current_url);
    std::cout << "SENT REQUEST TO ANGLE" << position << std::endl;
    if (ret == Succeed)
    {
        this->CreateSyncDelay(abs(this->_azimuthVal - position));
        this->_azimuthVal = position;
    }
    this->current_url = "";
    return ret;
}



RequestsError MotorRequests::SetElevationVal(float position)
{

    if (position < MIN_ELEVATION_ANGLE | position > MAX_ELEVATION_ANGLE) return RequestsError::CommandError;
    if (this->isAbsolute) {
        this->current_url = "http://" + this->_IpAddr + 
        ":7125/printer/gcode/script?script=G1%20Y" + to_string(position);   
    }
    else {
    float rel_coord_pos = position - this->_elevationVal;
    this->current_url = (rel_coord_pos > 0) ? "http://" + this->_IpAddr + 
    ":7125/printer/gcode/script?script=G1%20Y+" + to_string(rel_coord_pos) : 
    "http://" + this->_IpAddr + ":7125/printer/gcode/script?script=G1%20Y" + to_string(rel_coord_pos);
    }
    RequestsError ret = this->SendRequest(this->current_url);
    if (ret == Succeed)
    {
        this->CreateSyncDelay(abs(this->_elevationVal - position));
        this->_elevationVal = position;
    }
    this->current_url = "";
    return ret;
}


RequestsError MotorRequests::ZeroAzimuth()
{
   return this->SetAzimuthVal(0);

}

RequestsError MotorRequests::ZeroElevation()
{
    return this->SetElevationVal(0);
}


void MotorRequests::ChangeCoordinates(RequestCommands coordinatesType) {
    this->current_url = coordinatesType == absolute_cooridinates ? "http://" + this->_IpAddr 
    + ":7125/printer/gcode/script?script=G90" : "http://" + this->_IpAddr 
    + ":7125/printer/gcode/script?script=G91";
    RequestsError ret = this->SendRequest(this->current_url);
    this->isAbsolute = coordinatesType == absolute_cooridinates ? true : false;
    this->current_url = "";
}

RequestsError MotorRequests::SetCommand(const RequestCommands command)
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
    case zero_azimuth:
    {
        return this->ZeroAzimuth();
        break;
    }
    case zero_elevation:
    {
        return this->ZeroElevation();
        break;
    }
    case relative_coordinates:
    {
        this->ChangeCoordinates(relative_coordinates);
        return Succeed;
        break;
    }
    case absolute_cooridinates:
    {
        this->ChangeCoordinates(absolute_cooridinates);
        return Succeed;
        break;
    }
    default:
    {
        return RequestsError::CommandError;
    }  
    }
}

RequestsError MotorRequests::SetCommand(const RequestCommands command, const float value, bool isAbsolute)
{     

    if (isAbsolute == !this->isAbsolute) {
        RequestCommands coordinates = isAbsolute ? absolute_cooridinates : relative_coordinates;
        this->ChangeCoordinates(coordinates);
    }
    switch (command)
    {
    case set_azimuth_val:
    {
        return this->SetAzimuthVal(value);
        break;
    }
    case set_elevation_val:
    {
        return this->SetElevationVal(value);
        break;
    }
    case increase_azimuth_val:
    {
        return this->IncreaseAzimuthVal(value);
        break;
    }
    case increase_elevation_val:
    {
        return this->IncreaseElevationVal(value);
    }
    case decrease_azimuth_val:
    {
        return this->DecreaseAzimuthVal(value);
    }
    case decrease_elevation_val:
    {
        return this->DecreaseElevationVal(value);
    }
    default:
    {
        return RequestsError::CommandError;
    }   
    }

}


RequestsError MotorRequests::GridLogging(const float elevation, const float azimuth) {
    RequestsError ret1 = this->SetCommand(set_elevation_val, elevation, true);
    if (ret1 != Succeed)
        return ret1;
    RequestsError ret2 = this->SetCommand(set_azimuth_val, azimuth, true);
    return ret2;
}