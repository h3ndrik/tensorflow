/* Copyright 2019 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#include "tensorflow/lite/experimental/micro/examples/hello_world/output_handler.h"

#include "Arduino.h"
#include "tensorflow/lite/experimental/micro/examples/hello_world/constants.h"

// Use first channel of 16 channels
#define LEDC_CHANNEL_0     0

// Use 13 bit precision for LEDC timer
#define LEDC_TIMER_13_BIT  13

// Use 5000 Hz as a LEDC base frequency
#define LEDC_BASE_FREQ     5000

// The pin of the built-in LED
int led = LED_BUILTIN;

// Set brightness of the (inbuilt) LED with PWM
// This will give a slow fade-in and fade-out animation
void HandleOutput(tflite::ErrorReporter* error_reporter, float x_value,
                  float y_value) {

  // Track whether the function has run at least once
  static bool is_initialized = false;
  // Do this only once
  if (!is_initialized) {
    // Set the LED pin to output
    pinMode(led, OUTPUT);
    // Setup timer and attach timer to a led pin
    ledcSetup(LEDC_CHANNEL_0, LEDC_BASE_FREQ, LEDC_TIMER_13_BIT);
    ledcAttachPin(led, LEDC_CHANNEL_0);
    is_initialized = true;
  }

  // Calculate the brightness of the LED such that y=-1 is fully off
  // and y=1 is fully on. The LED's brightness can range from 0-255.
  int brightness = (int)(127.5f * (y_value + 1));

  // Set the brightness of the LED.
  ledcWrite(LEDC_CHANNEL_0, brightness);

  // Log the current brightness value for display in the Arduino plotter
  error_reporter->Report("%d\n", brightness);
}
