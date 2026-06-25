#include "SMSManager.h"

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>

void SMSManager::begin() {
    // No persistent client to initialize for REST-based gateways
}

static String urlEncode(const String &str) {
    String encoded = "";
    for (size_t i = 0; i < str.length(); ++i) {
        char c = str.charAt(i);
        if (c == ' ') encoded += '+';
        else if (c == '\n') encoded += "%0A";
        else encoded += c;
    }
    return encoded;
}

bool SMSManager::sendSMS(const String &number, const String &message) {
    if (!TWILIO_ENABLED) {
        Serial.println("SMSManager: Twilio disabled in Config.h");
        return false;
    }

    String account = TWILIO_ACCOUNT_SID;
    String token = TWILIO_AUTH_TOKEN;
    if (account.length() == 0 || token.length() == 0) {
        Serial.println("SMSManager: Twilio credentials not set");
        return false;
    }

    String url = String("https://api.twilio.com/2010-04-01/Accounts/") + account + "/Messages.json";

    WiFiClientSecure *client = new WiFiClientSecure();
    client->setInsecure(); // For production, validate certs

    HTTPClient http;
    if (!http.begin(*client, url)) {
        Serial.println("SMSManager: HTTP begin failed");
        delete client;
        return false;
    }

    http.setAuthorization(account.c_str(), token.c_str());
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    String body = "To=" + urlEncode(number) + "&From=" + urlEncode(String(TWILIO_FROM_NUMBER)) + "&Body=" + urlEncode(message);

    int httpCode = http.POST(body);
    bool ok = false;
    if (httpCode >= 200 && httpCode < 300) {
        Serial.printf("SMSManager: sent OK (%d)\n", httpCode);
        ok = true;
    } else {
        Serial.printf("SMSManager: send failed (%d)\n", httpCode);
        String resp = http.getString();
        Serial.println(resp);
    }

    http.end();
    delete client;
    return ok;
}
