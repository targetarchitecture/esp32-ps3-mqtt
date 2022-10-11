#include <Arduino.h>
// #include <sstream>
#include <ESP8266WiFi.h>
// #include <PubSubClient.h>
// #include <mqttClient.h>
// #include "credentials.h"
#include "topics.h"
 #include "vars.h"
#include <Preferences.h>
// #include <Ps3Controller.h>

#define PRINT_TO_SERIAL 1

// declare objects & variables
void setupWifi();
void onWifiConnect(const WiFiEventStationModeGotIP &event);
void onWifiDisconnect(const WiFiEventStationModeDisconnected &event);
void setUpPS3();
void notify();
void onConnect();

WiFiEventHandler wifiConnectHandler;
WiFiEventHandler wifiDisconnectHandler;

Preferences preferences;

unsigned long loopDelay = 100;

unsigned long interval = 1000 * 60 * 10; // 10 minutes for a reboot if no signal recieved

bool sendNetworkDetails = true;

unsigned long lastDelayLoopMillis = 0;

int battery = 0;

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);

#ifdef PRINT_TO_SERIAL
  Serial.begin(115200);
#endif

  // get loop delay
  preferences.begin("ps3", false);
  loopDelay = preferences.getULong("loopDelay", loopDelay);

  randomSeed(micros());

  setupWifi();

  //setupMQTTClient();

  setUpPS3(); // connect controller

  //MQTT_RUMBLE = 0;
}

void loop()
{
  // delay(50);  //produces 17-19 messages per second
  // delay(100); //produces 9-10 messages per second
  // delay(200); //produces ~5 messages per second
  delay(loopDelay); // now variable

  // loopPS2(MQTT_RUMBLE);

  // stop rumble after 500ms if no MQTT signal recieved
  unsigned long currentMillis = millis();

  if (currentMillis - lastRumbleCommandRecievedMillis > 500)
  {
    lastRumbleCommandRecievedMillis = currentMillis;
    MQTT_RUMBLE = 0;
  }

  if (WiFi.isConnected() == false)
  {
    // just do nothing and wait for the reconnection event to happen
    delay(1000);
  }
  else
  {
    // MQTT section
    if (MQTTClient.connected() == false)
    {
      MQTTConnect();
    }
    else
    {
      if (sendNetworkDetails == true)
      {
        // MQTTClient.publish(MQTT_IP_TOPIC, WIFI_SSID);
        MQTTClient.publish(MQTT_IP_TOPIC.c_str(), WiFi.localIP().toString().c_str());

        sendNetworkDetails = false;
      }

      // send loop delay
      if (currentMillis - lastDelayLoopMillis > 5000)
      {
        lastDelayLoopMillis = currentMillis;
        MQTTClient.publish(MQTT_LOOP_DELAY_TOPIC.c_str(), String(loopDelay).c_str());
      }
    }

    MQTTClient.loop();
  }

  // check for in-activity
  // if (currentMillis - lastCommandSentMillis > interval)
  // {
  //   MQTTClient.publish(MQTT_INFO_TOPIC.c_str(), "Time to reboot due to inactivity");

  //   delay(500);

  //   // well it's probably time for a reboot
  //   ESP.restart();
  // }

  // set LED off
  digitalWrite(LED_BUILTIN, HIGH);
}

void setupWifi()
{
  // Register event handlers
  wifiConnectHandler = WiFi.onStationModeGotIP(onWifiConnect);
  wifiDisconnectHandler = WiFi.onStationModeDisconnected(onWifiDisconnect);

  // sort out WiFi
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD); // Connect to the network
}

void onWifiConnect(const WiFiEventStationModeGotIP &event)
{
  sendNetworkDetails = true;

#ifdef PRINT_TO_SERIAL
  Serial.println("Connected to Wi-Fi sucessfully.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
#endif
}

void onWifiDisconnect(const WiFiEventStationModeDisconnected &event)
{
#ifdef PRINT_TO_SERIAL
  Serial.println("Disconnected from Wi-Fi, trying to connect...");
#endif
  WiFi.disconnect();
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD); // Reconnect to the network
}

