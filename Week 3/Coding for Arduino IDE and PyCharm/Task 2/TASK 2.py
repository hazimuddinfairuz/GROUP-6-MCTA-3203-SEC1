import serial
import matplotlib.pyplot as plt
import time


ser = serial.Serial('COM7', 9600)
time.sleep(2)


plt.ion()
fig, ax = plt.subplots()
x_vals, y_vals = [], []


try:
   while True:
       raw = ser.readline().decode().strip()


       if raw.isdigit():
           angle = int(raw)
           print("Servo Angle:", angle)


           x_vals.append(len(x_vals))
           y_vals.append(angle)


           ax.clear()
           ax.set_ylim(0, 180)
           ax.set_title("Real-Time Servo Angle Plot")
           ax.set_xlabel("Samples")
           ax.set_ylabel("Angle (°)")
           ax.plot(x_vals, y_vals)
           plt.pause(0.05)


except KeyboardInterrupt:
   print("Sending stop command to Arduino...")
   ser.write(b'x')  # Hantar stop signal ke Arduino
   time.sleep(1)


finally:
   ser.close()
   plt.ioff()
   plt.show()
   print("Serial connection closed.")