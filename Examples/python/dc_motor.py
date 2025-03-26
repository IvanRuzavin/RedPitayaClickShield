import time
from rp_overlay import overlay
import rp

# Initialize the FPGA overlay
fpga = overlay()
rp.rp_Init()

# CS = rp.rp_GPIOnSetDirection(0b00001000)
# RST = rp.rp_GPIOnSetDirection(0b00010000)
# INT = rp.rp_GPIOpSetDirection(0b00010000)

# PWM = rp.rp_GPIOpSetDirection(0b00001000)

# Set RST and CS pins as outputs
rp.rp_GPIOnSetDirection(0b00011000)
# Set PWM and INT pins as outputs
rp.rp_GPIOpSetDirection(0b00011000)

def pwm(duty_cycle, num_seconds):
    period_us = 875
    pulse_us = int((duty_cycle * period_us) / 100)
    num_periods = 0

    current_state = rp.rp_GPIOpGetState()[1]
    print(f"Current state: {rp.rp_GPIOpGetState()[1]:>08b}")
    pwm_cleared_state = current_state & 0b11111101
    print(f"Cleared state: {pwm_cleared_state:>08b}")
    pwm_set_state = pwm_cleared_state | 0b00001000
    print(f"Set state: {pwm_set_state:>08b}")

    while True:
        if num_periods ==  num_seconds *1000:
            break
        # rp.rp_DpinSetState(pin, rp.RP_HIGH)
        rp.rp_GPIOpSetState(pwm_set_state)
        time.sleep(pulse_us / 1000000)
        # rp.rp_DpinSetState(pin, rp.RP_LOW)
        rp.rp_GPIOpSetState(pwm_cleared_state)
        time.sleep((period_us - pulse_us) / 1000000)
        num_periods += 1

def set_motor_mode(motor_mode):
    if motor_mode == "MODE_CCW":
        # rp.rp_DpinSetState(CS, rp.RP_LOW)
        # rp.rp_DpinSetState(RST, rp.RP_HIGH)
        rp.rp_GPIOnSetState(0b00010000)
        # rp.rp_DpinSetState(INT, rp.RP_HIGH)
        rp.rp_GPIOpSetState(0b00010000)
    elif motor_mode == "MODE_CW":
        # rp.rp_DpinSetState(CS, rp.RP_HIGH)
        # rp.rp_DpinSetState(RST, rp.RP_LOW)
        rp.rp_GPIOnSetState(0b00001000)
        # rp.rp_DpinSetState(INT, rp.RP_HIGH)
        rp.rp_GPIOpSetState(0b00010000)
    elif motor_mode == "MODE_STOP":
        # rp.rp_DpinSetState(CS, rp.RP_LOW)
        # rp.rp_DpinSetState(RST, rp.RP_LOW)
        rp.rp_GPIOnSetState(0b00000000)
        # rp.rp_DpinSetState(INT, rp.RP_HIGH)
        rp.rp_GPIOpSetState(0b00010000)
    elif motor_mode == "MODE_STANDBY":
        # rp.rp_DpinSetState(CS, rp.RP_LOW)
        # rp.rp_DpinSetState(RST, rp.RP_LOW)
        rp.rp_GPIOnSetState(0b00000000)
        # rp.rp_DpinSetState(INT, rp.RP_LOW)
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

            # set pin state high
            # rp.rp_DpinSetState(pin, rp.RP_HIGH)
            rp.rp_GPIOpSetState(pwm_set_state)
            # delay for pulse duration
            time.sleep(pulse_us / 1000000)

            # set pin state low
            # rp.rp_DpinSetState(pin, rp.RP_LOW)
            rp.rp_GPIOpSetState(pwm_cleared_state)

            # delay for remaining period
            time.sleep((period_us - pulse_us) / 1000000)

    elif up_or_down == "down":
        # The 0.15 is because the motor starts spinning at 15%
        for i in range(num_steps, int(0.15 * num_steps) - 1, -1):
            duty_cycle = (i * 100) / num_steps  # percent of power, zero to 100
            pulse_us = (duty_cycle * period_us) / 100

            # set pin state high
            # rp.rp_DpinSetState(pin, rp.RP_HIGH)
            rp.rp_GPIOpSetState(pwm_set_state)

            # delay for pulse duration
            time.sleep(pulse_us / 1000000)

            # set pin state low
            # rp.rp_DpinSetState(pin, rp.RP_LOW)
            rp.rp_GPIOpSetState(pwm_cleared_state)

            # delay for remaining period
            time.sleep((period_us - pulse_us) / 1000000)

#sweep up or down to lower the current spike
set_motor_mode("MODE_CW")
pwm_sweep(50, "up")

while True:
    # Set motor mode to counter-clockwise
    set_motor_mode("MODE_CCW")
    # Run motor at 50% duty cycle for 3 seconds
    pwm_sweep(50, "up")

    # Set motor mode to stop
    set_motor_mode("MODE_STOP")
    # Run motor at 50% duty cycle for 3 seconds
    pwm_sweep(50, "down")

    # Set motor mode to clockwise
    set_motor_mode("MODE_CW")
    # Run motor at 50% duty cycle for 3 seconds
    pwm_sweep(50, "up")

    # Set motor mode to standby
    set_motor_mode("MODE_STANDBY")
    # Run motor at 50% duty cycle for 3 seconds
    pwm_sweep(50, "down")

# Release resources
rp.rp_Release()