#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <heartRate.h>
#include <spo2_algorithm.h>
#include <MAX30105.h>
#include <WiFi.h> // ESP32

#include <Firebase.h>
#include <Firebase_ESP_Client.h>
// Provide the token generation process info.
#include "addons/TokenHelper.h"
// Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

// Sensor pin
#define LM35_PIN 39

// Wi-Fi
#define WIFI_SSID "Wifi_name"
#define WIFI_PASSWORD "Wifi_passowrd"

// Firebase
#define API_KEY "API_KEY"
#define DATABASE_URL "DATABASE_URL"

FirebaseData readings;
FirebaseAuth auth;
FirebaseConfig config;

bool signupOk = false;

MAX30105 particleSensor;

const int32_t buffer_length = 20;
int32_t spo2;
int8_t valid_spo2;
int32_t heart_rate;
int8_t valid_heart_rate;

uint32_t ir_buffer[buffer_length];
uint32_t red_buffer[buffer_length];

byte led_brightness = 20;
byte sample_avg = 4;
byte led_mode = 2;
byte sample_rate = 100;
short pulse_width = 215;
short adc_range = 4096;

char heart_rate_sentence[30];
char spo2_sentence[30];
char temperature_sentence[30];

short adc_read;
short voltage;
float temp;

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// I²C Pins for ESP32
#define SDA_PIN 21
#define SCL_PIN 22

void initialize_display() {
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }

  display.clearDisplay();
  display.display();
  write_on_display(2, "Done", 30, 20, 0);
  delay(300);
}

void write_on_display(char text_size, char* value, char x_pos, char y_pos, char cl) {
  if (cl == 1) {
    display.clearDisplay();
  }

  display.setTextSize(text_size);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(x_pos, y_pos);
  display.println(value);
  display.display();
}

byte MAX30102_init() {
  if (!particleSensor.begin(Wire, I2C_SPEED_STANDARD)) {
    Serial.println("Sensor not found");
    while (1);
  }

  particleSensor.setup(led_brightness, sample_avg, led_mode, sample_rate, pulse_width, adc_range);
  particleSensor.setPulseAmplitudeRed(0x1F);
  particleSensor.setPulseAmplitudeGreen(0);

  return 1;
}


void wifi_init(char *ssid, char *password) {
  WiFi.begin(ssid, password);
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(200);
  }
  Serial.println();
  Serial.print("Connected to the IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();
}

void firebase_signup() {
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("Ok");
    signupOk = true;
  } else {
    Serial.println(config.signer.signupError.message.c_str());
  }

  config.token_status_callback = tokenStatusCallback;  // See addons/TokenHelper.h

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void setup() {
  Wire.begin(SDA_PIN, SCL_PIN);
  Serial.begin(115200);

  // Wi-Fi init
  wifi_init(WIFI_SSID, WIFI_PASSWORD);

  // Firebase init
  firebase_signup();

  pinMode(LM35_PIN, INPUT);

  initialize_display();

  if (MAX30102_init()) {
    Serial.println("The sensor is initialized successfully");
    write_on_display(1, "Sensor initialized", 0, 20, 0);
    delay(200);
    display.clearDisplay();
    display.display();
  } else {
    while (1) {
      Serial.println("Not initialized!");
      delay(200);
    }
  }
}

void loop() {
  // Temperature reading
  adc_read = analogRead(LM35_PIN);
  voltage = (adc_read * 3300) / 4096;
  temp = voltage / 10.0;


  static byte i = 0;
  
  ir_buffer[i] = particleSensor.getIR();
  red_buffer[i] = particleSensor.getRed();

  Serial.print("IR: ");
  Serial.print(ir_buffer[i]);
  Serial.print(", RED: ");
  Serial.println(red_buffer[i++]);

  i %= buffer_length;  // i = i % buffer_length

  if(i == 0) {
    // Calculate heart rate and SpO₂
      maxim_heart_rate_and_oxygen_saturation(ir_buffer, buffer_length, red_buffer, &spo2, &valid_spo2, &heart_rate, &valid_heart_rate);
    
      if (valid_heart_rate && heart_rate > 0) {
        sprintf(heart_rate_sentence, "Heart rate: %d", heart_rate);
      } else {
        sprintf(heart_rate_sentence, "Heart rate: --");
      }
      Serial.println(heart_rate_sentence);
      write_on_display(1, heart_rate_sentence, 0, 5, 1);
    
      if (valid_spo2 && spo2 > 0) {
        sprintf(spo2_sentence, "SpO2: %d%%", spo2);
      } else {
        sprintf(spo2_sentence, "SpO2: --");
      }
      Serial.println(spo2_sentence);
      write_on_display(1, spo2_sentence, 0, 20, 0);
    
      sprintf(temperature_sentence, "Temperature: %.1f C", temp);
      Serial.println(temperature_sentence);
      write_on_display(1, temperature_sentence, 0, 35, 0);
    
      // Firebase upload
      if (valid_heart_rate && valid_spo2) {
        if (Firebase.ready() && signupOk) {
          Firebase.RTDB.setInt(&readings, "Measurements/Heart_rate", heart_rate);
          Firebase.RTDB.setInt(&readings, "Measurements/SPO2", spo2);
          Firebase.RTDB.setFloat(&readings, "Measurements/Temperature", temp);
        } else {
          Serial.println("Reason: " + readings.errorReason());
        }
     } 

     delay(400);
  }

  delay(20);
}
