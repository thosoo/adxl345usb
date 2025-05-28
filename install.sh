#!/usr/bin/env bash
set -euo pipefail
IFS=$'\n\t'

# Installing ADXL345USB Tool (wrapper)
echo -e "\n[ $(date '+%Y-%m-%d %H:%M:%S') ] - Installing ADXL345USB Tool..."

# Install the wrapper directly from the same directory (must be in the same folder as this script)
SCRIPT_DIR=$(dirname "$(realpath "$0")")

# Ensure the wrapper exists
if [[ ! -f "$SCRIPT_DIR/adxl345usb" ]]; then
    echo "Error: adxl345usb wrapper not found in the installation directory."
    exit 1
fi

# Create the target directory if it doesn't exist
mkdir -p /usr/local/bin

# Install the wrapper
install -m 755 "$SCRIPT_DIR/adxl345usb" /usr/local/bin/adxl345spi

# Configure sudoers for OctoPrint
SUDOERS_FILE="/etc/sudoers.d/octoprint_adxl"
rm -f "$SUDOERS_FILE"
echo "octoprint ALL=(ALL) NOPASSWD: /usr/local/bin/adxl345spi" > "$SUDOERS_FILE"
chmod 440 "$SUDOERS_FILE"
chown root:root "$SUDOERS_FILE"

# Final message
echo -e "\n✔ ADXL345USB Tool installed successfully!"
