import os
import threading
import customtkinter as ctk
from CTkMessagebox import CTkMessagebox
import paho.mqtt.client as mqtt
MQTT_SERVER = "5239ffaebcbc49c6890527ee9c4b76e8.s1.eu.hivemq.cloud"
MQTT_PORT = 8883
MQTT_USER = "ESP_WOL"
MQTT_PASS = "@PasswordMQTT404*"
TOPIC = "desktop/commands"
REPLY_TOPIC = "desktop/commands/reply"
ctk.set_appearance_mode("dark")
ctk.set_default_color_theme("blue")
root = ctk.CTk()
root.withdraw() # Hide main window
# --- Popup with auto-confirm after 5 sec ---
def show_confirmation_main(action):
    # Add a small gray message under main text
    msg_box = CTkMessagebox(
        title=f"{action.capitalize()} Confirmation",
        message=f"Do you want to {action} the PC?\n\nAuto-confirm will happen in 5 seconds.",
        icon="question",
        option_1="No",
        option_2="Yes"
    )
    # Auto-confirm in 5 seconds
    def auto_confirm():
        msg_box.destroy() # Close popup if still open
        print(f"{action.capitalize()} auto-confirmed after 5 seconds.")
        client.publish(REPLY_TOPIC, f"{action.capitalize()} auto-confirmed and initiated.")
        if action == "off":
            os.system("shutdown /s /f /t 0")
        elif action == "sleep":
            os.system("rundll32.exe powrprof.dll,SetSuspendState 0,1,0")
        elif action == "ht":
            os.system("shutdown /h")
    
    timer = threading.Timer(5, auto_confirm)
    timer.start()
    
    # Wait for user selection
    response = msg_box.get()
    timer.cancel()  # Cancel the timer now that user has responded
    
    if response == "Yes":
        client.publish(REPLY_TOPIC, f"{action.capitalize()} confirmed and initiated.")
        if action == "off":
            os.system("shutdown /s /f /t 0")
        elif action == "sleep":
            os.system("rundll32.exe powrprof.dll,SetSuspendState 0,1,0")
        elif action == "ht":
            os.system("shutdown /h")
    else:
        print(f"{action.capitalize()} cancelled by user.")
        client.publish(REPLY_TOPIC, f"{action.capitalize()} cancelled by user.")
# --- MQTT handler ---
def on_message(client, userdata, msg):
    command = msg.payload.decode().strip().lower()
    print(f"Received command: {command}")
    if command in ("off", "sleep", "ht"):
        root.after(0, lambda: show_confirmation_main(command))
# --- MQTT setup ---
client = mqtt.Client()
client.username_pw_set(MQTT_USER, MQTT_PASS)
client.tls_set()
client.connect(MQTT_SERVER, MQTT_PORT)
client.subscribe(TOPIC)
client.on_message = on_message
def mqtt_loop():
    client.loop_forever()
threading.Thread(target=mqtt_loop, daemon=True).start()
print("Listening for shutdown/sleep/ht commands...")
root.mainloop()
