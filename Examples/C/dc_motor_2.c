#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "rp.h"

#define MIKROBUS 1

#if MIKROBUS == 1
    #define PWM_PIN RP_DIO1_P
    #define IN1_PIN RP_DIO1_N
    #define IN2_PIN RP_DIO2_N
    #define SLP_PIN RP_DIO2_P
#else
    #define PWM_PIN RP_DIO3_P
    #define IN1_PIN RP_DIO3_N
    #define IN2_PIN RP_DIO4_N
    #define SLP_PIN RP_DIO4_P
#endif


void pwm(int pin, int duty_cycle,int num_seconds)
{
    int period_us = 875;
    int num_periods = 0;
    int pulse_us = (duty_cycle * period_us) / 100;

    rp_DpinSetDirection(pin, RP_OUT);

    while (num_periods !=  num_seconds *1000 ) {
        rp_DpinSetState(pin, RP_HIGH);
        usleep(pulse_us);

        rp_DpinSetState(pin, RP_LOW);
        usleep(period_us - pulse_us);

        num_periods++;
        }
}

void pwm_sweep(int pin, int sweep_time, const char* up_or_down)
{
    int period_us = 875;
    int num_steps = sweep_time * 1000;

    rp_DpinSetDirection(pin, RP_OUT);

    if (strcmp(up_or_down, "up") == 0){
        for (int i = 0.15 * num_steps; i <= num_steps; i++) {
            int duty_cycle = (i * 100) / num_steps;
            int pulse_us = (duty_cycle * period_us) / 100;

            rp_DpinSetState(pin, RP_HIGH);
            usleep(pulse_us);

            rp_DpinSetState(pin, RP_LOW);
            usleep(period_us - pulse_us);
        }
    }
    else{
        for (int i = num_steps; i >= 0.15 * num_steps ; i--) {
            int duty_cycle = (i * 100) / num_steps;
            int pulse_us = (duty_cycle * period_us) / 100;

            rp_DpinSetState(pin, RP_HIGH);
            usleep(pulse_us);

            rp_DpinSetState(pin, RP_LOW);
            usleep(period_us - pulse_us);
        }
    }
}

void setMotorMode(const char* motorMode) {

    if (strcmp(motorMode, "MODE_CCW") == 0) {
        rp_DpinSetState(IN1_PIN, RP_LOW);
        rp_DpinSetState(IN2_PIN, RP_HIGH);
        rp_DpinSetState(SLP_PIN, RP_HIGH);
    } else if (strcmp(motorMode, "MODE_CW") == 0) {
        rp_DpinSetState(IN1_PIN, RP_HIGH);
        rp_DpinSetState(IN2_PIN, RP_LOW);
        rp_DpinSetState(SLP_PIN, RP_HIGH);
    } else if (strcmp(motorMode, "MODE_STOP") == 0) {
        rp_DpinSetState(IN1_PIN, RP_LOW);
        rp_DpinSetState(IN2_PIN, RP_LOW);
        rp_DpinSetState(SLP_PIN, RP_HIGH);
    } else if (strcmp(motorMode, "MODE_STANDBY") == 0) {
        rp_DpinSetState(IN1_PIN, RP_LOW);
        rp_DpinSetState(IN2_PIN, RP_LOW);
        rp_DpinSetState(SLP_PIN, RP_LOW);
    } else {

    }
}


int main (int argc, char **argv) {

    if (rp_Init() != RP_OK) {
        fprintf(stderr, "Red Pitaya API init failed!\n");
        return EXIT_FAILURE;
    }

    rp_DpinSetDirection(IN1_PIN, RP_OUT);
    rp_DpinSetDirection(IN2_PIN, RP_OUT);
    rp_DpinSetDirection(SLP_PIN, RP_OUT);

    setMotorMode("MODE_CCW");

    pwm_sweep(PWM_PIN, 10, "up");
    pwm_sweep(PWM_PIN, 10, "down");

    while(1){

        setMotorMode("MODE_CCW");
        pwm(PWM_PIN, 50, 3);

        setMotorMode("MODE_STOP");

        pwm(PWM_PIN, 50, 3);

        setMotorMode("MODE_CW");
        pwm(PWM_PIN, 50, 3);

        setMotorMode("MODE_STANDBY");
        pwm(PWM_PIN, 50, 3);
    }

    rp_Release();

    return EXIT_SUCCESS;
}
