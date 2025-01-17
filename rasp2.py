import RPi.GPIO as GPIO
import time

def water_flow_sensor():
    # Pin connected to the digital output of the FS-3000A
    flow_sensor_pin = 2
    
    # Variables to store the sensor data
    pulse_count = 0
    last_pulse_time = 0
    water_volume = 0.0
    pulses_per_liter = 7.0  # Calibration: Pulses per liter (update based on your sensor)
    last_time = 0
    no_flow_time = 0
    
    # Set up the GPIO
    GPIO.setmode(GPIO.BCM)
    GPIO.setup(flow_sensor_pin, GPIO.IN)
    
    # Attach the interrupt to count pulses on the rising edge
    GPIO.add_event_detect(flow_sensor_pin, GPIO.RISING, callback=pulse_counter)
    
    while True:
        # Check if no pulses have been detected for a certain period (e.g., 3 seconds)
        if time.time() - last_pulse_time > 3:  # No pulses for 3 seconds, tap is closed
            if pulse_count > 0:
                # Calculate the volume of water used when the tap is closed
                water_volume = pulse_count / pulses_per_liter  # Calculate volume in liters
                
                # Print the total water volume used when the tap was open
                print(f"Water Used: {water_volume:.2f} Liters")
                
                # Reset pulse count for the next measurement
                pulse_count = 0
                water_volume = 0.0
        else:
            # While the tap is open, keep measuring the flow rate
            if time.time() - last_time >= 1:  # Every second
                flow_rate = pulse_count / pulses_per_liter  # Calculate flow rate in L/min
                
                # Print the flow rate while the tap is open
                print(f"Flow Rate: {flow_rate:.2f} L/min")
                
                # Calculate and print the water volume used since the last measurement
                water_volume += flow_rate / 60.0  # Add the water volume used in the last second
                print(f"Water Used: {water_volume:.2f} Liters")
                
                # Reset pulse count for the next second
                pulse_count = 0
                last_time = time.time()
        
        # Detect when the tap is closed
        if pulse_count == 0:
            no_flow_time = time.time()

def pulse_counter(channel):
    global pulse_count, last_pulse_time
    pulse_count += 1  # Increment the pulse count every time a pulse is detected
    last_pulse_time = time.time()  # Update the last pulse time

if __name__ == "__main__":
    try:
        water_flow_sensor()
    except KeyboardInterrupt:
        GPIO.cleanup()
        print("Exiting...")
