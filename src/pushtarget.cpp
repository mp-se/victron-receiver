/*
MIT License

Copyright (c) 2024 Magnus

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
 */
#include <MQTT.h>

#include <config.hpp>
#include <helper.hpp>
#include <main.hpp>
#include <pushtarget.hpp>

VictronReceiverPush::VictronReceiverPush(
    VictronReceiverConfig* victronReceiverConfig)
    : BasePush(victronReceiverConfig) {
  _victronReceiverConfig = victronReceiverConfig;
}

void VictronReceiverPush::sendAll(String name, String mac, JsonObject& doc) {
  _http->setReuse(true);

  String payload;
  payload.reserve(2000);
  mac.replace(":", "");
  doc["name"] = name;

  if (myConfig.hasTargetMqtt()) {
    Log.info(F("PUSH: Sending data to MQTT for device %s" CR), name.c_str());

    // Convert the entries in the json document to mqtt messages on a topic
    for (JsonPair kv : doc) {
      String key = kv.key().c_str();
      char buf[200];

      if (key != "decrypted_data" && key != "model" && key != "vendor_id" &&
          key != "victron_record_type") {  // These are for internal use, skip
                                           // them if they
                                           // exist.

        // victron_instant/[mac_adress]/[attribute]:[value]|
        snprintf(&buf[0], sizeof(buf), "victron_instant/%s/%s:%s|", mac.c_str(),
                 key.c_str(), kv.value().as<String>().c_str());
        payload += buf;

        // home assistant sensor configuration, set device class where possible
        // and reference the value topic above.
        snprintf(&buf[0], sizeof(buf), "homeassistant/sensor/%s_%s/config:{",
                 name.c_str(), key.c_str());
        payload += buf;

        // add correct unit of measurement based on the attribute name
        if (key.indexOf("voltage") >= 0) {
          payload +=
              "\"device_class\":\"voltage\",\"unit_of_measurement\":\"V\",";
        } else if (key.indexOf("temperature") >= 0) {
          payload +=
              "\"device_class\":\"temperature\",\"unit_of_measurement\":"
              "\"°C\",";
        } else if (key.indexOf("current") >= 0) {
          payload +=
              "\"device_class\":\"current\",\"unit_of_measurement\":\"A\",";
        } else if (key.indexOf("power") >= 0) {
          payload +=
              "\"device_class\":\"current\",\"unit_of_measurement\":\"W\",";
        }

        snprintf(&buf[0], sizeof(buf), "\"unique_id\":\"%s_%s\",", mac.c_str(),
                 key.c_str());
        payload += buf;
        snprintf(&buf[0], sizeof(buf),
                 "\"device\":{\"identifiers\":\"%s\",\"name\":\"%s\"},",
                 mac.c_str(), name.c_str());
        payload += buf;
        snprintf(
            &buf[0], sizeof(buf),
            "\"name\":\"%s_%s\",\"state_topic\":\"victron_instant/%s/%s\"}|",
            name.c_str(), key.c_str(), mac.c_str(), key.c_str());
        payload += buf;
      }
    }

    sendMqtt(payload);
  } else if (myConfig.hasTargetHttpPost()) {
    // Convert the entries in the json document to rest api requests
    for (JsonPair kv : doc) {
      String key = kv.key().c_str();
      char buf[200];
      char url[200];

      if (key != "decrypted_data" && key != "model" && key != "vendor_id" &&
          key != "victron_record_type") {  // These are for internal use, skip
                                           // them if they
                                           // exist.

        Log.info(F("PUSH: Sending data to REST API for device %s and %s" CR),
                 name.c_str(), key.c_str());

        snprintf(url, sizeof(url), "%sapi/states/sensor.%s_%s",
                 myConfig.getTargetHttpPost(), name.c_str(), key.c_str());

        if (key.indexOf("voltage") >= 0) {
          snprintf(buf, sizeof(buf),
                   "{\"state\": \"%s\", \"attributes\": "
                   "{\"unit_of_measurement\": \"V\"}}",
                   kv.value().as<String>().c_str());
        } else if (key.indexOf("temperature") >= 0) {
          snprintf(buf, sizeof(buf),
                   "{\"state\": \"%s\", \"attributes\": "
                   "{\"unit_of_measurement\": \"°C\"}}",
                   kv.value().as<String>().c_str());
        } else if (key.indexOf("power") >= 0) {
          snprintf(buf, sizeof(buf),
                   "{\"state\": \"%s\", \"attributes\": "
                   "{\"unit_of_measurement\": \"W\"}}",
                   kv.value().as<String>().c_str());
        } else if (key.indexOf("current") >= 0) {
          snprintf(buf, sizeof(buf),
                   "{\"state\": \"%s\", \"attributes\": "
                   "{\"unit_of_measurement\": \"A\"}}",
                   kv.value().as<String>().c_str());
        } else {
          snprintf(buf, sizeof(buf), "{\"state\": \"%s\"}",
                   kv.value().as<String>().c_str());
        }

        String payload(buf);
        sendHttpPost(payload, url, "Content-Type: application/json",
                     myConfig.getHeader2HttpPost());
      }
    }
  }
}

// EOF