void notify()
{
  // //--- Digital cross/square/triangle/circle button events ---
  // if (Ps3.event.button_down.cross)
  //   Serial.println("Started pressing the cross button");
  // if (Ps3.event.button_up.cross)
  //   Serial.println("Released the cross button");

  // if (Ps3.event.button_down.square)
  //   Serial.println("Started pressing the square button");
  // if (Ps3.event.button_up.square)
  //   Serial.println("Released the square button");

  // if (Ps3.event.button_down.triangle)
  //   Serial.println("Started pressing the triangle button");
  // if (Ps3.event.button_up.triangle)
  //   Serial.println("Released the triangle button");

  // if (Ps3.event.button_down.circle)
  //   Serial.println("Started pressing the circle button");
  // if (Ps3.event.button_up.circle)
  //   Serial.println("Released the circle button");

  // //--------------- Digital D-pad button events --------------
  // if (Ps3.event.button_down.up)
  //   Serial.println("Started pressing the up button");
  // if (Ps3.event.button_up.up)
  //   Serial.println("Released the up button");

  // if (Ps3.event.button_down.right)
  //   Serial.println("Started pressing the right button");
  // if (Ps3.event.button_up.right)
  //   Serial.println("Released the right button");

  // if (Ps3.event.button_down.down)
  //   Serial.println("Started pressing the down button");
  // if (Ps3.event.button_up.down)
  //   Serial.println("Released the down button");

  // if (Ps3.event.button_down.left)
  //   Serial.println("Started pressing the left button");
  // if (Ps3.event.button_up.left)
  //   Serial.println("Released the left button");

  // //------------- Digital shoulder button events -------------
  // if (Ps3.event.button_down.l1)
  //   Serial.println("Started pressing the left shoulder button");
  // if (Ps3.event.button_up.l1)
  //   Serial.println("Released the left shoulder button");

  // if (Ps3.event.button_down.r1)
  //   Serial.println("Started pressing the right shoulder button");
  // if (Ps3.event.button_up.r1)
  //   Serial.println("Released the right shoulder button");

  // //-------------- Digital trigger button events -------------
  // if (Ps3.event.button_down.l2)
  //   Serial.println("Started pressing the left trigger button");
  // if (Ps3.event.button_up.l2)
  //   Serial.println("Released the left trigger button");

  // if (Ps3.event.button_down.r2)
  //   Serial.println("Started pressing the right trigger button");
  // if (Ps3.event.button_up.r2)
  //   Serial.println("Released the right trigger button");

  // //--------------- Digital stick button events --------------
  // if (Ps3.event.button_down.l3)
  //   Serial.println("Started pressing the left stick button");
  // if (Ps3.event.button_up.l3)
  //   Serial.println("Released the left stick button");

  // if (Ps3.event.button_down.r3)
  //   Serial.println("Started pressing the right stick button");
  // if (Ps3.event.button_up.r3)
  //   Serial.println("Released the right stick button");

  // //---------- Digital select/start/ps button events ---------
  // if (Ps3.event.button_down.select)
  //   Serial.println("Started pressing the select button");
  // if (Ps3.event.button_up.select)
  //   Serial.println("Released the select button");

  // if (Ps3.event.button_down.start)
  //   Serial.println("Started pressing the start button");
  // if (Ps3.event.button_up.start)
  //   Serial.println("Released the start button");

  // if (Ps3.event.button_down.ps)
  //   Serial.println("Started pressing the Playstation button");
  // if (Ps3.event.button_up.ps)
  //   Serial.println("Released the Playstation button");

  // //---------------- Analog stick value events ---------------
  // if (abs(Ps3.event.analog_changed.stick.lx) + abs(Ps3.event.analog_changed.stick.ly) > 2)
  // {
  //   Serial.print("Moved the left stick:");
  //   Serial.print(" x=");
  //   Serial.print(Ps3.data.analog.stick.lx, DEC);
  //   Serial.print(" y=");
  //   Serial.print(Ps3.data.analog.stick.ly, DEC);
  //   Serial.println();
  // }

  // if (abs(Ps3.event.analog_changed.stick.rx) + abs(Ps3.event.analog_changed.stick.ry) > 2)
  // {
  //   Serial.print("Moved the right stick:");
  //   Serial.print(" x=");
  //   Serial.print(Ps3.data.analog.stick.rx, DEC);
  //   Serial.print(" y=");
  //   Serial.print(Ps3.data.analog.stick.ry, DEC);
  //   Serial.println();
  // }

  // //--------------- Analog D-pad button events ----------------
  // if (abs(Ps3.event.analog_changed.button.up))
  // {
  //   Serial.print("Pressing the up button: ");
  //   Serial.println(Ps3.data.analog.button.up, DEC);
  // }

  // if (abs(Ps3.event.analog_changed.button.right))
  // {
  //   Serial.print("Pressing the right button: ");
  //   Serial.println(Ps3.data.analog.button.right, DEC);
  // }

  // if (abs(Ps3.event.analog_changed.button.down))
  // {
  //   Serial.print("Pressing the down button: ");
  //   Serial.println(Ps3.data.analog.button.down, DEC);
  // }

  // if (abs(Ps3.event.analog_changed.button.left))
  // {
  //   Serial.print("Pressing the left button: ");
  //   Serial.println(Ps3.data.analog.button.left, DEC);
  // }

  // //---------- Analog shoulder/trigger button events ----------
  // if (abs(Ps3.event.analog_changed.button.l1))
  // {
  //   Serial.print("Pressing the left shoulder button: ");
  //   Serial.println(Ps3.data.analog.button.l1, DEC);
  // }

  // if (abs(Ps3.event.analog_changed.button.r1))
  // {
  //   Serial.print("Pressing the right shoulder button: ");
  //   Serial.println(Ps3.data.analog.button.r1, DEC);
  // }

  // if (abs(Ps3.event.analog_changed.button.l2))
  // {
  //   Serial.print("Pressing the left trigger button: ");
  //   Serial.println(Ps3.data.analog.button.l2, DEC);
  // }

  // if (abs(Ps3.event.analog_changed.button.r2))
  // {
  //   Serial.print("Pressing the right trigger button: ");
  //   Serial.println(Ps3.data.analog.button.r2, DEC);
  // }

  // //---- Analog cross/square/triangle/circle button events ----
  // if (abs(Ps3.event.analog_changed.button.triangle))
  // {
  //   Serial.print("Pressing the triangle button: ");
  //   Serial.println(Ps3.data.analog.button.triangle, DEC);
  // }

  // if (abs(Ps3.event.analog_changed.button.circle))
  // {
  //   Serial.print("Pressing the circle button: ");
  //   Serial.println(Ps3.data.analog.button.circle, DEC);
  // }

  // if (abs(Ps3.event.analog_changed.button.cross))
  // {
  //   Serial.print("Pressing the cross button: ");
  //   Serial.println(Ps3.data.analog.button.cross, DEC);
  // }

  // if (abs(Ps3.event.analog_changed.button.square))
  // {
  //   Serial.print("Pressing the square button: ");
  //   Serial.println(Ps3.data.analog.button.square, DEC);
  // }

  // //---------------------- Battery events ---------------------
  // if (battery != Ps3.data.status.battery)
  // {
  //   battery = Ps3.data.status.battery;
  //   Serial.print("The controller battery is ");
  //   if (battery == ps3_status_battery_charging)
  //     Serial.println("charging");
  //   else if (battery == ps3_status_battery_full)
  //     Serial.println("FULL");
  //   else if (battery == ps3_status_battery_high)
  //     Serial.println("HIGH");
  //   else if (battery == ps3_status_battery_low)
  //     Serial.println("LOW");
  //   else if (battery == ps3_status_battery_dying)
  //     Serial.println("DYING");
  //   else if (battery == ps3_status_battery_shutdown)
  //     Serial.println("SHUTDOWN");
  //   else
  //     Serial.println("UNDEFINED");
  // }
}

void setUpPS3()
{
  // Ps3.attach(notify);
  // Ps3.attachOnConnect(onConnect);
  // Ps3.begin("98:f4:ab:67:8e:8e");
}

void onConnect()
{
  Serial.println("Connected.");
}