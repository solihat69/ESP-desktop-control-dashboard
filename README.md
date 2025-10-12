System Control Dashboard
 
Welcome to the System Control Dashboard, a modern web application for remotely managing Desktop and Server devices using Wake-on-LAN (WOL) and MQTT communication. It features a sleek, dark-themed UI with real-time status updates, an interactive background, and a user-friendly log panel, making it accessible for both beginners and advanced users.
 
Table of Contents

Overview
Features
Demo
Prerequisites
Installation
Usage
Getting Started
Managing the Desktop
Managing the Server (Admin Access)
Using the Log Panel


Troubleshooting
Technical Details
Contributing
License

Overview
The System Control Dashboard enables remote monitoring and control of a Desktop and a Server on a local network (192.168.100.0/24). It uses an ESP8266 microcontroller and MQTT to:

Check device status (Online/Offline).
Wake devices using Wake-on-LAN (WOL).
Send power commands (Shutdown, Sleep, Hibernate for Desktop; Shutdown, Sleep for Server).
Display real-time action logs.

The dashboard features a modern, dark-themed UI built with Tailwind CSS, enhanced by an animated canvas background and Font Awesome icons. Server controls require admin authentication for security.
Features

Responsive Design: Works seamlessly on mobile and desktop devices.
Interactive UI: Animated background dots that respond to mouse movement.
Real-Time Updates: Instant device status and log updates via MQTT.
Admin Authentication: Secure login for Server controls (default: admin/admin).
Wake-on-LAN Support: Wake devices remotely with one click.
Detailed Log Panel: Tracks actions with timestamps.
Comprehensive Guide: In-app guide with step-by-step instructions and troubleshooting.
Easy Deployment: Single HTML file for the frontend, with ESP8266 and Python scripts for backend.

Demo
Coming soon: A demo video or GIF showcasing the dashboard in action! 
Prerequisites

Hardware:
ESP8266 microcontroller (e.g., NodeMCU).
Desktop and Server with Wake-on-LAN enabled in BIOS/UEFI.
Local network (192.168.100.0/24).


Software:
Arduino IDE for ESP8266 code.
Python 3.x with paho-mqtt, customtkinter, and CTkMessagebox libraries.
Web browser (e.g., Chrome, Firefox).


Network:
Wi-Fi network with SSID and password.
MQTT broker (e.g., HiveMQ Cloud) with credentials.
Static IPs:
Desktop: 192.168.100.99
Server: 192.168.100.95
ESP8266: 192.168.100.101





Installation

Clone the Repository:
git clone https://github.com/your-username/system-control-dashboard.git
cd system-control-dashboard


Set Up the ESP8266:

Install Arduino IDE and add ESP8266 board support.
Install libraries: ESP8266WiFi, PubSubClient, ESPping.
Open esp8266.ino in Arduino IDE.
Update Wi-Fi credentials:const char* ssid = "Your-WiFi-SSID";
const char* password = "Your-WiFi-Password";


Update MQTT broker details if needed:const char* mqttServer = "your-mqtt-broker";
const char* mqttUser = "your-mqtt-username";
const char* mqttPassword = "your-mqtt-password";


Upload the code to the ESP8266.


Set Up the Desktop Client:

Install Python 3.x and required libraries:pip install paho-mqtt customtkinter CTkMessagebox


Save the Python script as desktop_client.py.
Update MQTT broker details if needed:MQTT_SERVER = "your-mqtt-broker"
MQTT_USER = "your-mqtt-username"
MQTT_PASS = "your-mqtt-password"


Run the script:python desktop_client.py




Host the Dashboard:

Place index.html in a web server directory (e.g., using http-server or XAMPP).
Alternatively, open index.html directly in a browser (note: some browsers may restrict WebSocket connections).
Access the dashboard via the server URL or file:// path.


Configure Devices:

Enable Wake-on-LAN in BIOS/UEFI for Desktop and Server.
Set static IPs:
Desktop: 192.168.100.99, MAC: D8:BB:C1:CE:CF:93
Server: 192.168.100.95, MAC: 00:19:66:3D:F0:42


Ensure devices are on the same network as the ESP8266 (192.168.100.101).



Usage
Getting Started

Open the dashboard in a web browser.
The Desktop section automatically checks the device's status.
Click the Guide button (question mark icon) in the top-right corner to view the in-app guide.
To access Server controls, click "Server (Admin)" and log in with admin/admin.

Managing the Desktop
Control and monitor a desktop computer:

Check Status:

