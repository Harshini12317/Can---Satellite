#include <Wire.h>
#include <Adafruit_BMP085.h>   // For BMP180
#include <Adafruit_MPU6050.h>  // For MPU6050
#include <Adafruit_Sensor.h>
#include <SPI.h>
#include <LoRa.h>

// ---------------- I2C Pins ----------------
#define SDA_PIN 21
#define SCL_PIN 22

// ---------------- LoRa Pins ----------------
#define NSS_PIN   5
#define RST_PIN   14
#define DIO0_PIN  2

// ---------------- Sensor Objects ----------------
Adafruit_BMP085 bmp;
Adafruit_MPU6050 mpu;

int counter = 0;

void setup() {
  Serial.begin(115200);
  delay(1000);

  // ---------- I2C Init ----------
  Wire.begin(SDA_PIN, SCL_PIN);

  // ---------- BMP180 Init ----------
  Serial.println("Initializing BMP180...");
  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP180 sensor, check wiring!");
    while (1) delay(1000);
  }
  Serial.println("BMP180 initialized successfully!");

  // ---------- MPU6050 Init ----------
  Serial.println("Initializing MPU6050...");
  if (!mpu.begin()) {
    Serial.println("Could not find a valid MPU6050 sensor, check wiring!");
    while (1) delay(1000);
  }
  Serial.println("MPU6050 initialized successfully!");
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  // ---------- LoRa Init ----------
  Serial.println("Initializing LoRa...");
  LoRa.setPins(NSS_PIN, RST_PIN, DIO0_PIN);
  if (!LoRa.begin(433E6)) {   // Change frequency if needed
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  Serial.println("LoRa initialized successfully!");
}

void loop() {
  // --- BMP180 Data ---
  float bmpTemp = bmp.readTemperature();
  int32_t pressure = bmp.readPressure();
  float altitude = bmp.readAltitude();

  // --- MPU6050 Data ---
  sensors_event_t accel, gyro, temp;
  mpu.getEvent(&accel, &gyro, &temp);

  // --- Print to Serial ---
  Serial.println("=== BMP180 (Barometer) ===");
  Serial.printf("Temperature: %.2f °C\n", bmpTemp);
  Serial.printf("Pressure: %ld Pa\n", pressure);
  Serial.printf("Altitude: %.2f m\n", altitude);

  Serial.println("=== MPU6050 (IMU) ===");
  Serial.printf("Temperature: %.2f °C\n", temp.temperature);
  Serial.printf("Acceleration -> X: %.2f, Y: %.2f, Z: %.2f m/s^2\n",
                accel.acceleration.x, accel.acceleration.y, accel.acceleration.z);
  Serial.printf("Gyro -> X: %.2f, Y: %.2f, Z: %.2f rad/s\n",
                gyro.gyro.x, gyro.gyro.y, gyro.gyro.z);

  // --- Send Data via LoRa ---
  LoRa.beginPacket();
  LoRa.print("Packet ");
  LoRa.print(counter);
  LoRa.print(" | BMP180 -> Temp: ");
  LoRa.print(bmpTemp);
  LoRa.print(" °C, Pressure: ");
  LoRa.print(pressure);
  LoRa.print(" Pa, Alt: ");
  LoRa.print(altitude);
  LoRa.print(" m | MPU6050 -> Temp: ");
  LoRa.print(temp.temperature);
  LoRa.print(" °C, Accel: (");
  LoRa.print(accel.acceleration.x);
  LoRa.print(", ");
  LoRa.print(accel.acceleration.y);
  LoRa.print(", ");
  LoRa.print(accel.acceleration.z);
  LoRa.print("), Gyro: (");
  LoRa.print(gyro.gyro.x);
  LoRa.print(", ");
  LoRa.print(gyro.gyro.y);
  LoRa.print(", ");
  LoRa.print(gyro.gyro.z);
  LoRa.print(")");
  LoRa.endPacket();

  Serial.println("Data sent via LoRa!");
  Serial.println("--------------------------\n");

  counter++;
  delay(2000);  // Update every 2 seconds
}
