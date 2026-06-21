#pragma once

#include <Arduino.h>
#include "Config.h"

class EmailManager {
public:
    void begin();
    bool sendAlert(const String &subject, const String &body);
};
