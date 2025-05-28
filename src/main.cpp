// adxl345usb firmware – Fly‑ADXL345‑USB (RP2040)
// =================================================
// * Builds with the *standard* Arduino‑MBed core.
// * Streams ADXL345 data exactly like navaismo/adxl345spi – header
//   "time,x,y,z" followed by CSV lines, default 250 Hz.
// * Uses an mbed::SPI instance on GPIO 10‑12 (the Fly‑board tracks).
//
//   CS  = GPIO 9   (chip‑select)
//   SCK = GPIO 10  (SPI1 SCK)
//   MOSI= GPIO 11  (SPI1 TX)
//   MISO= GPIO 12  (SPI1 RX)
//
// =============================================================

#include <Arduino.h>
#include <SPI.h>          // still needed for SPISettings type
#include "mbed.h"

/* ---------------- ADXL345 register map ---------------- */
#define REG_DEVID       0x00
#define REG_BW_RATE     0x2C
#define REG_POWER_CTL   0x2D
#define REG_DATA_FORMAT 0x31
#define REG_DATAX0      0x32

/* ---------------- Hardware pins ---------------------- */
#define CS_PIN 9       // chip‑select (Fly‑ADXL345‑USB default)

/* mbed SPI on GPIO11(MOSI) / 12(MISO) / 10(SCK) */
static mbed::SPI spiPort(
    digitalPinToPinName(11),   // MOSI
    digitalPinToPinName(12),   // MISO
    digitalPinToPinName(10));  // SCK

constexpr uint32_t SPI_SPEED = 2'000'000;   // 2 MHz

/* conversion: ±16 g full‑res → 3.9 mg/LSB → 1/4096 g/LSB */
constexpr float ACC_CONV = 16.0f / 4096.0f;

volatile uint32_t sample_us = 4'000;        // 250 Hz default

/* ---------------- Low‑level helpers ------------------ */
inline void csLow()  { digitalWrite(CS_PIN, LOW);  }
inline void csHigh() { digitalWrite(CS_PIN, HIGH); }

void adxlWrite(uint8_t reg, uint8_t val) {
    csLow();
    spiPort.write(reg);
    spiPort.write(val);
    csHigh();
}

void adxlReadBurst(uint8_t startReg, uint8_t *buf, size_t len) {
    csLow();
    spiPort.write(0x80 | 0x40 | startReg);   // READ | MULTI
    for (size_t i = 0; i < len; ++i) buf[i] = spiPort.write(0);
    csHigh();
}

/* ---------------- Sensor initialisation -------------- */
void setupADXL(uint8_t bw_rate_code = 0x0A) { // 0x0A = 100 Hz placeholder
    pinMode(CS_PIN, OUTPUT);
    csHigh();

    spiPort.frequency(SPI_SPEED);
    spiPort.format(8 /*bits*/, 3 /*mode*/);  // SPI mode 3

    adxlWrite(REG_DATA_FORMAT, 0x0B);   // full‑res ±16 g
    adxlWrite(REG_BW_RATE, bw_rate_code);
    adxlWrite(REG_POWER_CTL, 0x08);     // measurement mode
}

/* ---------------- USB command parser ----------------- */
String cmd;
volatile uint32_t t0_us;                 // reset when rate changes

void handleCommand() {
    cmd.trim(); cmd.toUpperCase();

    if (cmd.startsWith("F=")) {
        uint32_t f = cmd.substring(2).toInt();
        if (f >= 1 && f <= 3200) {
            sample_us = 1'000'000UL / f;
            t0_us = micros();                  // start time at 0 again
            Serial.println("time,x,y,z");     // header for new block
        }
    } else if (cmd.startsWith("H")) {
        Serial.println("Commands: F=<1-3200> Hz  | H help");
    }
    cmd = "";                                 // clear buffer
}

/* ---------------- Arduino setup ---------------------- */
void setup() {
    Serial.begin(2'000'000);
    while (!Serial) { }

    setupADXL();
    t0_us = micros();
    Serial.println("time,x,y,z");
}

/* ---------------- Main loop -------------------------- */
void loop() {
    static uint32_t next_us = micros();

    /* command handling */
    while (Serial.available()) {
        char c = Serial.read();
        if (c == '\n') handleCommand(); else cmd += c;
    }

    /* sampling */
    uint32_t now = micros();
    if ((int32_t)(now - next_us) >= 0) {
        next_us += sample_us;

        uint8_t raw[6];
        adxlReadBurst(REG_DATAX0, raw, 6);
        int16_t rx = (raw[1] << 8) | raw[0];
        int16_t ry = (raw[3] << 8) | raw[2];
        int16_t rz = (raw[5] << 8) | raw[4];

        float x = rx * ACC_CONV;
        float y = ry * ACC_CONV;
        float z = rz * ACC_CONV;
        float t = (now - t0_us) * 1e-6f;

        Serial.print(t, 6);  Serial.print(',');
        Serial.print(x, 6);  Serial.print(',');
        Serial.print(y, 6);  Serial.print(',');
        Serial.println(z, 6);
    }
}
