import time
from rp_overlay import overlay
import rp

# Initialize the FPGA overlay
fpga = overlay()
rp.rp_Init()

# Pin definitions
## CS = rp.rp_GPIOnSetDirection(0b00001000)
## RST = rp.rp_GPIOnSetDirection(0b00010000)
## INT = rp.rp_GPIOpSetDirection(0b00010000)

## PWM = rp.rp_GPIOpSetDirection(0b00001000)

# Set RST and CS pins as outputs
rp.rp_GPIOnSetDirection(0b00011000)
# Set PWM and INT pins as outputs
rp.rp_GPIOpSetDirection(0b00011000)

def set_motor_mode(motor_mode):
    # Counter ClockWise direction
    if motor_mode == "MODE_CCW":
        # Set CS to LOW and RST to HIGH
        rp.rp_GPIOnSetState(0b00010000)
        # Set INT to HIGH
        rp.rp_GPIOpSetState(0b00010000)
    # ClockWise direction
    elif motor_mode == "MODE_CW":
        # Set CS to HIGH and RST to LOW
        rp.rp_GPIOnSetState(0b00001000)
        # Set INT to HIGH
        rp.rp_GPIOpSetState(0b00010000)
    elif motor_mode == "MODE_STOP":
        # Set CS to LOW and RST to LOW
        rp.rp_GPIOnSetState(0b00000000)
        # Set INT to HIGH
        rp.rp_GPIOpSetState(0b00010000)
    elif motor_mode == "MODE_STANDBY":
        # Set CS to LOW and RST to LOW
        rp.rp_GPIOnSetState(0b00000000)
        # Set INT to LOW
        rp.rp_GPIOpSetState(0b00000000)
    else:
        # Handle the default case
        pass

def pwm_sweep(sweep_time, up_or_down):
    period_us = 875
    num_steps = int(sweep_time * 1000)  # Convert sweep time to milliseconds

    current_state = rp.rp_GPIOpGetState()[1]
    print(f"Current state: {rp.rp_GPIOpGetState()[1]:>08b}")
    pwm_cleared_state = current_state & 0b11111101
    print(f"Cleared state: {pwm_cleared_state:>08b}")
    pwm_set_state = pwm_cleared_state | 0b00001000
    print(f"Set state: {pwm_set_state:>08b}")

    if up_or_down == "up":
        # The 0.15 is because the motor starts spinning at 15%
        for i in range(int(0.15 * num_steps), num_steps + 1):
            duty_cycle = (i * 100) / num_steps  # percent of power, zero to 100
            pulse_us = (duty_cycle * period_us) / 100

            # Set PWM to HIGH
            rp.rp_GPIOpSetState(pwm_set_state)
            # Delay for pulse duration
            time.sleep(pulse_us / 1000000)

            # Set PWM to LOW
            rp.rp_GPIOpSetState(pwm_cleared_state)
            # Delay for remaining period
            time.sleep((period_us - pulse_us) / 1000000)

    elif up_or_down == "down":
        # The 0.15 is because the motor starts spinning at 15%
        for i in range(num_steps, int(0.15 * num_steps) - 1, -1):
            duty_cycle = (i * 100) / num_steps  # percent of power, zero to 100
            pulse_us = (duty_cycle * period_us) / 100

            # Set PWM to HIGH
            rp.rp_GPIOpSetState(pwm_set_state)
            # Delay for pulse duration
            time.sleep(pulse_us / 1000000)

            # Set PWM to LOW
            rp.rp_GPIOpSetState(pwm_cleared_state)
            # Delay for remaining period
            time.sleep((period_us - pulse_us) / 1000000)

while True:
    # Set motor mode to counter-clockwise
    set_motor_mode("MODE_CCW")
    # Increase motor speed from 15 to 100% for 10 seconds
    pwm_sweep(10, "up")
    # Decrease motor speed from 100 to 15% for 10 seconds
    pwm_sweep(10, "down")

    # Set motor mode to clockwise
    set_motor_mode("MODE_CW")
    # Increase motor speed from 15 to 100% for 10 seconds
    pwm_sweep(10, "up")
    # Decrease motor speed from 100 to 15% for 10 seconds
    pwm_sweep(10, "down")

# Release resources
rp.rp_Release()