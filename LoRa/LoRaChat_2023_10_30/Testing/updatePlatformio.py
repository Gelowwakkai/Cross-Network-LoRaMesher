from platformio import util
import os
import threading
import subprocess
import time
import colorama
from colorama import Fore

# PlatfromIO configuration
"""envPort = {
    "COM12": "ttgo-lora32-v1",
    "COM14": "ttgo-lora32-v1",
    "COM32": "ttgo-lora32-v1",
}"""
envPort = {
    "COM12": "ttgo-lora32-v1",
    "COM14": "ttgo-lora32-v1",
    "COM32": "ttgo-t-beam"
}

class PortsPlatformIo:
    def printPorts():
        ports = util.get_serial_ports()

        for port in ports:
            print(port["port"], end=",", flush=True)

        print()


class UpdatePlatformIO:
    def __init__(self, directory, shared_state_change, shared_state):
        self.shared_state_change = shared_state_change

        self.shared_state = shared_state

        # Get all the serial ports
        ports = util.get_serial_ports()

        for port in ports:
            print(port["port"], end=",", flush=True)

        print()

        # Generate an empty list of processes
        self.processes = []

        self.file = os.path.join(directory, "Monitoring")

        os.makedirs(self.file, exist_ok=True)

        # Start the build
        if not self.build():
            shared_state["error"] = True
            shared_state["error_message"] = "Build failed"
            shared_state_change.set()
            return

        shared_state["builded"] = True
        shared_state_change.set()

        # Spawn a thread for each port
        self.spawnThreads()

    def build(self):
        print(Fore.LIGHTGREEN_EX + "Building" + Fore.RESET, end="", flush=True)

        # Get the unique environments
        environments = set(envPort.values())

        buildThreads = []

        # Build the environments
        for env in environments:
            file = os.path.join(self.file, "build" + env + ".txt")

            buildThread = threading.Thread(
                target=self.buildEnv,
                args=(
                    env,
                    file,
                ),
            )

            buildThread.start()
            buildThreads.append(buildThread)

        # Wait for all the threads to finish
        for buildThread in buildThreads:
            buildThread.join()

        # Print
        print(Fore.GREEN + "Successfully builded" + Fore.RESET)

        return True

    def buildEnv(self, env, fileName):
        print("Start building env: " + env)

        process = subprocess.Popen(
            ["pio", "run", "-e", env],
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
        )

        self.processes.append(process)

        i = 8

        # Check if one of the lines contains an error
        for line in process.stdout:
            decoded = line.decode("utf-8", "ignore")
            with open(fileName, "a") as f:
                f.write(decoded)

            if "[FAILED]" in decoded or "error occurred" in decoded:
                print(Fore.RED + "Build failed" + Fore.RESET)
                self.shared_state["error"] = True
                self.shared_state["error_message"] = "Build failed"
                self.shared_state_change.set()
                process.kill()
                return False

            if i == 0:
                print(Fore.LIGHTGREEN_EX + "." + Fore.RESET, end="", flush=True)
                i = 8

            i -= 1

        # Print without newline character
        print()

        process.wait()

        # Remove the process from the list
        self.processes.remove(process)

    def spawnThreads(self):
        # Get all the serial ports
        ports = util.get_serial_ports()

        if len(ports) == 0:
            print(Fore.RED + "No serial ports found" + Fore.RESET)
            self.shared_state["error"] = True
            self.shared_state["error_message"] = "No serial ports found"
            self.shared_state_change.set()
            return

        # Upload to all the serial ports
        for port in ports:
            if port["port"] not in envPort:
                print(
                    Fore.YELLOW
                    + "Port "
                    + port["port"]
                    + " not in the dictionary"
                    + Fore.RESET
                )
                continue

            # Spawn a thread for each port
            x = threading.Thread(
                target=self.uploadAndMonitor,
                args=(envPort[port["port"]], port["port"]),
            )

            x.start()

    def uploadToPort(self, env, portName):
        print("Start uploading to port: " + portName + ", env: " + env)
        process = subprocess.Popen(
            ["pio", "run", "-e", env, "--target", "upload", "--upload-port", portName],
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
        )

        self.processes.append(process)

        i = 8

        # Check if one of the lines contains an error
        for line in process.stdout:
            decoded = line.decode("utf-8", "ignore")
            if "[FAILED]" in decoded or "error occurred" in decoded:
                print("Error in port: " + portName)
                self.shared_state["error"] = True
                self.shared_state["error_message"] = portName + " failed to upload"
                self.shared_state_change.set()
                process.kill()
                return False

            if i == 0:
                print(".", end="", flush=True)
                i = 8

            i -= 1

        # Print with newline character
        print()

        # Wait for the process to finish or timeout
        process.wait()

        # Remove the process from the list
        self.processes.remove(process)

        # Print
        print(Fore.GREEN + "Successfully uploaded to port: " + portName + Fore.RESET)

        return True

    def monitorPort(self, portName):
        print("Start monitoring port: " + portName)

        file = os.path.join(self.file, "monitor_" + portName + ".txt")

        process = subprocess.Popen(
            [
                "pio",
                "device",
                "monitor",
                "--port",
                portName,
                "-f",
                "esp32_exception_decoder",
                "-f",
                "time",
            ],
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
        )

        self.processes.append(process)

        # Write the output to a file, but I want to read the file on the fly
        self.ErrorOccurred = False
        self.deletingIn = 40  # lines

        for line in process.stdout:
            with open(file, "a", encoding="utf-8") as f:
                f.write(line.decode("utf-8", "ignore"))

            if "Guru Meditation Error" in line.decode(
                "utf-8", "ignore"
            ) or "assert failed" in line.decode("utf-8", "ignore"):
                self.ErrorOccurred = True

            if self.ErrorOccurred:
                self.deletingIn -= 1
                if self.deletingIn == 0:
                    print(Fore.RED + "Error in port: " + portName + Fore.RESET)
                    process.kill()
                    return

        process.wait()

        self.processes.remove(process)

    def uploadAndMonitor(self, env, portName):
        # Upload the code to the board
        if self.uploadToPort(env, portName):
            if self.shared_state["deviceMonitorStarted"] == False:
                self.shared_state["deviceMonitorStarted"] = True
                self.shared_state_change.set()

            # Monitor the port
            self.monitorPort(portName)

    def killThreads(self):
        print("Killing update and monitor threads")
        for process in self.processes:
            process.kill()


def getNumberOfPorts():
    return len(util.get_serial_ports())
