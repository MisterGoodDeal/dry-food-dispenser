/*
  This a simple example of the aREST Library for Arduino (Uno/Mega/Due/Teensy)
  using the Ethernet library (for example to be used with the Ethernet shield).
  See the README file for more details.

  Written in 2014 by Marco Schwartz under a GPL license.
*/

// Libraries
#include <SPI.h>
#include <Ethernet.h>
#include <aREST.h>
#include <avr/wdt.h>

// Enter a MAC address for your controller below.
byte mac[] = {0x90, 0xA2, 0xDA, 0x0E, 0xFE, 0x40};

// IP address in case DHCP fails
IPAddress ip(192, 168, 1, 222);

// Ethernet server
EthernetServer server(80);

// Create aREST instance
aREST rest = aREST();

// Pin used
const int stepPin = A0;
const int dirPin = A1;
const int button = 8;

// Variables to be exposed to the API
int amountToDispense = 0;

// Declare functions to be exposed to the API
int setAmount(String count);

void setup(void)
{
  // Set pinMode
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  pinMode(button, INPUT_PULLUP);

  // Start Serial
  Serial.begin(115200);

  // Function to be exposed
  rest.function("one", setAmount);
  // rest.function("two", setAmount(portion2));
  // rest.function("three", setAmount(portion3));
  // rest.function("four", setAmount(portion4));

  // Give name & ID to the device (ID should be 6 characters long)
  rest.set_id("001");
  rest.set_name("dry_food_dispenser");

  // Start the Ethernet connection and the server
  if (Ethernet.begin(mac) == 0)
  {
    Serial.println("Failed to configure Ethernet using DHCP");
    // no point in carrying on, so do nothing forevermore:
    // try to congifure using IP address instead of DHCP:
    Ethernet.begin(mac, ip);
  }
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());

  // Start watchdog
  wdt_enable(WDTO_4S);
}

void loop()
{
  // listen for incoming clients
  EthernetClient client = server.available();
  rest.handle(client);
  wdt_reset();

  if (digitalRead(button) == LOW)
  {
    setAmount("50");
  }
}

// Custom function accessible by the API
int feed(int amount)
{
  digitalWrite(dirPin, LOW); //Changes the rotations direction
  for (int x = 0; x < amount; x++)
  {
    digitalWrite(stepPin, HIGH);
    delay(8);
    digitalWrite(stepPin, LOW);
    delay(8);
  }
  Serial.println("feeded");
  amountToDispense = 0;
  return 1;
}

int setAmount(String count)
{
  int state = count.toInt();
  digitalWrite(dirPin, LOW); //Changes the rotations direction
  for (int x = 0; x < state; x++)
  {
    digitalWrite(stepPin, HIGH);
    delay(10);
    digitalWrite(stepPin, LOW);
    delay(10);
  }
  return state;
}