Click Refresh to check if the Desktop is online or offline.
Status Indicators:
White Dot: Desktop Online (shows Shutdown, Sleep, Hibernate buttons).
Gray Dot: Desktop Offline (shows Wake Up button).
Gray Dot (Waking Up...): Displayed during wake-up attempts.
Gray Dot (Timeout): Appears if wake-up fails after 60 seconds.




Wake Up:

If offline, click Wake Up to send a Wake-on-LAN signal.
Logs show: e.g., [06:28:10 PM] Sent wake-up signal to Desktop. Waiting for it to come online....


Power Commands (when online):

Shutdown: Powers off the Desktop.
Sleep: Enters low-power sleep mode.
Hibernate: Saves state and powers off.
A confirmation popup appears on the Desktop (auto-confirms after 5 seconds).



Managing the Server (Admin Access)
Control the Server with admin privileges:

Accessing Controls:

Click "Server (Admin)" to expand the section.
Enter admin/admin in the login modal.
Server controls appear after login.


Check Status:

Click Refresh to check Server status.
Status Indicators:
White Dot: Server Online (shows Shutdown, Sleep buttons).
Gray Dot: Server Offline (shows Wake Up button).
Gray Dot (Waking Up...): Displayed during wake-up attempts.
Gray Dot (Timeout): Appears if wake-up fails after 3 minutes.




Wake Up:

Click Wake Up to send a Wake-on-LAN signal (up to 3 minutes).
Logs show: e.g., [06:28:10 PM] Server starting. 15 seconds before checking again..


Power Commands (when online):

Shutdown: Powers off the Server.
Sleep: Enters low-power sleep mode.



Using the Log Panel
The log panel displays real-time action logs with timestamps:

Example Logs:

[06:28:03 PM] Desktop online. Reply 'off', 'sleep', or 'ht'
[06:28:10 PM] Sent wake-up signal to Server. Waiting for it to come online...
[06:28:25 PM] Timeout: Desktop did not come online in time.


Manage Logs:

Scroll: View older logs (max height: 20rem).
Clear: Click Clear to reset logs (logs [Init] Logs cleared.).



Troubleshooting

Status Stuck on "Checking status...":
Refresh the page or click Refresh.
Ensure internet connection and MQTT broker (wss://5239ffaebcbc49c6890527ee9c4b76e8.s1.eu.hivemq.cloud:8884/mqtt) are accessible.


Wake-Up Fails with "Timeout":
Verify Wake-on-LAN is enabled in BIOS/UEFI.
Check network and ESP8266 power status.
Confirm MAC and IP addresses in esp8266.ino.


Admin Login Fails:
Use admin/admin (case-sensitive).


No Device Response:
Check logs for errors like Disconnected from broker.
Refresh the page to reconnect.


Buttons Disabled:
For Server: Log in as admin.
For Desktop: Wait for status check to complete.


ESP8266 Issues:
Use Serial Monitor (115200 baud) to debug.
Verify Wi-Fi and MQTT credentials.



Technical Details

Frontend (index.html):

Built with HTML, JavaScript, Tailwind CSS, and mqtt.min.js.
Animated canvas background with mouse-responsive dots.
Responsive design with modals for login and guide.
Font Awesome icons (e.g., desktop, bolt).


ESP8266 Backend (esp8266.ino):

Runs on ESP8266, connects to Wi-Fi and MQTT (HiveMQ Cloud).
Sends Wake-on-LAN UDP magic packets.
Uses ESPping for status checks.
Supports proxy connections (e.g., SSH).
Subscribes to MQTT topics: status/desktop, status/server, wake/desktop, wake/server, etc.


Desktop Client (desktop_client.py):

Python script with paho-mqtt and customtkinter.
Listens on desktop/commands for off, sleep, ht.
Shows confirmation popup with 5-second auto-confirm.
Executes system commands for shutdown, sleep, or hibernate.


MQTT Setup:

Broker: wss://5239ffaebcbc49c6890527ee9c4b76e8.s1.eu.hivemq.cloud:8884/mqtt
Credentials: ESP_WOL/@PasswordMQTT404*
Topics:
status/desktop, status/server: Status and commands.
wake/desktop, wake/server: Wake-on-LAN triggers.
desktop/commands/reply, server/commands/reply: Responses.




Timeouts:

Desktop wake-up: 60 seconds.
Server wake-up: 3 minutes.
Proxy connection: 3 minutes.



Contributing
Contributions are welcome! To contribute:

Fork the repository.
Create a feature branch (git checkout -b feature/YourFeature).
Commit changes (git commit -m 'Add YourFeature').
Push to the branch (git push origin feature/YourFeature).
Open a Pull Request.

Please include tests and update documentation.
License
This project is licensed under the MIT License. See the LICENSE file for details.
