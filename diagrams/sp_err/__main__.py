import numpy as np
import serial
import serial.tools.list_ports
import seaborn as sns
import matplotlib.pyplot as plt
import pandas as pd
import time


# --- SERIAL ---

print()

ports = serial.tools.list_ports.comports()
for port, desc, hwid in ports:
    print(f"{port}: {desc} [{hwid}]")

port = input(f"port = [{ports[0][0]}]").strip()
if port == "":
    port = ports[0][0]

ser = serial.Serial(port, 115200, timeout=1)

# --- SERIAL ---

m = []

while True:
    if ser.in_waiting != 0:
        try:
            msg = ser.read_until(expected="\r\n".encode("ascii")).decode("ascii")
            print(msg, end="")
            if msg.startswith("D"):
                m.append([ float(x) for x in msg[1:].split(",") ])
            if msg.startswith("F"):
                sns.set_theme(style="darkgrid")

                flights = sns.load_dataset("flights")

                data = pd.DataFrame({
                    "power": [x[0] for x in m] * 2,
                    "error": [x[1] for x in m] + [x[2] for x in m],
                    "wheel": [0] * len(m) + [1] * len(m),
                })

                # Plot the responses for different events and regions
                sns.lineplot(x="power", y="error",
                             hue="wheel",
                             data=data)

                plt.show()

                time.sleep(1000)
        except Exception as e:
            print(e)
