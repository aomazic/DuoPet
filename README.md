# DuoPet Automatic Cat and Dog Feeder

## Overview
DuoPet is an innovative solution for pet owners to ensure their furry companions are fed the right amount of food, whether they're home or away. This project integrates a web client, a server running on Raspberry Pi, and an Arduino that manages the mechanical aspects of the feeder. Communication between the Raspberry Pi and Arduino is established via MQTT for reliable message delivery.

## Features
- **Web Client**: A user-friendly interface for scheduling feeding times and quantities.
- **MQTT Communication**: Efficient and reliable messaging protocol for IoT devices.
- **Dual Feeding Mechanism**: Separate compartments and dispensing systems for cats and dogs.
- **Real-time Notifications**: Alerts and status updates on feeding activities and system health.

## Hardware Requirements
- Raspberry Pi (Server Host)
- Arduino (Controls mechanics)
- Servo Motors (Mechanical control for food dispensing)
- RFID Reader (Pet identification)
- Ultrasonic Sensors (Food level detection)
- Various electronic components (Wires, resistors, LEDs)

## Software Requirements
- MQTT Broker/Client
- Web Server (Apache/Nginx or similar)
- JavaScript, HTML, CSS (For web client)
- Arduino IDE (For programming the Arduino)

## Setup and Installation
1. Set up the Raspberry Pi with a suitable OS and install the MQTT broker.
2. Flash the Arduino with the provided code using the Arduino IDE.
3. Connect the mechanical components as per the circuit diagram.
4. Deploy the web client to the Raspberry Pi and ensure it can communicate with the MQTT broker.

## Usage
- **Configuring Feed Times**: Set up feeding schedules through the web interface.
- **Manual Feeding**: Override scheduled feeding times and dispense food manually.
- **Monitoring**: Check the current status of the feeder, including food levels and feeding logs.


## Made by
- Antonio Omazić
- Dorijan Osmakčić