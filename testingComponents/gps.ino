void setup() {
  // UART0 is Serial (pins TX0=GPIO1, RX0=GPIO3)
  Serial.begin(9600);   // GPS default baud

  // Small delay to settle
  delay(2000);
  Serial.println("=== GPS UART0 Test Start ===");
  Serial.println("Reading GPS data from TX0/RX0...");
}

void loop() {
  // Forward GPS data directly to Serial Monitor
  while (Serial.available()) {
    char c = Serial.read();
    Serial.write(c);  // Echo GPS output back to monitor
  }
}
