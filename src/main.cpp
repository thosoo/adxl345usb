// adxl345usb firmware – Fly‑ADXL345‑USB (RP2040)
// =================================================
// * Builds with the *standard* Arduino‑MBed core.
// * Streams ADXL345 data exactly like navaismo/adxl345spi – header
//   "time,x,y,z" followed by CSV lines, default 250 Hz.
// * Optional dual‑sensor mode outputs "time,x0,y0,z0,x1,y1,z1".
// * Uses an mbed::SPI instance.
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

/* ───── Board-specific pins (overridable via build_flags) ─────────── */
#ifndef CS_PIN
#  define CS_PIN 9          // legacy default (single sensor)
#endif
#ifndef SCK_PIN
#  define SCK_PIN 10
#endif
#ifndef MOSI_PIN
#  define MOSI_PIN 11
#endif
#ifndef MISO_PIN
#  define MISO_PIN 12
#endif

#ifdef DUAL_SPI
#  ifndef CS0_PIN
#    define CS0_PIN 17
#  endif
#  ifndef SCK0_PIN
#    define SCK0_PIN 18
#  endif
#  ifndef MOSI0_PIN
#    define MOSI0_PIN 19
#  endif
#  ifndef MISO0_PIN
#    define MISO0_PIN 16
#  endif

#  ifndef CS1_PIN
#    define CS1_PIN 13
#  endif
#  ifndef SCK1_PIN
#    define SCK1_PIN 10
#  endif
#  ifndef MOSI1_PIN
#    define MOSI1_PIN 11
#  endif
#  ifndef MISO1_PIN
#    define MISO1_PIN 12
#  endif
#endif

/* SPI instance(s) on the chosen GPIOs */
#ifdef DUAL_SPI
static mbed::SPI mbedSpi0(
    digitalPinToPinName(MOSI0_PIN),
    digitalPinToPinName(MISO0_PIN),
    digitalPinToPinName(SCK0_PIN));
static mbed::SPI mbedSpi1(
    digitalPinToPinName(MOSI1_PIN),
    digitalPinToPinName(MISO1_PIN),
    digitalPinToPinName(SCK1_PIN));
#else
static mbed::SPI spiPort(
    digitalPinToPinName(MOSI_PIN),
    digitalPinToPinName(MISO_PIN),
    digitalPinToPinName(SCK_PIN));
#endif

constexpr uint32_t SPI_SPEED = 2'000'000;   // 2 MHz

/* conversion: ±16 g full‑res → 3.9 mg/LSB → 1/4096 g/LSB */
constexpr float ACC_CONV = 16.0f / 4096.0f;

volatile uint32_t sample_us = 4'000;        // 250 Hz default

/* ---------------- Low‑level helpers ------------------ */
#ifdef DUAL_SPI
inline void cs0Low()  { digitalWrite(CS0_PIN, LOW);  }
inline void cs0High() { digitalWrite(CS0_PIN, HIGH); }
inline void cs1Low()  { digitalWrite(CS1_PIN, LOW);  }
inline void cs1High() { digitalWrite(CS1_PIN, HIGH); }
#else
inline void csLow()  { digitalWrite(CS_PIN, LOW);  }
inline void csHigh() { digitalWrite(CS_PIN, HIGH); }
#endif

#ifdef DUAL_SPI
void adxlWrite(mbed::SPI &port, int cs_pin, uint8_t reg, uint8_t val) {
    digitalWrite(cs_pin, LOW);
    port.write(reg);
    port.write(val);
    digitalWrite(cs_pin, HIGH);
}

void adxlReadBurst(mbed::SPI &port, int cs_pin, uint8_t startReg, uint8_t *buf, size_t len) {
    digitalWrite(cs_pin, LOW);
    port.write(0x80 | 0x40 | startReg);   // READ | MULTI
    for (size_t i = 0; i < len; ++i) buf[i] = port.write(0);
    digitalWrite(cs_pin, HIGH);
}
#else
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
#endif

/* ---------------- Sensor initialisation -------------- */
#ifdef DUAL_SPI
void setupADXL(mbed::SPI &port, int cs_pin, uint8_t bw_rate_code = 0x0A) {
    pinMode(cs_pin, OUTPUT);
    digitalWrite(cs_pin, HIGH);

    port.frequency(SPI_SPEED);
    port.format(8 /*bits*/, 3 /*mode*/);  // SPI mode 3

    adxlWrite(port, cs_pin, REG_DATA_FORMAT, 0x0B); // full-res ±16g
    adxlWrite(port, cs_pin, REG_BW_RATE, bw_rate_code);
    adxlWrite(port, cs_pin, REG_POWER_CTL, 0x08);   // measurement mode
}
#else
void setupADXL(uint8_t bw_rate_code = 0x0A) { // 0x0A = 100 Hz placeholder
    pinMode(CS_PIN, OUTPUT);
    csHigh();

    spiPort.frequency(SPI_SPEED);
    spiPort.format(8 /*bits*/, 3 /*mode*/);  // SPI mode 3

    adxlWrite(REG_DATA_FORMAT, 0x0B);   // full‑res ±16 g
    adxlWrite(REG_BW_RATE, bw_rate_code);
    adxlWrite(REG_POWER_CTL, 0x08);     // measurement mode
}
#endif

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
#ifdef DUAL_SPI
            Serial.println("time,x0,y0,z0,x1,y1,z1");
#else
            Serial.println("time,x,y,z");     // header for new block
#endif
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

#ifdef DUAL_SPI
    setupADXL(mbedSpi0, CS0_PIN);
    setupADXL(mbedSpi1, CS1_PIN);
    t0_us = micros();
    Serial.println("time,x0,y0,z0,x1,y1,z1");
#else
    setupADXL();
    t0_us = micros();
    Serial.println("time,x,y,z");
#endif
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

#ifdef DUAL_SPI
        uint8_t raw0[6], raw1[6];
        adxlReadBurst(mbedSpi0, CS0_PIN, REG_DATAX0, raw0, 6);
        adxlReadBurst(mbedSpi1, CS1_PIN, REG_DATAX0, raw1, 6);

        int16_t rx0 = (raw0[1] << 8) | raw0[0];
        int16_t ry0 = (raw0[3] << 8) | raw0[2];
        int16_t rz0 = (raw0[5] << 8) | raw0[4];
        int16_t rx1 = (raw1[1] << 8) | raw1[0];
        int16_t ry1 = (raw1[3] << 8) | raw1[2];
        int16_t rz1 = (raw1[5] << 8) | raw1[4];

        float x0 = rx0 * ACC_CONV;
        float y0 = ry0 * ACC_CONV;
        float z0 = rz0 * ACC_CONV;
        float x1 = rx1 * ACC_CONV;
        float y1 = ry1 * ACC_CONV;
        float z1 = rz1 * ACC_CONV;
        float t = (now - t0_us) * 1e-6f;

        Serial.print(t, 6);  Serial.print(',');
        Serial.print(x0, 6); Serial.print(',');
        Serial.print(y0, 6); Serial.print(',');
        Serial.print(z0, 6); Serial.print(',');
        Serial.print(x1, 6); Serial.print(',');
        Serial.print(y1, 6); Serial.print(',');
        Serial.println(z1, 6);
#else
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
#endif
    }
}
