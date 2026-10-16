#define BLYNK_TEMPLATE_ID   "TMPL3tWrBR-_Y"
#define BLYNK_TEMPLATE_NAME "Smart Fire Alarm"
#define BLYNK_AUTH_TOKEN    "iBsCwlFWkVTybJuXTS5B7XdZpd8zJPnm"
#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

char ssid[] = "Ram_kumar_pandey74641";
char pass[] = "Radhe Radhe";

// ---------- Pin Definitions ----------
const int FLAME_PIN  = 27;   // Flame sensor DO
const int BUZZER_PIN = 26;   // Buzzer driver IN
const int LED_PIN    = 25;   // LED driver IN
const int RESET_PIN  = 33;   // Reset button (other leg to GND)

// Most flame sensors output LOW when flame is detected
const int FLAME_ACTIVE = LOW;
const int RESET_ACTIVE = LOW;   // Button pressed = LOW (INPUT_PULLUP)

// ---------- Timing ----------
const unsigned long BLINK_INTERVAL = 250;   // ms, blink speed

// ---------- Global Variables ----------
BlynkTimer timer;
bool previousFire = false;
bool alertSent    = false;
bool alarmLatched = false;     // Stays ON after flame is gone
bool blinkState   = false;
unsigned long lastBlink = 0;

// ---------- Alarm Output ----------
void setAlarmOutput(bool on)
{
  if (on) {
    // Blink buzzer and LED together
    unsigned long now = millis();
    if (now - lastBlink >= BLINK_INTERVAL) {
      lastBlink = now;
      blinkState = !blinkState;
    }
    digitalWrite(BUZZER_PIN, blinkState ? HIGH : LOW);
    digitalWrite(LED_PIN,    blinkState ? HIGH : LOW);
  } else {
    blinkState = false;
    digitalWrite(BUZZER_PIN, LOW);
    digitalWrite(LED_PIN,    LOW);
  }
}

// ---------- Fire Check Function ----------
void checkFire()
{
  bool fireDetected = (digitalRead(FLAME_PIN) == FLAME_ACTIVE);

  // Fire just detected: latch the alarm and notify
  if (fireDetected && !previousFire) {
    alarmLatched = true;
    Serial.println("FIRE ALERT!");
  }

  // Send Blynk notification only once per fire event
  if (fireDetected && !alertSent && Blynk.connected()) {
    Blynk.logEvent("fire_alert", "Flame detected! Check the area immediately.");
    alertSent = true;
  }

  // Fire gone: alarm stays latched until reset
  if (!fireDetected && previousFire) {
    alertSent = false;
    Serial.println("Flame gone. Alarm still latched until reset.");
  }

  // Reset button: clears the latch if flame is no longer present
  if (digitalRead(RESET_PIN) == RESET_ACTIVE) {
    if (!fireDetected) {
      alarmLatched = false;
      Serial.println("Alarm reset.");
    }
  }

  previousFire = fireDetected;
}

// ---------- Setup ----------
void setup()
{
  Serial.begin(115200);

  pinMode(FLAME_PIN,  INPUT);
  pinMode(RESET_PIN,  INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_PIN,    OUTPUT);

  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(LED_PIN,    LOW);

  WiFi.begin(ssid, pass);
  Blynk.config(BLYNK_AUTH_TOKEN);

  timer.setInterval(300L, checkFire);
} 

// ---------- Loop ----------
void loop()
{
  Blynk.run();
  timer.run();

  // Blink while fire is present or alarm is latched
  bool fireNow = (digitalRead(FLAME_PIN) == FLAME_ACTIVE);
  setAlarmOutput(fireNow || alarmLatched);
}