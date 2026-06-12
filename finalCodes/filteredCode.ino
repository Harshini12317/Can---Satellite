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

// ---------------- Filtering Config ----------------
#define FILTER_SIZE 10  // moving average window

float bmpTempBuf[FILTER_SIZE], pressureBuf[FILTER_SIZE], altitudeBuf[FILTER_SIZE];
float accelXBuf[FILTER_SIZE], accelYBuf[FILTER_SIZE], accelZBuf[FILTER_SIZE];
float gyroXBuf[FILTER_SIZE], gyroYBuf[FILTER_SIZE], gyroZBuf[FILTER_SIZE];
float mpuTempBuf[FILTER_SIZE];

int bufIndex = 0;

// ---------------- Moving Average Function ----------------
float movingAverage(float buf[], float newVal) {
  buf[bufIndex] = newVal;   // store in buffer
  float sum = 0;
  for (int i = 0; i < FILTER_SIZE; i++) {
    sum += buf[i];
  }
  return sum / FILTER_SIZE;
}

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

  // Initialize buffers to 0
  for (int i = 0; i < FILTER_SIZE; i++) {
    bmpTempBuf[i] = pressureBuf[i] = altitudeBuf[i] = 0;
    accelXBuf[i] = accelYBuf[i] = accelZBuf[i] = 0;
    gyroXBuf[i] = gyroYBuf[i] = gyroZBuf[i] = 0;
    mpuTempBuf[i] = 0;
  }
}

void loop() {
  // --- BMP180 Data ---
  float bmpTempRaw = bmp.readTemperature();
  int32_t pressureRaw = bmp.readPressure();
  float altitudeRaw = bmp.readAltitude();

  // --- MPU6050 Data ---
  sensors_event_t accel, gyro, temp;
  mpu.getEvent(&accel, &gyro, &temp);

  // --- Apply Moving Average ---
  float bmpTemp = movingAverage(bmpTempBuf, bmpTempRaw);
  float pressure = movingAverage(pressureBuf, pressureRaw);
  float altitude = movingAverage(altitudeBuf, altitudeRaw);

  float accelX = movingAverage(accelXBuf, accel.acceleration.x);
  float accelY = movingAverage(accelYBuf, accel.acceleration.y);
  float accelZ = movingAverage(accelZBuf, accel.acceleration.z);

  float gyroX = movingAverage(gyroXBuf, gyro.gyro.x);
  float gyroY = movingAverage(gyroYBuf, gyro.gyro.y);
  float gyroZ = movingAverage(gyroZBuf, gyro.gyro.z);

  float mpuTemp = movingAverage(mpuTempBuf, temp.temperature);

  // Update buffer index
  bufIndex = (bufIndex + 1) % FILTER_SIZE;

  // --- Print to Serial ---
  Serial.println("=== BMP180 (Barometer) ===");
  Serial.printf("Temperature: %.2f °C\n", bmpTemp);
  Serial.printf("Pressure: %.2f Pa\n", pressure);
  Serial.printf("Altitude: %.2f m\n", altitude);

  Serial.println("=== MPU6050 (IMU) ===");
  Serial.printf("Temperature: %.2f °C\n", mpuTemp);
  Serial.printf("Acceleration -> X: %.2f, Y: %.2f, Z: %.2f m/s^2\n", accelX, accelY, accelZ);
  Serial.printf("Gyro -> X: %.2f, Y: %.2f, Z: %.2f rad/s\n", gyroX, gyroY, gyroZ);

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
  LoRa.print(mpuTemp);
  LoRa.print(" °C, Accel: (");
  LoRa.print(accelX);
  LoRa.print(", ");
  LoRa.print(accelY);
  LoRa.print(", ");
  LoRa.print(accelZ);
  LoRa.print("), Gyro: (");
  LoRa.print(gyroX);
  LoRa.print(", ");
  LoRa.print(gyroY);
  LoRa.print(", ");
  LoRa.print(gyroZ);
  LoRa.print(")");
  LoRa.endPacket();

  Serial.println("Data sent via LoRa!");
  Serial.println("--------------------------\n");

  counter++;
  delay(200);  // Faster sampling (200 ms)
}
