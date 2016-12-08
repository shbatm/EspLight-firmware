#include "effectParse.h"

#define NUM_PARAM 7

WiFiUDP effectListener;
String effectVariables[NUM_PARAM][2];

// ?pincode=1234&effect=0&brightness=255&var0=207&var1=255&var2=236[&savesetting=1]
// String effectVariables[7][2];

void setupEffectParse(int port)
{
  // start listening for udp packets. for effect controle.
  effectListener.begin(port);
}

String getAlphaNumString(String &data)
{
  String text = "";
  String tail = "";
  if(data[0] == '?' || data[0] == '=' || data[0] == '&')
  {
    int i = 0;
    while(isalnum(data[i+1]))
    {
      text += data[i+1];
      i++;
    }
    i++;
    while(data[i] != '\0')
    {
      tail += data[i];
      i++;
    }
  }
  data = tail;
  return text;
}

String effectArg(const char *par)
{
  for(int i = 0; i < NUM_PARAM; i++)
  {
    if(effectVariables[i][0] == String(par))
    {
      return effectVariables[i][1];
    }
  }
  return String("");
}

void applyEffectData()
{
  // don't apply if codes don't match.
  stripcontrol.pincode = effectArg("pin").toInt();
  if(accessPin != stripcontrol.pincode)
  {
    // debug print some info. 
    Serial.println("rejecting packet.");
    Serial.printf("set:%d\ngot:%d\n", accessPin, stripcontrol.pincode);
    return;
  }
  else
  {
    Serial.println("pins:");
    Serial.printf("set:%d\ngot:%d\n", accessPin, stripcontrol.pincode);
    // parse and store in struct.
    stripcontrol.effect = effectArg("effect").toInt();
    stripcontrol.brightness = effectArg("brightness").toInt();
    stripcontrol.varZero = effectArg("var0").toInt();
    stripcontrol.varOne = effectArg("var1").toInt();
    stripcontrol.varTwo = effectArg("var2").toInt();
    // stripcontrol.changed = true; # Re-used as flag to store new colors
    
    // If `savesetting` works, expose the rest of the contruct and 
    // comment out the 5 lines above to enable only partial URLs to be sent.
    //
    // if(effectArg("effect") != String(""))  
    // {
    //   stripcontrol.effect = effectArg("effect").toInt();
    // }
    // if(effectArg("brightness") != String(""))  
    // {
    //   stripcontrol.brightness = effectArg("brightness").toInt();
    // }
    // if(effectArg("var0") != String(""))  
    // {
    //   stripcontrol.varZero = effectArg("var0").toInt();
    // }
    // if(effectArg("var1") != String(""))  
    // {
    //   stripcontrol.varOne = effectArg("var1").toInt();
    // }
    // if(effectArg("var2") != String(""))  
    // {
    //   stripcontrol.varTwo = effectArg("var2").toInt();
    // }
    // If savesetting flag is passed, write the settings to EEPROM
    if(effectArg("savesetting") != String(""))  
    {
      if(effectArg("savesetting").toInt() == 1)
      {
        stripcontrol.changed = 2;
      }
    }
    if (stripcontrol.changed != 2) { stripcontrol.changed = 1; }
  }
  debugPrintStripControl();
}

void parseEffectPacket(String data)
{
  /* clear current settings. */
  for(int i = 0; i < NUM_PARAM; i++)
  {
    String parameter = "";
    String argument = "";
    effectVariables[i][0] = parameter;
    effectVariables[i][1] = argument;
  }
  /* apply received settings. */
  for(int i = 0; i < NUM_PARAM; i++)
  {
    String parameter = getAlphaNumString(data);
    String argument = getAlphaNumString(data);
    effectVariables[i][0] = parameter;
    effectVariables[i][1] = argument;
  }
}

void printPacketInfo(int packetSize)
{
  Serial.printf("packet size: %d ", packetSize);
  Serial.print("from: ");
  Serial.print(effectListener.remoteIP());
  Serial.printf(" port: %d\n", effectListener.remotePort());
}

String readPacketContents(WiFiUDP listener)
{
  String received = "";
  while(listener.available())
  {
    received += (char)listener.read();
  }
  return received;
}

void handleEffectUpdate()
{
  String received;
  int cb = effectListener.parsePacket();
  if(cb)
  {
    printPacketInfo(cb);
    if(effectListener.peek() == '?')
    {
      // got a http string.
      Serial.println("got effect string:");
      received = readPacketContents(effectListener);
      Serial.println(received);
      parseEffectPacket(received);
      applyEffectData();
    }
    else
    {
      Serial.println("got here");
      received = readPacketContents(effectListener);
      Serial.print("received: ");
      Serial.println(received);
      Serial.println("got a find request");
      Serial.print("received == EspFind: ");
      Serial.println(String("EspFind") == received ? "True":"False");
      if(String("EspFind") == received)
      {
        findResponse(effectListener);
      }
    }
  }
}

void findResponse(WiFiUDP listener)
{
  // create a packetd to put things in.
  listener.beginPacket(listener.remoteIP(), listener.remotePort());
  // reply with our board name.
  listener.print(board_name);
  Serial.print("Sending: ");
  Serial.println(board_name);
  Serial.print("to: ");
  Serial.print(listener.remoteIP());
  Serial.print(":");
  Serial.println(listener.remotePort());
  // end creating packet and send.
  listener.endPacket();
}
