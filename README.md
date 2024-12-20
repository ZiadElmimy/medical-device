# Medical device

It's a prototype for a wearable medical device that measures the heart rate, SpO2, and temperature of the body.

### Components used
* ESP32S board
* OLED display 
* MAX30102 (heart rate and SpO2 sensor)
* LM35 (temperature sensor)

### Steps
#### Building the embedded system or IoT device
* Interfacing the sensors to the ESP32 board and get the readings
* Display the readings on the OLED display
* Connecting the board to WiFi to be the gateway of the data to the internet

#### Connecting between the IoT device and the user interface for monitoring
* Connecting the board to Firebase real time database
* Get the SDK of the firebase to be able to connect third-part application or website to access the data of it
* Build a custom website using HTML, CSS, and JavaScript to monitor the data
* Fetch the data from Firebase RTDB and monitor them on the website made