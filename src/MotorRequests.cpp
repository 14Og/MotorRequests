#include <fstream>
#include "MotorRequests.h"


size_t curlWriteToString (void *buffer, size_t size, size_t nmemb, void *userp) {
  ((std::string*) userp)->append ((const char*) buffer, size * nmemb);
  return size * nmemb;};

MotorRequests::MotorRequests(): _ipAddr{"10.42.0.150"}, 
                                _gcodeTemplate{":7125/printer/gcode/script?script="} 
                            {}


MotorRequests::MotorRequests(std::string& ipAddr, std::string& gcodeTemplate): _ipAddr{ipAddr},
                                                                            _gcodeTemplate{gcodeTemplate} 
                            {}


bool MotorRequests::CheckRequest() {
    return _lastDumpedRequest[0] != "error";
}

void MotorRequests::StartSession() {

    curl_global_init(CURL_GLOBAL_ALL);

    _handler = curl_easy_init();
    if (_handler == nullptr) throw std::runtime_error("Server initialization problem."); //  if no CURL initialized then stop 
    curl_easy_setopt(_handler, CURLOPT_CONNECTTIMEOUT, 10L);  //  set CURL connection timeout to connect to the server
    curl_easy_setopt(_handler, CURLOPT_TIMEOUT, 10L);  //  set CURL request timer to serve, create and implement request

    std::string start_url = CreateUrl("HOME");
    std::string kinematics_url = CreateUrl("G91");

    SendRequest(start_url, false);
    if (CheckRequest()) {
        SendRequest(kinematics_url, false); 
        ParseKinematicParams();
        GetCurrentPositionRequest();
    }
    else throw std::runtime_error("Can't home axes.\n");
}


void MotorRequests::SendRequest(std::string &url, bool verbose) {
    CURLcode res;
    std::string requestDump;
    _isBusy = true;
    if (verbose) std::cout << "URL IS: \t" << url << "\n";
    curl_easy_setopt(_handler, CURLOPT_URL, url.c_str()); 
    curl_easy_setopt(_handler, CURLOPT_WRITEDATA, &requestDump); 
    curl_easy_setopt(_handler, CURLOPT_WRITEFUNCTION, curlWriteToString);                           
    res = curl_easy_perform(_handler);
    if (res != CURLE_OK) throw std::runtime_error("CURL request error.");
    if (requestDump == "") throw std::runtime_error("Malware request");
    if (verbose) std::cout << "REQUEST DONE: \t" << url << "\n\n";
    _lastDumpedRequest = json::parse(requestDump);
    if (_lastDumpedRequest.contains("error")) PrintError();
}


void MotorRequests::ParseKinematicParams() {
    std::string request = "http://10.42.0.150/printer/objects/query?toolhead";
    SendRequest(request, false);
    _vel = _lastDumpedRequest["result"]["status"]["toolhead"]["max_velocity"].as<uint16_t>();
    _acc = _lastDumpedRequest["result"]["status"]["toolhead"]["max_accel"].as<uint16_t>();
}


void MotorRequests::EndSession() {
    if (_handler != nullptr){   
        curl_easy_cleanup(_handler);
        curl_global_cleanup();
        std::cout << "\nENDED SESSION\n";
    }
}


MotorRequests::~MotorRequests() {   
    MotorsStop();
    EndSession();
}


void MotorRequests::MotorsStop() {
    _currentUrl = CreateUrl("STOP");
    SendRequest(_currentUrl);
}


void MotorRequests::FirmwareRestart() {
    _currentUrl = CreateUrl("FIRMWARE_RESTART");
    SendRequest(_currentUrl);
}


void MotorRequests::EmergencyStop() {
    _currentUrl = CreateUrl("M112");
    SendRequest(_currentUrl);
}


void MotorRequests::IncreaseAzimuthVal(float position) {
    _currentUrl = CreateUrl("INCREASE_AZIMUTH%20", "ANGLE=", std::to_string(position));
    SendRequest(_currentUrl);

}


void MotorRequests::IncreaseElevationVal(float position) {     
    _currentUrl = CreateUrl("INCREASE_ELEVATION%20", "ANGLE=", std::to_string(position));
    SendRequest(_currentUrl);
}


