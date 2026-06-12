# Can---Satellite
Arduino-based CanSat flight software designed for atmospheric data collection, GPS positioning, telemetry communication, sensor integration, and real-time mission monitoring.

### Overview

This repository contains the complete Arduino firmware and supporting resources for our CanSat project.

A CanSat is a miniature satellite designed to fit within the volume of a standard soda can. The objective of this project is to simulate the functions of a real satellite, including environmental monitoring, data collection, telemetry transmission, and payload operations during flight.

The CanSat collects real-time atmospheric and positional data during launch, descent, and landing, and transmits the information to a ground station for monitoring and analysis.

## 🎯 Mission Objectives
# Primary Mission
Measure atmospheric parameters during flight.
Collect and transmit sensor data in real time.
Monitor altitude changes throughout descent.
Ensure reliable communication with the ground station.
Secondary Mission
GPS tracking and location reporting.
Data logging for post-flight analysis.
Autonomous payload operations.
Safe recovery after landing.

# 🛠 Hardware Components
Microcontroller
Arduino Nano / Arduino Uno / ESP32 (update with your board)
Sensors
BMP280/BME280 – Pressure, Temperature, Altitude
DHT11/DHT22 – Temperature & Humidity
MPU6050 – Accelerometer & Gyroscope
GPS Module (Neo-6M)


# Communication
LoRa Module (SX1278)


# Power System
Lithium-ion Battery
Voltage Regulation Circuit

# ⚙️ Software Features
Sensor Monitoring

# The system continuously reads data from onboard sensors, including:

Temperature
Humidity
Atmospheric Pressure
Altitude
GPS Coordinates
Acceleration
Orientation
Telemetry System

Sensor data is packaged and transmitted to the ground station at regular intervals.

## Example telemetry packet:

TIME:120
TEMP:29.4
HUM:62
ALT:145
LAT:13.0827
LON:80.2707
GPS Tracking

The onboard GPS module provides:

Latitude
Longitude
Altitude
Time Synchronization
Data Logging

All sensor readings are stored locally for:

Flight analysis
Mission validation
Performance evaluation
Autonomous Operations

The CanSat performs:

Sensor initialization
Data collection
Telemetry transmission
Logging operations
Landing detection

without requiring manual intervention
