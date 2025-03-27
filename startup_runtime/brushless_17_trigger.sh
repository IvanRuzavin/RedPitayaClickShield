#!/bin/bash
export PYTHONPATH=/opt/redpitaya/lib/python:/root/RedPitayaClickShield/rp-api/apiroot
echo "PYTHONPATH=$PYTHONPATH" >> /tmp/myscript.log
taskset 0x2 /usr/bin/python3 /root/RedPitayaClickShield/Examples/python/brushless_17.py >> /tmp/myscript.log 2>&1
exit 0