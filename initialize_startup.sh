#!/bin/bash
# /root/RedPitayaClickShield/initialize_startup.sh

SYSTEMD_DIR=/etc/systemd/system
SRC_DIR=/root/RedPitayaClickShield/startup_runtime
TARGET_SCRIPT=/usr/local/brushless_17_trigger.sh

# Copy or symlink the trigger script to /usr/local
cp "$SRC_DIR/brushless_17_trigger.sh" "$TARGET_SCRIPT"
chmod +x "$TARGET_SCRIPT"

# Link service and timer units to systemd directory
ln -sf "$SRC_DIR/brushless.service" "$SYSTEMD_DIR/brushless.service"
ln -sf "$SRC_DIR/brushless.timer" "$SYSTEMD_DIR/brushless.timer"

# Reload systemd and enable timer
systemctl daemon-reload
systemctl enable brushless.timer
systemctl restart brushless.timer

echo "Startup service initialized and timer started."
echo "Rebooting now..."

reboot