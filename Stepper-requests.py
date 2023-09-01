import requests
import socket
import os
import json
import sys


class CommandError(Exception):
    def __init__(self, message):
        message = f"*CommandError*:{message}"
        super().__init__(message)

class ServerError(Exception):
    def __init__(self, message):
        message = f"*ServerError*:{message}"
        super().__init__(message)


        
        
class MotorRequest:
    def __init__(self) -> None:
        self.args = sys.argv
        self.hostname = socket.gethostname()
        try:
            self.ip_addr = json.loads(os.popen("ip -j -4 route").read())[0]["prefsrc"]
        except KeyError:
            self.ip_addr = socket.gethostbyname(self.hostname)
            print("NO WLAN0 CONNECTION!")
        
    def load_json_response(self, response):
        response = json.loads(response.text)
        return response

    def process_command(self) -> str:
        """
        4 types of commands:
         -starting command
         -moving command
         -firmware restart command
         -turn motors off
        """
        if len(self.args) < 2:
            raise CommandError("NOT ENOUGH COMMANDS")
        else:
            try:
                if self.args[1] == "START":
                    self.starting_command()
                elif self.args[1] == "RESTART":
                    self.restart_command()
                elif self.args[1] == "OFF":
                    self.off_command()
                elif (self.args[1].upper() == "X" or self.args[1].upper() == "Y") and (self.args[2]== "+10" or self.args[2] == "-10"):
                    self.move_command()
                else:
                    raise CommandError("NOT ENOUGH COMMANDS")
            except IndexError:
                raise CommandError("NOT ENOUGH COMMANDS")


    def starting_command(self):
        resp = self.load_json_response(requests.post(f"http://{self.ip_addr}:7125/printer/gcode/script?script=G28 X0 Y0"))
        if not resp["result"] == "ok":
            raise ServerError("BAD SERVER REQUEST")
        else:
            resp = self.load_json_response(requests.post(f"http://{self.ip_addr}:7125/printer/gcode/script?script=G91"))
            if not resp["result"] == "ok":
                raise ServerError("BAD SERVER REQUEST")
            else:
                print("STARTING COMMAND: SUCCESSFULL")


    def restart_command(self):
        resp = self.load_json_response(requests.post(f"http://{self.ip_addr}:7125/printer/gcode/firmware_restart"))
        print(resp)

    
    def off_command(self):
        resp = self.load_json_response(requests.post(f"http://{self.ip_addr}:7125/printer/gcode/script?script=M84"))
        print(resp)

    def move_command(self):
        axis = self.args[1]
        angle = self.args[2]
        resp = self.load_json_response(requests.post(f"http://{self.ip_addr}:7125/printer/gcode/script?script=G1 {axis}{angle}"))
        print(resp)



      
if __name__ == "__main__":       
    motors = MotorRequest()
    motors.process_command()

