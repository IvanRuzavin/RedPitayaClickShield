# In this ReadMe you can find an info how to run Brushless 17 Click demo every 30 seconds and what has been done to finish this project
## What has been done
- Added (brushless_17.py)[./Examples/python/brushless_17.py] script to Examples/python folder
    - With the latest 2.00 RedPitaya OS neither system approach with modifying system gpio files nor C API approach (which nodifies them as well)worked
    - Python approach with using python macros from RedPitaya didn't work as well
    - Only writing binary values to the registers using Python API approach worked
- Added (dc_motor.py)[./Examples/python/dc_motor.py] and (gpio.py)[./Examples/python/gpio.py] scripts to Examples/python folder for testing purposes
- Added (brushless.service)[./startup_runtime/brushless.service] and (brushless.timer)[./startup_runtime/brushless.timer] files to startup_runtime folder
    - This increased a portability level of this demo
- Added (initialize_startup.sh)[./initialize_startup.sh] script for portability purposes as well

## How to run the demo every 30s
- Power Up your RedPitaya
- Connect to it via browser
- Go to Development->Web Console
    - Connect to RedPitaya using root as a login and password
- Run `git clone https://github.com/IvanRuzavin/RedPitayaClickShield`
- Run `chmod +x RedPitayaClickShield/initialize_startup.sh`
- Run `./RedPitayaClickShield/initialize_startup.sh`
    - This will reboot your RedPitaya and after it is ON again you will have brushless 17 Click running in the background every 30 seconds
    - After rebooting RedPitaya now will turn the motor on and off every 30 seconds
- Useful commands
    - journalctl -u brushless.service
        - Will show you if there were any errors during startup call of the demo script
    - cat /tmp/myscript.log
        - Will show you if there were any errors during runnict the script itself
    - systemctl status brushless.service
        - Will show you the status of service
