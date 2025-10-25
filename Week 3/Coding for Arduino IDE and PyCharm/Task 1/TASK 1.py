import serial
import time


ser = serial.Serial('COM7', 9600)
time.sleep(2)  # wait for Arduino to reset


print("Running... Press CTRL+C to stop.")


try:
   while True:
       line = ser.readline().decode().strip()

       if "/" in line:
           try:
                #Parse Arduino data
               data = line.split("/")
               voltage = float(data[0])
               adc_value = int(data[1].replace("ADC:", "").strip())
               resistance = float(data[2].replace("R:", "").strip())

               print(f"Voltage: {voltage:.2f} V | ADC: {adc_value} | Resistance: {resistance:.2f} Ω")

               # LED Control Logic
               if adc_value > (1023 / 2):
                   ser.write(b"LED_ON\n")
               else:
                   ser.write(b"LED_OFF\n")
           except:
               print("Parsing Error:", line)

except KeyboardInterrupt:
   print("Stopped by user.")

finally:
   ser.close()
   print("Serial connection closed.")