/*
  Object color sampler
  --------------------
  Samples the color of objects and outputs CSV logfile to serial console

  Hardware: Arduino Nano 33 BLE Sense board.

  Usage: Place object of interest to the color sensor

  This example code is in the public domain.

  Taken and modified from 
  https://create.arduino.cc/editor/TensorFlowExamples/ca761558-13ed-4190-baee-89ced06147c3/preview
*/

#include <Arduino_APDS9960.h>

void setup()
{

    Serial.begin(9600);

    // Check if the Serial port is ready
    while (!Serial)
    {
    };

    // Check if the color sensor is ready
    if (!APDS.begin())
    {
        Serial.println("Error initializing APDS9960 sensor.");
    }

    // Print the header
    Serial.println("Red,Green,Blue,RedRatio,GreenRatio,BlueRatio,Class");
}

void loop()
{
    // a is the ambient light intensity
    int r, g, b, a, p;
    float sum;

    // Check if both color and proximity data sample is available.
    while (!APDS.colorAvailable() || !APDS.proximityAvailable())
    {
    }

    // Read the color and proximity sensor.
    APDS.readColor(r, g, b, a);
    sum = r + g + b;
    p = APDS.readProximity();

    // if object is close and well enough illumated
    if (p == 0 && a > 10 && sum > 0)
    {

        float redRatio = r / sum;
        float greenRatio = g / sum;
        float blueRatio = b / sum;

        // Print the data in CSV format; the second argument is the number's precision.
        Serial.print(r);
        Serial.print(',');
        Serial.print(g);
        Serial.print(',');
        Serial.print(b);
        Serial.print(',');
        Serial.print(redRatio, 3);
        Serial.print(',');
        Serial.print(greenRatio, 3);
        Serial.print(',');
        Serial.print(blueRatio, 3);
        Serial.print(',');
        // This number is the class. Remember to change it!
        Serial.print('0');
        Serial.println();
    }
}