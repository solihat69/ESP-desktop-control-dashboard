#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <WiFiUdp.h>
#include <ESPping.h>
// --- LED ---
#define LED_PIN LED_BUILTIN
bool ledActive = false; // True while a request is being processed
// --- PROXY STRUCT ---
struct Proxy {
  uint16_t listenPort;
  IPAddress targetIP;
  uint16_t targetPort;
  uint8_t* targetMac;
  WiFiServer* server;
};
// --- WIFI SETUP ---
const char* ssid = "Your-WiFi-SSID";
const char* password = "Your-WiFi-Password";
// Fixed IP
IPAddress local_IP(192, 168, 100, 101);
IPAddress gateway(192, 168, 100, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(8, 8, 8, 8);
IPAddress secondaryDNS(8, 8, 4, 4);
// --- TARGET DEVICES ---
uint8_t serverMac[6] = {0x00,0x19,0x66,0x3D,0xF0,0x42};
uint8_t desktopMac[6] = {0xD8,0xBB,0xC1,0xCE,0xCF,0x93};
IPAddress serverIP(192, 168, 100, 95);
IPAddress desktopIP(192, 168, 100, 99);
// --- MQTT SETUP ---
const int mqttPort = 8883;
const char* mqttServer = "your-mqtt-broker";
const char* mqttUser = "your-mqtt-username";
const char* mqttPassword = "your-mqtt-password";
WiFiClientSecure espClient;
PubSubClient client(espClient);
WiFiUDP Udp;
// --- WOL FUNCTION ---
bool sendWOL(uint8_t* mac) {
  uint8_t packet[102];
  for(int i=0;i<6;i++) packet[i]=0xFF;
  for(int i=1;i<=16;i++){
    for(int j=0;j<6;j++){
      packet[i*6 + j] = mac[j];
    }
  }
  if (Udp.beginPacket(IPAddress(255,255,255,255), 9) == 0) {
    Serial.println("Failed to begin UDP packet!");
    return false;
  }
  size_t written = Udp.write(packet, sizeof(packet));
  if (written != sizeof(packet)) {
    Serial.printf("Failed to write UDP: %d != %d\n", written, sizeof(packet));
    Udp.endPacket();
    return false;
  }
  if (Udp.endPacket() == 0) {
    Serial.println("Failed to end UDP packet!");
    return false;
  }
  Serial.println("Magic packet sent!");
  return true;
}
// --- CHECK IF DEVICE ONLINE ---
bool isDeviceUp(IPAddress ip) {
  return Ping.ping(ip);
}
// --- PROXY INSTANCES ---
Proxy proxies[4];
// --- HANDLE PROXY ---
void handleProxy(Proxy* p) {
  WiFiClient client = p->server->available();
  if(client) {
    digitalWrite(LED_PIN, LOW); // LED ON (activity started)
    Serial.printf("Incoming connection on port %d\n", p->listenPort);
    if(!isDeviceUp(p->targetIP)) {
      Serial.println("Target OFF → Sending WOL...");
      if (!sendWOL(p->targetMac)) {
        Serial.println("Failed to send WOL packet!");
        client.stop();
        digitalWrite(LED_PIN, HIGH);
        return;
      }
      unsigned long start = millis();
      while(!isDeviceUp(p->targetIP)) {
        delay(500);
        if(millis()-start>180000) {
          Serial.println("Timeout: Target did not start");
          client.stop();
          digitalWrite(LED_PIN, HIGH);
          return;
        }
      }
      Serial.println("Target is ONLINE!");
    }
    WiFiClient targetConn;
    if(targetConn.connect(p->targetIP, p->targetPort)) {
      Serial.println("Forwarding traffic...");
      while(client.connected() && targetConn.connected()) {
        while(client.available()) {
          targetConn.write(client.read());
          // Blink LED on each packet
          digitalWrite(LED_PIN, HIGH);
          delay(200);
          digitalWrite(LED_PIN, LOW);
          delay(200);
          digitalWrite(LED_PIN, LOW);
        }
        while(targetConn.available()) {
          client.write(targetConn.read());
          digitalWrite(LED_PIN, HIGH);
          delay(200);
          digitalWrite(LED_PIN, LOW);
          delay(200);
          digitalWrite(LED_PIN, LOW);
        }
        yield();
      }
    }
    client.stop();
    targetConn.stop();
    digitalWrite(LED_PIN, HIGH); // LED OFF when done
  }
}
// --- SAFE PUBLISH ---
bool safePublish(const char* topic, const char* payload, const char* replyTopic = nullptr, const char* errorMsg = nullptr) {
  if (client.publish(topic, payload)) {
    return true;
  } else {
    Serial.printf("Failed to publish to %s\n", topic);
    if (replyTopic && errorMsg) {
      client.publish(replyTopic, errorMsg);
    }
    return false;
  }
}
// --- MQTT CALLBACK ---
void callback(char* topic, byte* payload, unsigned int length) {
  String msg = "";
  for (unsigned int i = 0; i < length; i++) msg += (char)payload[i];
  msg.trim();
  Serial.print("MQTT Message Received → Topic: ");
  Serial.print(topic);
  Serial.print(", Payload: ");
  Serial.println(msg);
  // Start LED if not already active
  if (!ledActive) {
    digitalWrite(LED_PIN, LOW);
    ledActive = true;
  } else {
    digitalWrite(LED_PIN, HIGH);
    delay(200);
    digitalWrite(LED_PIN, LOW);
    delay(200);
    digitalWrite(LED_PIN, LOW);
  }
  auto wakeDevice = [&](IPAddress ip, uint8_t* mac, const char* replyTopic, unsigned long timeout, unsigned long interval, const char* name) {
    if (isDeviceUp(ip)) {
      safePublish(replyTopic, (String("The ") + name + " already online. No wake-up needed.").c_str());
      digitalWrite(LED_PIN, HIGH); ledActive = false;
      return;
    }
    Serial.printf("Waking %s...\n", name);
    if (!sendWOL(mac)) {
      safePublish(replyTopic, (String("Failed to send wake-up signal to ") + name + ".").c_str());
      digitalWrite(LED_PIN, HIGH); ledActive = false;
      return;
    }
    safePublish(replyTopic, (String("Sent wake-up signal to ") + name + ". Waiting for it to come online...").c_str());
    unsigned long start = millis();
    bool cameOnline = false;
    while (millis() - start < timeout) {
      if (isDeviceUp(ip)) { cameOnline = true; break; }
      safePublish(replyTopic, (String(name) + " starting. " + String(interval / 1000) + " seconds before checking again.").c_str());
      digitalWrite(LED_PIN, HIGH);
      delay(200);
      digitalWrite(LED_PIN, LOW);
      delay(200);
      digitalWrite(LED_PIN, LOW);
      delay(interval);
    }
    safePublish(replyTopic, (
      cameOnline ?
        (String(name) + " online and ready.") :
        (String("Timeout: ") + name + " did not come online in time.")
    ).c_str());
    digitalWrite(LED_PIN, HIGH);
    ledActive = false;
  };
  // --- Desktop commands ---
  if (String(topic) == "status/desktop") {
    bool status = isDeviceUp(desktopIP);
    if (msg.equalsIgnoreCase("wake")) {
      wakeDevice(desktopIP, desktopMac, "status/desktop/reply", 60000, 5000, "Desktop");
    }
    else if (status && (msg.equalsIgnoreCase("off") || msg.equalsIgnoreCase("sleep") || msg.equalsIgnoreCase("ht"))) {
      String cmd = msg;
      safePublish("status/desktop/reply", ("Sending desktop " + cmd + " command...").c_str());
      if (!safePublish("desktop/commands", cmd.c_str(), "status/desktop/reply", "Failed to send command to desktop.")) {
        digitalWrite(LED_PIN, HIGH);
        ledActive = false;
        return;
      }
      safePublish("status/desktop/reply", ("Desktop " + cmd + " command sent. Waiting for confirmation from desktop...").c_str());
      digitalWrite(LED_PIN, HIGH);
      ledActive = false;
    }
    else {
      safePublish("status/desktop/reply", status ?
        "Desktop online. Reply 'off', 'sleep', or 'ht'" :
        "Desktop offline. Reply 'wake' to wake it."
      );
      digitalWrite(LED_PIN, HIGH);
      ledActive = false;
    }
    Serial.println("Replied with status/desktop/reply");
  }
  // --- Server commands ---
  else if (String(topic) == "status/server") {
    bool status = isDeviceUp(serverIP);
    if (msg.equalsIgnoreCase("wake")) {
      wakeDevice(serverIP, serverMac, "status/server/reply", 180000, 15000, "Server");
    } else if (status && (msg.equalsIgnoreCase("off") || msg.equalsIgnoreCase("sleep"))) {
      String cmd = msg;
      safePublish("status/server/reply", ("Sending server " + cmd + " command...").c_str());
      if (!safePublish("server/commands", cmd.c_str(), "status/server/reply", "Failed to send command to server.")) {
        digitalWrite(LED_PIN, HIGH); ledActive = false;
        return;
      }
      safePublish("status/server/reply", ("Server " + cmd + " command sent. Waiting for confirmation from server...").c_str());
      // Assuming server has similar reply mechanism; if not, remove "Waiting..." part
      digitalWrite(LED_PIN, HIGH); ledActive = false;
    } else {
      safePublish("status/server/reply", status ?
        "Server online. Reply 'off' or 'sleep'" :
        "Server offline. Reply 'wake' to wake it."
      );
      digitalWrite(LED_PIN, HIGH); ledActive = false;
    }
    Serial.println("Replied with status/server/reply");
  }
  // --- Existing wake topics ---
  else if (String(topic) == "wake/server") {
    wakeDevice(serverIP, serverMac, "wake/server/reply", 180000, 15000, "Server");
  }
  else if (String(topic) == "wake/desktop") {
    wakeDevice(desktopIP, desktopMac, "wake/desktop/reply", 60000, 5000, "Desktop");
  }
  // --- Relay desktop reply ---
  else if (String(topic) == "desktop/commands/reply") {
    safePublish("status/desktop/reply", msg.c_str());
  }
  // --- Relay server reply (if implemented on server) ---
  else if (String(topic) == "server/commands/reply") {
    safePublish("status/server/reply", msg.c_str());
  }
}
// --- MQTT RECONNECT ---
void reconnect() {
  while(!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if(client.connect("ESP8266_WOL", mqttUser, mqttPassword)) {
      Serial.println("connected");
      client.subscribe("wake/server");
      client.subscribe("wake/desktop");
      client.subscribe("status/server");
      client.subscribe("status/desktop");
      client.subscribe("desktop/commands/reply");
      client.subscribe("server/commands/reply"); // If server has reply
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}
// --- SETUP ---
void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH); // OFF by default
  if(!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS))
    Serial.println("STA Failed to configure");
  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");
  while(WiFi.status()!=WL_CONNECTED){ delay(500); Serial.print("."); }
  Serial.println("\nConnected! ESP IP: "); Serial.println(WiFi.localIP());
  Udp.begin(9);
  // --- INIT PROXIES ---
  proxies[0].listenPort = 2222; proxies[0].targetIP = serverIP; proxies[0].targetPort = 22; proxies[0].targetMac = serverMac; proxies[0].server = new WiFiServer(2222); proxies[0].server->begin();
  proxies[1].listenPort = 2445; proxies[1].targetIP = serverIP; proxies[1].targetPort = 445; proxies[1].targetMac = serverMac; proxies[1].server = new WiFiServer(2445); proxies[1].server->begin();
  proxies[2].listenPort = 25678; proxies[2].targetIP = serverIP; proxies[2].targetPort = 5678; proxies[2].targetMac = serverMac; proxies[2].server = new WiFiServer(25678); proxies[2].server->begin();
  proxies[3].listenPort = 2223; proxies[3].targetIP = desktopIP; proxies[3].targetPort = 22; proxies[3].targetMac = desktopMac; proxies[3].server = new WiFiServer(2223); proxies[3].server->begin();
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
  espClient.setInsecure(); // HiveMQ TLS
}
// --- LOOP ---
void loop() {
  if(!client.connected()) reconnect();
  client.loop();
  for(int i=0;i<4;i++) handleProxy(&proxies[i]);
}
