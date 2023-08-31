import requests
import socket
import os
import json
import sys


class CommandError(SyntaxError):
    def __init__(self, message):
        super().__init__(message)
        
        
class MotorRequest:
    def __init__(self):
        self.hostname = socket.gethostname()
        try:
            self.ip_addr = json.loads(os.popen("ip -j -4 route").read())[0]["prefsrc"]
        except KeyError:
            self.ip_addr = socket.gethostbyname(self.hostname)
        
    def process_command(self):
        """
        motor name - X or Y
        coordinates - from -90 to 90  
        command - move or park (G1 or G28)
        """
        if len(sys.argv) < 4:
            raise CommandError("NOT ENOUGH COMMANDS PASSED TO THE SCRIPT")
        
        if sys.argv[1] == "M":
            self.command = "G1"
        elif sys.argv[1] == "P":
            self.command ="G28"
        else:
            raise CommandError("WRONG MOVING COMMAND PASSED TO THE SCRIPT")
        
        if sys.argv[2] == "X" or sys.argv[2] == "Y":
            self.motor_name = sys.argv[2]
        else:
            raise CommandError("WRONG MOTOR COMMAND")
        
        if int(sys.argv[3]) > -90 and int(sys.argv[3]) <= 0:
            self.coordinates = sys.argv[3]
        elif int(sys.argv[3]) > 0 and int(sys.argv[3]) < 90:
            self.coordinates = "+" + sys.argv[3]
        else:
            raise CommandError("WRONG COORDINATES TYPE PASSED TO THE SCRIPT")
        
        return f"motor name : {self.motor_name}\ncoordinates: {self.coordinates}\ncommand: {self.command}"


    def create_request(self):
        self.request = f"http://{self.ip_addr}:7125/printer/gcode/script?script={self.command} {self.motor_name}{self.coordinates}"

    
    def send_request(self):
        r = requests.post(self.request)
        r = json.loads(r.text)
        print(r)

           
if __name__ == "__main__":       

    motors = MotorRequest()
    print(motors.process_command())
    motors.create_request()
    motors.send_request()