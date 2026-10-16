
#define BLYNK_TEMPLATE_ID "TMPL3tWrBR-_Y"
#define BLYNK_TEMPLATE_NAME "Smart Fire Alarm"
#define BLYNK_AUTH_TOKEN "iBsCwlFWkVTybJuXTS5B7XdZpd8zJPnm"

#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

char ssid[] = "Ram_kumar_pandey74641";
char pass[] = "Radhe Radhe";

const int FLAME_PIN  = 27;
const int BUZZER_PIN = 26; // Driver input
const int LED_PIN    = 25; // Driver input

// Most flame sensors output LOW when flame is detected.
// Change LOW to HIGH if your sensor works oppositely.
const int FLAME_ACTIVE = LOW;

BlynkTimer timer;

bool previousFire = false;
bool alertSent = false;

void checkFire()
{
  bool fireDetected =
      (digitalRead(FLAME_PIN) == FLAME_ACTIVE);

  digitalWrite(BUZZER_PIN, fireDetected ? HIGH : LOW);
  digitalWrite(LED_PIN, fireDetected ? HIGH : LOW);

  if (fireDetected && !previousFire) {
    Serial.println("FIRE ALERT!");

    if (Blynk.connected() && !alertSent) {
      Blynk.logEvent(
        "fire_alert",
        "Flame detected! Check the area immediately."
      );
      alertSent = true;
    }
  }

  if (!fireDetected) {
    alertSent = false;
    Serial.println("No flame detected.");
  }

  previousFire = fireDetected;
}

void setup()
{
  Serial.begin(115200);

  pinMode(FLAME_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(LED_PIN, LOW);

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  timer.setInterval(300L, checkFire);
}

void loop()
{
  Blynk.run();
  timer.run();
}
