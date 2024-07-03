# PwngridSpam Attack

## Description
Welcome to the ESP-32 PwngridSpam Attack project! 

This standalone ESP-32 attack is designed to spam face and name on all pwnagotchi devices nearby and can be used to causing a DoSPWND (Denial Of Screen PWND part). 

Perfect for showcasing at events like DEFCON.😜

It's also available on the Evil-M5Project : https://github.com/7h30th3r0n3/Evil-M5Core2


## Installation
To set up the project, follow these steps:

1. **Download the Code:**
   Download the provided code files to your local machine.

2. **Install dependencies:**
   Ensure you have the necessary libraries installed in your Arduino IDE.

3. **Upload the code:**
   Use the Arduino IDE to flash the provided code to your ESP-32 device.

## Usage
Once the code is uploaded to your ESP-32 it's start spamming automatically and you can control the beacon transmissions via serial commands:

- **Start regular beacon transmission:**
    ```sh
    start
    ```

- **Stop beacon transmission:**
    ```sh
    stop
    ```

- **Start PWND beacon transmission:**
    ```sh
    dospwnd
    ```

- **Stop PWND beacon transmission and start regular beacon transmission:**
    ```sh
    stopdospwnd
    ```

### Serial Command Guide
- `start`: Begin regular beacon transmission.
- `stop`: Halt all beacon transmissions.
- `dospwnd`: Initiate DoSPWND mode beacon transmission.
- `stopdospwnd`: Cease PWND mode and resume regular beacon transmission.

## Features
- **Beacon Spam:** Transmit custom faces and names to pwnagotchi devices.
- **DoSPWND Mode:** Special mode to block pwnagotchi screens.
- **Serial Control:** Easy control through serial commands.
- **Event Ready:** Ideal for live demonstrations at hacking events.

## License
This project is licensed under the MIT License. See the `LICENSE` file for details.

## Credits
Special thanks to the open-source community and all contributors.

---

Bring this project to your next hacking event and demonstrate the power and flexibility of the ESP-32 in a fun and interactive way.

Perfect for engaging with fellow hackers and showcasing innovative uses of Wi-Fi technology.
