#pragma once

#include <Arduino.h>
#include "Config.h"

class SMSManager {
public:
    void begin();
    bool sendSMS(const String &number, const String &message);
};
