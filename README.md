System Control Dashboard
 
Welcome to the System Control Dashboard, a modern, user-friendly web application for remotely managing Desktop and Server devices using Wake-on-LAN (WOL) and MQTT communication. This project features a sleek, dark-themed UI with interactive elements, real-time status updates, and a detailed log panel, making it ideal for both novice and advanced users.
 
📋 Table of Contents

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

🌟 Overview
The System Control Dashboard allows you to monitor and control two devices—a Desktop and a Server—over a local network (192.168.100.0/24). Using an ESP8266 microcontroller and MQTT, the dashboard enables you to:

Check device status (Online/Offline).
Wake devices using Wake-on-LAN (WOL).
Send power commands (Shutdown, Sleep, Hibernate for Desktop; Shutdown, Sleep for Server).
View real-time logs of all actions.

The dashboard features a modern, dark-themed UI built with Tailwind CSS, enhanced by an interactive animated background and Font Awesome icons for a polished look. Admin access is required for Server controls, ensuring secure management.
✨ Features

📱 Responsive Design: Adapts seamlessly to mobile and desktop screens.
🎨 Interactive UI: Animated background with dots that respond to mouse movement, creating an engaging experience.
🔄 Real-Time Updates: Displays device status and logs instantly via MQTT.
🔒 Admin Authentication: Secure login for Server controls (default credentials: admin/admin).
⚡ Wake-on-LAN Support: Wake devices remotely with a single click.
📜 Detailed Log Panel: Tracks all actions with timestamps for easy monitoring.
🛠️ Comprehensive Guide: In-app guide with step-by-step instructions and troubleshooting tips.
🚀 Easy Deployment: Single HTML file for the frontend, with ESP8266 and Python scripts for backend control.

🎥 Demo

Coming soon: A demo video showcasing the dashboard in action!
🛠️ Prerequisites

Hardware:
ESP8266 microcontroller (e.g., NodeMCU).
Desktop and Server devices with Wake-on-LAN enabled in BIOS/UEFI.
Local network (192.168.100.0/24).


Software:
Arduino IDE for uploading the ESP8266 code.
Python 3.x with paho-mqtt, customtkinter, and CTkMessagebox libraries for the Desktop client.
Web browser (Chrome, Firefox, etc.) for the dashboard.


Network:
Wi-Fi network with SSID and password.
MQTT broker (e.g., HiveMQ Cloud) with credentials.
Devices configured with static IPs:
Desktop: 192.168.100.99
Server: 192.168.100.95
ESP8266: 192.168.100.101





📦 Installation

Clone the Repository:
git clone https://github.com/your-username/system-control-dashboard.git
cd system-control-dashboard


Set Up the ESP8266:

Install the Arduino IDE and add ESP8266 board support.
Install required libraries: ESP8266WiFi, PubSubClient, ESPping.
Open esp8266.ino in the Arduino IDE.
Update Wi-Fi credentials in the code:const char* ssid = "Your-WiFi-SSID";
const char* password = "Your-WiFi-Password";


Update MQTT broker details if using a custom broker:const char* mqttServer = "your-mqtt-broker";
const char* mqttUser = "your-mqtt-username";
const char* mqttPassword = "your-mqtt-password";


Upload the code to your ESP8266.


Set Up the Desktop Client:

Ensure Python 3.x is installed.
Install required libraries:pip install paho-mqtt customtkinter CTkMessagebox


Save the Python script as desktop_client.py.
Update MQTT broker details if necessary:MQTT_SERVER = "your-mqtt-broker"
MQTT_USER = "your-mqtt-username"
MQTT_PASS = "your-mqtt-password"


Run the script:python desktop_client.py




Host the Dashboard:

Place index.html in a web server directory (e.g., using http-server or a local server like XAMPP).
Alternatively, open index.html directly in a browser for testing (note: some browsers may restrict WebSocket connections).
Access the dashboard via the server URL or file:// path.


Configure Devices:

Enable Wake-on-LAN in the BIOS/UEFI of both Desktop and Server.
Set static IPs:
Desktop: 192.168.100.99, MAC: D8:BB:C1:CE:CF:93
Server: 192.168.100.95, MAC: 00:19:66:3D:F0:42


Ensure devices are on the same network as the ESP8266 (192.168.100.101).



📖 Usage
Getting Started

Open the dashboard in a web browser.
The Desktop section automatically checks the device's status.
Click the Guide button (❓) in the top-right corner to view the in-app guide.
To access Server controls, click the "Server (Admin)" header and log in with credentials (admin/admin).

Managing the Desktop
The Desktop section allows you to monitor and control a desktop computer.

Check Status:

Click the Refresh button (🔄) to check if the Desktop is online or offline.
Status Indicators:
🟢 White Dot: Desktop Online (shows Shutdown, Sleep, Hibernate buttons).
⚪ Gray Dot: Desktop Offline (shows Wake Up button).
⚪ Gray Dot (Waking Up...): Appears during wake-up attempts.
⚪ Gray Dot (Timeout): Appears if wake-up fails after 60 seconds.




