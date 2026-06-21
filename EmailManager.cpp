#include "EmailManager.h"
#include <WiFiClientSecure.h>

void EmailManager::begin() {
    // Initialize any email-related resources if needed
}

bool EmailManager::sendAlert(const String &subject, const String &body) {
    // Placeholder: implement SMTP/TLS send using configured server in Config.h
    (void)subject;
    (void)body;
    return false;
}
