#include "otaupload.h"

void setupOta()
{
  Serial.setDebugOutput(true);
  OTA.begin(OTAPORT);
}

void handleSketchUpdate()
{
  if (OTA.parsePacket()) {
    IPAddress remote = OTA.remoteIP();
    int cmd  = OTA.parseInt();
    int port = OTA.parseInt();
    int size   = OTA.parseInt();

    Serial.print("Update Start: ip:");
    Serial.print(remote);
    Serial.printf(", port:%d, size:%d\n", port, size);
    uint32_t startTime = millis();

    WiFiUDP::stopAll();

    if(!Update.begin(size)){
      Serial.println("Update Begin Error");
      return;
    }

    WiFiClient client;
    if (client.connect(remote, port)) {

      uint32_t written;
      while(!Update.isFinished()){
        written = Update.write(client);
        if(written > 0) client.print(written, DEC);
      }
      Serial.setDebugOutput(false);

      if(Update.end()){
        client.println("OK");
        Serial.printf("Update Success: %u\nRebooting...\n", millis() - startTime);
        ESP.restart();
      } else {
        Update.printError(client);
        Update.printError(Serial);
      }
    } else {
      Serial.printf("Connect Failed: %u\n", millis() - startTime);
    }
  }
}

// int handleSketchUpdate()
// {
//   int cb = listener.parsePacket();
//   if(cb)
//   {
//     IPAddress remote = listener.remoteIP();
//     int cmd  = listener.parseInt();
//     int port = listener.parseInt();
//     int sz   = listener.parseInt();
//     Serial.println("Got update: ");
//     Serial.printf("%d %d %d\r\n", cmd, port, sz);
//     WiFiClient cl;
//     if(!cl.connect(remote, port))
//     {
//       Serial.println("failed to connect.");
//       ESP.reset();
//     }
//     listener.stop();
//     if(!ESP.updateSketch(cl, sz))
//     {
//       Serial.println("Update failed.");
//       ESP.reset();
//     }
//   }
//   return cb;
// }