void MotorRequests::DecreaseAzimuthVal(float position) {
    _currentUrl = CreateUrl("DECREASE_AZIMUTH%20", "ANGLE=", std::to_string(position));
    SendRequest(_currentUrl);
}


void MotorRequests::DecreaseElevationVal(float position) {
    _currentUrl = CreateUrl("DECREASE_ELEVATION%20", "ANGLE=", std::to_string(position));
    SendRequest(_currentUrl);
}


void MotorRequests::SetAzimuthVal(float position)
{
    _currentUrl = CreateUrl("SET_AZIMUTH%20", "ANGLE=", std::to_string(position));
    SendRequest(_currentUrl);
}


void MotorRequests::SetElevationVal(float position) {

    _currentUrl = CreateUrl("SET_ELEVATION%20", "ANGLE=", std::to_string(position));
    SendRequest(_currentUrl);
}


void MotorRequests::ZeroAzimuth() {
   return SetAzimuthVal(0);

}

void MotorRequests::ZeroElevation() {
    return SetElevationVal(0);
}


void MotorRequests::SetCommand(const RequestCommands command)
{

    switch (command)
    {
    case motors_stop:
    {
        MotorsStop();
        break;
    }
    case firmware_restart:
    {
        FirmwareRestart();
        break;
    }
    case emergency_stop:
    {
        EmergencyStop();
        break;
    }
    case zero_azimuth:
    {
        ZeroAzimuth();
        break;
    }
    case zero_elevation:
    {
        ZeroElevation();
        break;
    }
    default:
    {
        return throw std::runtime_error("Unspecified command for this funtion parameters.");
    }  
    }
    while(1) {
        GetCurrentPositionRequest();
        if(Probe()) break;
    }
}

void MotorRequests::SetCommand(const RequestCommands command, const float value)
{     

    switch (command)
    {
    case set_azimuth_val:
    {
        SetAzimuthVal(value);
        break;
    }
    case set_elevation_val:
    {
        SetElevationVal(value);
        break;
    }
    case increase_azimuth_val:
    {
        IncreaseAzimuthVal(value);
        break;
    }
    case increase_elevation_val:
    {
        IncreaseElevationVal(value);
        break;
    }
    case decrease_azimuth_val:
    {
        DecreaseAzimuthVal(value);
        break;
    }
    case decrease_elevation_val:
    {
        DecreaseElevationVal(value);
        break;
    }
    default:
    {
        throw std::runtime_error("Unspecified command for this function parameters.");
    }
    }
    while(1) {
        GetCurrentPositionRequest();
        if(Probe()) break;
    }
}


void MotorRequests::GridCalibration(const float azimuth, const float elevation) {
    _currentUrl = CreateUrl("GRID_CALIBRATION%20", "AZIMUTH=", std::to_string(azimuth), "%20ELEVATION=", std::to_string(elevation));
    SendRequest(_currentUrl);
    while(1) {
        GetCurrentPositionRequest();
        if (Probe()) break;
    }
    
}


void MotorRequests::GetCurrentPositionRequest() {
    std::string request = "http://10.42.0.150/printer/objects/query?motion_report";
    SendRequest(request, false);
    json& position = _lastDumpedRequest["result"]["status"]["motion_report"]["live_position"];   
    _azimuthVal = position[0].as<float>();
    _elevationVal = position[1].as<float>();
}


bool MotorRequests::Probe() {
    if (_lastDumpedRequest.contains("error")) return true;
    std::string probe = "http://10.42.0.150/printer/objects/query?idle_timeout";
    SendRequest(probe, false);
    std::string  status = _lastDumpedRequest["result"]["status"]["idle_timeout"]["state"].as<std::string>();
    if (status == "Ready") _isBusy = false;
    PrintStatus();
    return status == "Ready";
}


void MotorRequests::PrintStatus() {
    std::cout << "AZIMUTH: " << _azimuthVal << "\tELEVATION: " << _elevationVal << "\tSTATUS: " 
    << (_isBusy ? "BUSY\n" : "READY\n");
}


void MotorRequests::PrintError() {
    std::cout << "ERROR: " <<  _lastDumpedRequest["error"]["code"] << "\t" << _lastDumpedRequest["error"]["message"] << "\n";
}
