#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <DHT.h>

// oled display stuff
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32' However, mine is a 128x64 at 0x3C. Idk why.
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
unsigned long display_timer = 400; // Sets the frame rate.
unsigned long display_start_time;  // Holds the millis() time chosen to start timing.
// tri-circle icon variables
unsigned int radius = 4;
unsigned int y0 = 3, y1 = 9, y2 = 9;

// DHT stuff
const int dataPin = 8;
float humidity = 0, temperature = 0;
float min_temp = 100, max_temp = 0; //temp solution to a start value for the mins.
float min_humid = 100, max_humid = 0;
unsigned long dht_timer = 2000; // milliseconds
unsigned long dht_start_time;
DHT dht(dataPin, DHT11);

// Buzzer stuff
const int buzzer_pin = 10;

//---------------------------------------------------------------------------

void setup() {
  //init oled display and dht temperature sensor.
  Serial.begin(9600);
  Serial.print("PC Air Temp START!");
  pinMode(buzzer_pin, OUTPUT);
  dht.begin(); // Start the temperature sensor
  dht_start_time = millis(); // Start the timer.
  display_start_time = millis();

  //Start the display
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  
  Serial.println("Setup complete");
  play(100, 1, 1);
    play(100, 2, 2);
    play(100, 1, 1);
    play(100, 2, 2);
    play(100, 1, 1);
    play(50, 2, 1);
    play(100, 3, 2);
    play(100, 4, 4);
}

void loop() {
  if ((millis() - dht_start_time) > dht_timer)
  {
    // Restart timer;
    dht_start_time = millis();

    dht_data_read();
    dht_fail_check();
    dht_min_max_check();
    
    serial_display_stats();
  }
  
  if ((millis() - display_start_time) > display_timer)
  {
    // Restart display timer.
    display_start_time = millis();
    
    radius = radius + 1;
    radius = radius % 4;
    
    display.clearDisplay();
    display_title();
    display_icon();
    display_stats();
    display.display();  
  }
  
}
//-----------------------------------------------------------------------------

void play(int ms, int delay1, int delay2) {
    for (int i = 1; i <= ms; i++) {
        digitalWrite(buzzer_pin, HIGH);
        delay(delay1);
        digitalWrite(buzzer_pin, LOW);
        delay(delay2);
    }
}

void display_title()
{
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,2);
  display.println("PC Env Monitor");
}

void display_icon()
{
  display.drawCircle(115,y0,radius, SSD1306_WHITE);
  display.drawCircle(111,y1,radius, SSD1306_WHITE);
  display.drawCircle(119,y2,radius, SSD1306_WHITE);
}

void display_stats()
{
  display.setTextSize(2);             // Normal 1:1 pixel scale
  display.setCursor(0,13);             // Start at top-left corner
  display.print(F("H: "));
  display.print(humidity);
  display.print("%\n");
  display.print(F("T: "));
  display.print(temperature);
  display.print("C\n");

  display.setTextSize(1);
  display.setCursor(0,45);
  display.print("Mn/Mx H: ");
  display.print(min_humid);
  display.print("/");
  display.print(max_humid);

  display.print("\nMn/Mx T: ");
  display.print(min_temp);
  display.print("/");
  display.print(max_temp);
}

void serial_display_stats()
{
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print("°C\n");

  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.print("%\n");

  Serial.print("Min / Max Temperature: ");
  Serial.print(min_temp);
  Serial.print(" / ");
  Serial.print(max_temp);

  Serial.print("\nMin / Max Humidity:    ");
  Serial.print(min_humid);
  Serial.print(" / ");
  Serial.print(max_humid);
  Serial.println("\n");
}

void dht_fail_check()
{
  // check for failure
  if (isnan(humidity) || isnan(temperature))
  {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }  
}

void dht_data_read()
{
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();  
}

void dht_min_max_check()
{
   // set minimum and maximum humidity and temperature
  if (humidity < min_humid)
  {
    min_humid = humidity;  
  }
  else if (humidity > max_humid)
  {
    max_humid = humidity;
  }

  if (temperature < min_temp)
  {
    min_temp = temperature;  
  }
  else if (temperature > max_temp)
  {
    max_temp = temperature;
  }  
}
