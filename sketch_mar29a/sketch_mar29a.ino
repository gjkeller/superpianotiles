/*  
* ESP32 LED Blink Example  
*/  
#define GREEN_LED_GPIO 22
#define BLUE_LED_GPIO 19

void setup() {  
 // Set pin mode  
  pinMode(GREEN_LED_GPIO,OUTPUT);  
  pinMode(BLUE_LED_GPIO,OUTPUT);  
}  
void loop() {  
 delay(500); // 500ms  
 digitalWrite(GREEN_LED_GPIO,HIGH); // Turn on LED  
 digitalWrite(BLUE_LED_GPIO,HIGH); // Turn on LED  
 delay(500); // 500ms   
 digitalWrite(GREEN_LED_GPIO,LOW); // Turn off LED  
 digitalWrite(BLUE_LED_GPIO,LOW); // Turn on LED  
}         
