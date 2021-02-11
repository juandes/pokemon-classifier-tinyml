/*
  Object classifier by color
  --------------------------

  Uses RGB color sensor input to Neural Network to classify objects
  Outputs object class to serial using unicode emojis

  Note: The direct use of C/C++ pointers, namespaces, and dynamic memory is generally
        discouraged in Arduino examples, and in the future the TensorFlowLite library
        might change to make the sketch simpler.

  Hardware: Arduino Nano 33 BLE Sense board.

  Created by Don Coleman, Sandeep Mistry
  Adapted by Dominic Pajak

  This example code is in the public domain.

  Example inspired by
  https://create.arduino.cc/editor/TensorFlowExamples/8508c70f-5155-4e3b-b982-c5f6bd36ea5c/preview.
  I've modified it to suit my use case.
*/

#include <TensorFlowLite.h>

#include "tensorflow/lite/micro/all_ops_resolver.h"
#include "tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "tensorflow/lite/version.h"
#include "Arduino_APDS9960.h"
#include "model.h"

// Set up logging
tflite::MicroErrorReporter tflErrorReporter;

// Add all the TensorFlow Lite Micro operations
tflite::AllOpsResolver tflOpsResolver;

const tflite::Model *tflModel = nullptr;
tflite::MicroInterpreter *tflInterpreter = nullptr;
TfLiteTensor *tflInputTensor = nullptr;
TfLiteTensor *tflOutputTensor = nullptr;

// Create a static memory buffer for TFLM, the size may need to
// be adjusted based on the model you are using.
constexpr int tensorArenaSize = 8 * 1024;
byte tensorArena[tensorArenaSize];

void setup()
{
    Serial.begin(9600);
    while (!Serial)
    {
    };

    Serial.println("Object classification using RGB color sensor");
    Serial.println("--------------------------------------------");
    Serial.println("Arduino Nano 33 BLE Sense running TensorFlow Lite Micro");
    Serial.println("");

    if (!APDS.begin())
    {
        Serial.println("Error initializing APDS9960 sensor.");
    }

    // Initialize the led's.
    pinMode(LEDR, OUTPUT);
    pinMode(LEDG, OUTPUT);
    //pinMode(LEDB, OUTPUT);

    // Ensure it is off by default
    // On the Arduino NANO 33 BLE Sense, HIGH is off.
    digitalWrite(LEDR, HIGH);
    digitalWrite(LEDG, HIGH);

    // get the TFL representation of the model byte array
    tflModel = tflite::GetModel(model);
    if (tflModel->version() != TFLITE_SCHEMA_VERSION)
    {
        Serial.println("Model schema mismatch!");
        while (1)
            ;
    }

    // Create an interpreter to run the model
    tflInterpreter = new tflite::MicroInterpreter(tflModel, tflOpsResolver, tensorArena, tensorArenaSize, &tflErrorReporter);

    // Allocate memory for the model's input and output tensors
    tflInterpreter->AllocateTensors();

    // Get pointers for the model's input and output tensors
    tflInputTensor = tflInterpreter->input(0);
    tflOutputTensor = tflInterpreter->output(0);
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
    p = APDS.readProximity();
    sum = r + g + b;

    // Check if there's an object close and well illuminated enough.
    if (p == 0 && a > 10 && sum > 0)
    {

        // Normalize the values.
        float redRatio = r / sum;
        float greenRatio = g / sum;
        float blueRatio = b / sum;

        // Input sensor data to the loaded model.
        tflInputTensor->data.f[0] = redRatio;
        tflInputTensor->data.f[1] = greenRatio;
        tflInputTensor->data.f[2] = blueRatio;

        // Invoke the inference.
        // This is a great guide explaining the process
        // https://www.tensorflow.org/lite/guide/inference
        TfLiteStatus invokeStatus = tflInterpreter->Invoke();
        if (invokeStatus != kTfLiteOk)
        {
            Serial.println("Invoke failed!");
            while (1)
                ;
            return;
        }

        // 0.50 is my threshold
        if (tflOutputTensor->data.f[0] < 0.50)
        {
            Serial.print("Pikachu: ");
            // Turn on the red and green LEDs to get yellow.
            digitalWrite(LEDR, LOW);
            digitalWrite(LEDG, LOW);
        }
        else
        {
            Serial.print("Bulbasaur: ");
            // Turn on the green LED.
            digitalWrite(LEDG, LOW);
        }

        Serial.print(float(tflOutputTensor->data.f[0]), 5);
        Serial.println();

        // Wait until the sensor does not detect the object.
        while (!APDS.proximityAvailable() || (APDS.readProximity() == 0))
        {
        }

        // Turn off the LEDs.
        digitalWrite(LEDR, HIGH);
        digitalWrite(LEDG, HIGH);
    }
}