const int flowSensorPin = 2;  // Pin connected to the digital output of the FS-3000A
volatile int pulseCount = 0;   // Number of pulses from the sensor
unsigned long lastPulseTime = 0;  // Variable to store the last time a pulse was detected
float waterVolume = 0.0;       // Volume of water in liters
const float pulsesPerLiter = 7.0;  // Calibration: Pulses per liter (update based on your sensor)
unsigned long lastTime = 0;   // Variable to store the last time the flow rate was calculated
unsigned long noFlowTime = 0; // Variable to store the time when flow stops

void setup() {
  // Start serial communication
  Serial.begin(9600);
  Serial.println("FS-3000A Water Flow Sensor Initialized");

  // Set the flow sensor pin as input
  pinMode(flowSensorPin, INPUT);

  // Attach interrupt to count pulses on the rising edge (when signal goes from LOW to HIGH)
  attachInterrupt(digitalPinToInterrupt(flowSensorPin), pulseCounter, RISING);
}

void loop() {
  // Check if no pulses have been detected for a certain period (e.g., 3 seconds)
  if (millis() - lastPulseTime > 3000) {  // No pulses for 3 seconds, tap is closed
    if (pulseCount > 0) {
      // Calculate the volume of water used when the tap is closed
      waterVolume = pulseCount / pulsesPerLiter;  // Calculate volume in liters

      // Print the total water volume used when the tap was open
      Serial.print("Water Used: ");
      Serial.print(waterVolume);
      Serial.println(" Liters");

      // Reset pulse count for the next measurement
      pulseCount = 0;
      waterVolume = 0.0;
    }
  } else {
    // While the tap is open, keep measuring the flow rate
    if (millis() - lastTime >= 1000) {  // Every second
      float flowRate = pulseCount / pulsesPerLiter;  // Calculate flow rate in L/min

      // Print the flow rate while the tap is open
      Serial.print("Flow Rate: ");
      Serial.print(flowRate);
      Serial.println(" L/min");

      // Calculate and print the water volume used since the last measurement
      waterVolume += flowRate / 60.0;  // Add the water volume used in the last second
      Serial.print("Water Used: ");
      Serial.print(waterVolume);
      Serial.println(" Liters");

      // Reset pulse count for the next second
      pulseCount = 0;
      lastTime = millis();
    }
  }

  // Detect when the tap is closed
  if (pulseCount == 0) {
    noFlowTime = millis();
  }
}

// Interrupt service routine to count pulses
void pulseCounter() {
  pulseCount++;  // Increment the pulse count every time a pulse is detected
  lastPulseTime = millis();  // Update the last pulse time
}
