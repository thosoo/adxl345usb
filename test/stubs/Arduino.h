#ifndef ARDUINO_H
#define ARDUINO_H
#include <string>
#include <cstdint>
#include <vector>
#include <cstdio>
#include <cctype>

#define OUTPUT 1
#define INPUT 0
#define HIGH 1
#define LOW 0

class String {
public:
    std::string s;
    String() {}
    String(const char* str) : s(str) {}
    void trim() {
        const char* ws = " \t\r\n";
        size_t b = s.find_first_not_of(ws);
        if (b == std::string::npos) { s.clear(); return; }
        size_t e = s.find_last_not_of(ws);
        s = s.substr(b, e - b + 1);
    }
    void toUpperCase() {
        for (char &c : s) c = toupper(static_cast<unsigned char>(c));
    }
    bool startsWith(const char* prefix) const {
        return s.rfind(prefix, 0) == 0;
    }
    String substring(size_t from) const {
        return String(s.substr(from).c_str());
    }
    long toInt() const {
        return std::stol(s);
    }
    String& operator+=(char c) { s += c; return *this; }
    String& operator=(const char* str) { s = str; return *this; }
};

struct SerialStub {
    std::string out;
    std::vector<char> in;
    void begin(unsigned long) {}
    operator bool() const { return true; }
    bool available() const { return !in.empty(); }
    char read() { char c = in.front(); in.erase(in.begin()); return c; }
    void print(const char* str) { out += str; }
    void print(char c) { out += c; }
    void print(float v, int digits) { char buf[32]; std::snprintf(buf, sizeof(buf), "%.*f", digits, v); out += buf; }
    void print(const String& s) { out += s.s; }
    void println(const char* str) { out += str; out += '\n'; }
    void println(float v, int digits) { char buf[32]; std::snprintf(buf, sizeof(buf), "%.*f", digits, v); out += buf; out += '\n'; }
    void println(const String& s) { out += s.s; out += '\n'; }
};
inline SerialStub Serial;

inline int digitalPinToPinName(int pin) { return pin; }
inline void pinMode(int, int) {}
inline void digitalWrite(int, int) {}

inline uint32_t fakeMicros = 0;
inline uint32_t micros() { return fakeMicros; }

#endif
