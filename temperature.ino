// This #include statement was automatically added by the Particle IDE.
#include <SparkFunMicroOLED.h>

// This #include statement was automatically added by the Particle IDE.
#include <SparkFunRHT03.h>

#include <string.h>


/////////////////////
// Pin Definitions //
/////////////////////
const int RHT03_DATA_PIN = D3; // RHT03 data pin
const int LOWER_TEMP = D2;
const int RAISE_TEMP = D4;
const int LED_PIN = D7; // LED to show when the sensor's are being read
const int mover = RX;

//DHT dht(RHT03_DATA_PIN, DHT22);

#define PIN_RESET D6  // Connect RST to pin 6
#define PIN_DC    D5  // Connect DC to pin 5 (required for SPI)
#define PIN_CS    A2 // Connect CS to pin A2 (required for SPI)
//MicroOLED oled(MODE_SPI, PIN_RESET, PIN_DC, PIN_CS);
MicroOLED oled(MODE_SPI, PIN_RESET, PIN_DC, PIN_CS);

Servo myservo;
//int currpos;

///////////////////////////
// RHT03 Object Creation //
///////////////////////////
RHT03 rht; // This creates a RTH03 object, which we'll use to interact with the sensor

float minimumTempC = 100;
float maximumTempC = 0;
float minimumTempF = 100;
float maximumTempF = 0;
float minimumHumidity = 0;
float maximumHumidity = 100;

#define PRINT_RATE 1500 // Time in ms to delay between prints.
#define ROUNDF(f, c) (((double)((int)((f) * (c))) / (c)))

char humidityS[5];
char tempFS[5];
char tempCS[5];
#define PRINT_RATE 1500 // Time in ms to delay between prints.

void setup() 
{
    strncpy(humidityS, "", 10);
    strncpy(tempFS, "", 10);
    strncpy(tempCS, "", 10);
    Particle.variable("humidity", humidityS);
    Particle.variable("temperatureF", tempFS);
    Particle.variable("temperatureC", tempCS);
    Particle.function("setTemp", setTemp);
    
    oled.begin();
    oled.setFontType(1);
    
    // Using the 'rht' object created in the global section, we'll begin by calling
    // its member function `begin`.
    // The parameter in this function is the DIGITAL PIN we're using to communicate
    // with the sensor.
    rht.begin(RHT03_DATA_PIN);  // Initialize the RHT03 sensor
    
    //dht.begin();
    
    Serial.begin(9600);
    
    myservo.attach(mover);
    myservo.write(90);
    //currpos = 90;
    delay(500);
    myservo.detach();
    
    // Don't forget to set the pin modes of our analog sensor (INPUT) and the LED (OUTPUT):
    pinMode(LED_PIN, OUTPUT); // Set the LED pin as an OUTPUT
    digitalWrite(LED_PIN, LOW); // Initially set the LED pin low -- turn the LED off.
    
    
    pinMode(LOWER_TEMP, INPUT_PULLUP); // Set the LED pin as an OUTPUT
    
    pinMode(RAISE_TEMP, INPUT_PULLUP); // Set the LED pin as an OUTPUT
    
    minimumTempC = 18.9;
    maximumTempC = 33.9;
    minimumTempF = 57;
    maximumTempF = 93;
}

void loop() 
{   
    delay(PRINT_RATE); // delay for 1s, printing too much will make the output very hard to read.
    digitalWrite(LED_PIN, HIGH); // Turn the LED on -- it'll blink whenever the sensor is being read.
    
    // Use the RHT03 member function `update()` to read new humidity and temperature values from the sensor.
    // There's a chance the reading might fail, so `update()` returns a success indicator. It'll return 1
    // if the update is successful, or a negative number if it fails.
    int update = rht.update();

    if (update == 1) // If the update succeeded, print out the new readings:
    {
        double humidity;
        double tempF;
        double tempC;
        // The `humidity()` RHT03 member function returns the last successfully read relative
        // humidity value from the RHT03.
        // It'll return a float value -- a percentage of RH between 0-100.
        // ONLY CALL THIS FUNCTION AFTER SUCCESSFULLY RUNNING rht.update()!.
        humidity = rht.humidity();
        if(minimumHumidity <= humidity && humidity <= maximumHumidity)
            sprintf(humidityS, "%.1f%s", humidity, "% H");
        if (humidity > maximumHumidity) maximumHumidity = 100;
        if (humidity < minimumHumidity) minimumHumidity = 0;
        
        // The `tempF()` RHT03 member function returns the last succesfully read 
        // farenheit temperature value from the RHT03.
        // It returns a float variable equal to the temperature in Farenheit.
        // ONLY CALL THIS FUNCTION AFTER SUCCESSFULLY RUNNING rht.update()!.
        tempF = rht.tempF();
        if(minimumTempF <= tempF && tempF <= maximumTempF)
            sprintf(tempFS, "%.1f%s", tempF, " F");
        if (tempF > maximumTempF) maximumTempF = 93;
        if (tempF < minimumTempF) minimumTempF = 57;
        // Do some math to calculate the max/min tempF
        
        // `tempC()` works just like `tempF()`, but it returns the temperature value in
        // Celsius.
        // ONLY CALL THIS FUNCTION AFTER SUCCESSFULLY RUNNING rht.update()!.
        tempC = rht.tempC();
        if(minimumTempC <= tempC && tempC <= maximumTempC)
            sprintf(tempCS, "%.1f%s", tempC, " C");
        // Do some math to calculate the max/min tempC
        if (tempC > maximumTempC) maximumTempC = 33.9;
        if (tempC < minimumTempC) minimumTempC = 18.9;
        
        Serial.printlnf("Success: %f\n, %f\n, %f\n", tempF, tempC, humidity);
        Serial.println(); // Print a blank line
        oled.clear(PAGE);
        oled.setCursor(0, 0);
        oled.print(tempFS);
        oled.setCursor(0, 15);
        oled.print(tempCS);
        oled.setCursor(0,30);
        oled.print(humidityS);
        oled.display();
        
    }
    else // If the update failed, give the sensor time to reset:
    {
        Serial.println("Error reading from the RHT03."); // Print an error message
        Serial.println(); // Print a blank line
        
        delay(RHT_READ_INTERVAL_MS); // The RHT03 needs about 1s between read attempts
    }
    digitalWrite(LED_PIN, LOW); // Turn the LED off
    
    String tempSetting = "0";
    
    int raising; 
    raising = digitalRead(RAISE_TEMP);
    if(raising == LOW)
    {   // If we push down on the push button
        tempSetting = "90";
        setTemp(tempSetting);
    }
    
    int lowering; 
    lowering = digitalRead(LOWER_TEMP);
    if(lowering == LOW)
    {   // If we push down on the push button
        tempSetting = "60";
        setTemp(tempSetting);
    }
}

int setTemp(String num){
    double amount = atof(num);
    myservo.attach(mover);
    int pos = (amount-75)*5 + 90;
    Serial.printlnf("Pos: %d", pos);
    if(amount){
        if(pos > 180) return -1;
        if(pos < 0) return -1;
        Serial.printlnf("Pos after: %d", pos);
        myservo.write(pos);
        delay(750);
        myservo.detach();
        
    }
    return 75 + (pos-90)/5;
}