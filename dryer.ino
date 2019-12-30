#define DEBUG 0
#define dbg_print(str) if (DEBUG) Serial.println(str);

void setup() {
    Serial.begin(9600);
}

void loop() {
    delay(1000);
}