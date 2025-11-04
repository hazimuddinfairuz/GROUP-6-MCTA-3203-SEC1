import serial
import time
import math

# single serial port to Arduino (which also provides MPU6050 data)
arduino = serial.Serial("COM7", 9600, timeout=1)
time.sleep(2)

AUTHORIZED_ID = "0013082958"
armed = False
led_on = False

print("System Ready ✅")

def detect_circle():
    """
    Ask Arduino to stream a short burst of MPU data, collect points,
    and decide whether a circular motion happened.
    Returns True if gesture detected (lenient), False otherwise.
    """
    print("🔄 Requesting samples from Arduino... Move in a circle now!")

    # ask Arduino to stream (~2 seconds)
    arduino.reset_input_buffer()  # clear old data
    arduino.write(b'R')

    points = []
    start = time.time()
    timeout = 3.0  # safety timeout

    # read until timeout (Arduino will stop streaming after ~2s)
    while time.time() - start < timeout:
        raw = arduino.readline().decode(errors='ignore').strip()
        if not raw:
            continue
        # expect: ax,ay,az,gx,gy,gz
        parts = raw.split(',')
        if len(parts) >= 3:
            try:
                ax = int(parts[0])
                ay = int(parts[1])
                az = int(parts[2])
                points.append((ax, ay, az))
            except ValueError:
                # skip malformed lines
                continue

    print(f"Collected {len(points)} samples.")

    if len(points) < 8:
        print("⚠️ Not enough motion data — try moving more/stronger.")
        return False

    # Build angles from (ax, ay) and unwrap to follow rotation
    angles = [math.degrees(math.atan2(p[1], p[0])) for p in points]

    # Unwrap angles to avoid -180/180 discontinuity
    unwrapped = [angles[0]]
    for a in angles[1:]:
        prev = unwrapped[-1]
        diff = a - prev
        if diff > 180:
            diff -= 360
        elif diff < -180:
            diff += 360
        unwrapped.append(prev + diff)

    # Compute net rotation and also range (helps with noisy motion)
    net_rotation = unwrapped[-1] - unwrapped[0]
    rotation_range = max(unwrapped) - min(unwrapped)

    print(f"Net rotation: {net_rotation:.1f}°, Range: {rotation_range:.1f}°")

    # LENIENT detection logic (either a decent net rotation OR wide range)
    # Lower thresholds to make detection easier for imperfect circles
    if abs(net_rotation) > 100 or rotation_range > 140:
        print("✅ Circle gesture detected!")
        return True

    # extra check: count direction-consistent large angle deltas
    deltas = [unwrapped[i+1]-unwrapped[i] for i in range(len(unwrapped)-1)]
    large_moves = sum(1 for d in deltas if abs(d) > 10)  # count meaningful moves
    if large_moves >= max(6, len(points)//4):
        print("✅ Motion had multiple consistent movements — accepting as circle.")
        return True

    print("❌ Motion insufficient — try a clearer circular motion.")
    return False



while True:
    try:
        card = input("Tap card: ").strip()
    except KeyboardInterrupt:
        print("\nExiting.")
        break

    if card == AUTHORIZED_ID:
        armed = not armed

        if armed:
            print("✅ Card Accepted — now do circular motion!")

            if detect_circle():
                print("✅ Gesture OK → LED ON")
                arduino.write(b'1')
                led_on = True
            else:
                print("❌ Wrong gesture — try again")
                armed = False

        else:
            print("🔒 System Disarmed → LED OFF")
            arduino.write(b'0')
            led_on = False

    else:
        print("❌ WRONG CARD")

        arduino.write(b'X')