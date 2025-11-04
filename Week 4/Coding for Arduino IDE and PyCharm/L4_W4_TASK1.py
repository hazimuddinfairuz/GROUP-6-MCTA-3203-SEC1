import serial, time, math
from collections import deque
import matplotlib.pyplot as plt

# --- Serial connection ---
ser = serial.Serial('COM7', 9600, timeout=1)  # adjust port if needed

# --- Parameters ---
GYRO_SCALE = 131.0  # LSB per °/s for ±250 dps
DEADZONE = 5.0      # ignore small gyro noise
CIRCLE_DEG = 340    # total rotation threshold for detection
WINDOW = 2.0        # seconds of recent data kept
COOLDOWN = 1.5      # seconds between detections

# --- Live plot setup (optional) ---
plt.ion()
fig, ax = plt.subplots()
ax.set_title("MPU6050 Accelerometer Live Data")
ax.set_xlabel("Samples")
ax.set_ylabel("Accel (raw)")
line_ax, = ax.plot([], [], label='AX')
line_ay, = ax.plot([], [], label='AY')
line_az, = ax.plot([], [], label='AZ')
ax.legend()
data_ax, data_ay, data_az = deque(maxlen=200), deque(maxlen=200), deque(maxlen=200)

# --- Gyro integration setup ---
rot_hist = deque()  # (timestamp, delta_angle)
last_t, last_detect = None, 0.0

print("Starting live plot + circle detection.\nMove MPU in circle to test. Press Ctrl+C to stop.")

try:
    while True:
        raw = ser.readline().decode(errors='ignore').strip()
        if not raw:
            continue

        vals = raw.split(',')
        if len(vals) != 6:
            continue

        try:
            ax_raw, ay_raw, az_raw, gx_raw, gy_raw, gz_raw = map(int, vals)
        except ValueError:
            continue

        # --- Plot accel live ---
        data_ax.append(ax_raw)
        data_ay.append(ay_raw)
        data_az.append(az_raw)
        line_ax.set_xdata(range(len(data_ax)))
        line_ax.set_ydata(list(data_ax))
        line_ay.set_xdata(range(len(data_ay)))
        line_ay.set_ydata(list(data_ay))
        line_az.set_xdata(range(len(data_az)))
        line_az.set_ydata(list(data_az))
        ax.relim()
        ax.autoscale_view()
        fig.canvas.draw()
        fig.canvas.flush_events()

        # --- Gyro integration ---
        now = time.time()
        if last_t is None:
            last_t = now
            continue
        dt = now - last_t
        last_t = now

        gz_dps = gz_raw / GYRO_SCALE
        if abs(gz_dps) < DEADZONE:
            gz_dps = 0
        dtheta = gz_dps * dt  # incremental rotation in degrees
        rot_hist.append((now, dtheta))

        # drop old samples
        while rot_hist and now - rot_hist[0][0] > WINDOW:
            rot_hist.popleft()

        # --- Detection ---
        total = sum(d for _, d in rot_hist)
        abs_total = sum(abs(d) for _, d in rot_hist)

        if (abs(total) > CIRCLE_DEG or abs_total > CIRCLE_DEG) and (now - last_detect > COOLDOWN):
            # direction = "CW" if total < 0 else "CCW"
            print(f"✅ Circle detected! Direction: circle, Net rotation: {total:.0f}°")
            last_detect = now

except KeyboardInterrupt:
    print("\nStopped.")
finally:
    ser.close()