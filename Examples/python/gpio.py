import time
from rp_overlay import overlay
import rp

fpga = overlay()
rp.rp_Init()

rp.rp_GPIOpSetDirection(0b11111111)
print(f"Column P directions: {rp.rp_GPIOpGetDirection()[1]:>08b}")
print(f"Column N directions: {rp.rp_GPIOpGetDirection()[0]:>08b}")

for i in range(10):
    print("Setting the ON state")
    rp.rp_GPIOpSetState(0b11111111)
    time.sleep(1)
    print("Setting the OFF state")
    rp.rp_GPIOpSetState(0b00000000)
    time.sleep(1)

rp.rp_Release()