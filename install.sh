#!/usr/bin/env bash
set -euo pipefail
IFS=$'\n\t'

# ─────────────────── Helper ────────────────────────────────
die() { echo "ERROR: $*" >&2; exit 1; }
timestamp() { date '+%F %T'; }
log() { echo -e "[ $(timestamp) ]  $*"; }

[[ $EUID -eq 0 ]] || die "please run as root (sudo bash install.sh)"

# ─────────────────── Locate wrapper ────────────────────────
#   Works on systems without GNU realpath (BusyBox, Alpine…)
SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd -P)"
WRAPPER_SRC="$SCRIPT_DIR/adxl345usb"
WRAPPER_DST="/usr/local/bin/adxl345spi"   # keep old name for the plugin

[[ -f $WRAPPER_SRC ]] || die "wrapper '$WRAPPER_SRC' not found"

# ─────────────────── Install ───────────────────────────────
log "Installing ADXL345USB wrapper → ${WRAPPER_DST}"
install -m 755 "$WRAPPER_SRC" "$WRAPPER_DST"

# ─────────────────── Sudoers entry (OctoPrint user) ────────
SUDOERS_FILE="/etc/sudoers.d/octoprint_adxl"
log "Configuring sudoers → $SUDOERS_FILE"

cat <<EOF | tee "$SUDOERS_FILE" >/dev/null
octoprint ALL=(ALL) NOPASSWD: ${WRAPPER_DST}
EOF
chmod 440 "$SUDOERS_FILE"
chown root:root "$SUDOERS_FILE"

log "✔ ADXL345USB Tool installed successfully!"