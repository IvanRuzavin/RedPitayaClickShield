#!/bin/bash
# /root/RedPitayaClickShield/startup_runtime/initialize_startup.sh

SYSTEMD_DIR=/etc/systemd/system
SRC_DIR=/root/RedPitayaClickShield/startup_runtime

ln -sf "$SRC_DIR/brushless.service" "$SYSTEMD_DIR/brushless.service"
ln -sf "$SRC_DIR/brushless.timer" "$SYSTEMD_DIR/brushless.timer"

systemctl daemon-reload
systemctl enable brushless.timer
systemctl restart brushless.timer

reboot