Wake Up:

If offline, click Wake Up (⚡) to send a Wake-on-LAN signal.
Logs show progress: e.g., Sent wake-up signal to Desktop. Waiting for it to come online....


Power Commands (when online):

Shutdown (⏻): Powers off the Desktop.
Sleep (🛌): Enters low-power sleep mode.
Hibernate (❄): Saves state and powers off.
A confirmation popup appears on the Desktop (auto-confirms after 5 seconds).



Managing the Server (Admin Access)
The Server section requires admin login and offers similar controls.

Accessing Controls:

Click "Server (Admin)" to expand the section.
Enter admin/admin in the login modal.
Server controls appear after successful login.


Check Status:

Click Refresh (🔄) to check Server status.
Status Indicators:
🟢 White Dot: Server Online (shows Shutdown, Sleep buttons).
⚪ Gray Dot: Server Offline (shows Wake Up button).
⚪ Gray Dot (Waking Up...): Appears during wake-up attempts.
⚪ Gray Dot (Timeout): Appears if wake-up fails after 3 minutes.




Wake Up:

Click Wake Up (⚡) to send a Wake-on-LAN signal (may take up to 3 minutes).
Logs show progress: e.g., Server starting. 15 seconds before checking again..


Power Commands (when online):

Shutdown (⏻): Powers off the Server.
Sleep (🛌): Enters low-power sleep mode.



Using the Log Panel
The log panel (📜) at the bottom displays real-time action logs with timestamps.

Example Logs:

[06:28:03 PM] Desktop online. Reply 'off', 'sleep', or 'ht'
[06:28:10 PM] Sent wake-up signal to Server. Waiting for it to come online...
[06:28:25 PM] Timeout: Desktop did not come online in time.


Manage Logs:

Scroll: Use the scrollbar to view older logs (max height: 20rem).
Clear: Click Clear (🗑️) to reset logs (logs Logs cleared.).



🐛 Troubleshooting

Status Stuck on "Checking status...":
Refresh the page or click Refresh (🔄).
Verify internet connection and MQTT broker accessibility (wss://5239ffaebcbc49c6890527ee9c4b76e8.s1.eu.hivemq.cloud:8884/mqtt).


Wake-Up Fails with "Timeout":
Ensure Wake-on-LAN is enabled in the device's BIOS/UEFI.
Check network connectivity and ESP8266 power status.
Verify MAC and IP addresses in esp8266.ino.


Admin Login Fails:
Use admin/admin (case-sensitive).


No Device Response:
Check logs for errors like Disconnected from broker.
Refresh the page to reconnect to MQTT.


Buttons Disabled:
For Server: Log in as admin.
For Desktop: Wait for status check to complete.


ESP8266 Issues:
Check Serial Monitor for errors (baud rate: 115200).
Ensure Wi-Fi and MQTT credentials are correct.



🧑‍💻 Technical Details

Frontend (index.html):

Built with HTML, JavaScript, and Tailwind CSS.
Uses mqtt.min.js for MQTT communication.
Features an interactive canvas background with animated dots responding to mouse movement.
Responsive design with modals for admin login and guide.
Font Awesome icons for buttons (e.g., fa-desktop, fa-bolt).


ESP8266 Backend (esp8266.ino):

Runs on an ESP8266 microcontroller.
Connects to Wi-Fi and MQTT broker (HiveMQ Cloud).
Handles Wake-on-LAN via UDP magic packets.
Uses ESPping library to check device status.
Supports proxy connections for SSH and other ports.
Subscribes to MQTT topics: status/desktop, status/server, wake/desktop, wake/server, etc.


Desktop Client (desktop_client.py):

Python script using paho-mqtt and customtkinter.
Listens for commands (off, sleep, ht) on desktop/commands.
Displays a confirmation popup with 5-second auto-confirm.
Executes system commands for shutdown, sleep, or hibernate.


MQTT Communication:

Broker: wss://5239ffaebcbc49c6890527ee9c4b76e8.s1.eu.hivemq.cloud:8884/mqtt
Credentials: ESP_WOL/@PasswordMQTT404*
Topics:
status/desktop, status/server: Request status or send commands.
wake/desktop, wake/server: Trigger Wake-on-LAN.
desktop/commands/reply, server/commands/reply: Relay responses.




Timeouts:

Desktop wake-up: 60 seconds.
Server wake-up: 3 minutes.
Proxy connection timeout: 3 minutes.



🤝 Contributing
Contributions are welcome! To contribute:

Fork the repository.
Create a feature branch (git checkout -b feature/YourFeature).
Commit your changes (git commit -m 'Add YourFeature').
Push to the branch (git push origin feature/YourFeature).
Open a Pull Request.

Please include tests and update documentation as needed.
📜 License
This project is licensed under the MIT License. See the LICENSE file for details.
