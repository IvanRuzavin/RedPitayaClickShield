import time
from rp_overlay import overlay
import rp


fpga = overlay()
rp.rp_Init()

# Set all DIO_P pins to be outputs
rp.rp_GPIOpSetDirection(0b00010000)
# Set all DIO_N pins to be outputs
# rp.rp_GPIOnSetDirection(0b00010000)


print(f"Column P directions: {int(rp.rp_GPIOpGetDirection()[1]) & 0b11111111:>08b}")
# print(f"Column N directions: {int(rp.rp_GPIOnGetDirection()[1]) & 0b11111111:>08b}")

for i in range(10):
    print("Setting the ON state")
    rp.rp_GPIOpSetState(0b00010000)
    # rp.rp_GPIOnSetState(0b00010000)
    time.sleep(1)
    print("Setting the OFF state")
    rp.rp_GPIOpSetState(0b00000000)
    # rp.rp_GPIOnSetState(0b00000000)
    time.sleep(1)

rp.rp_Release()