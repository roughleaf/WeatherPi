#pragma once
#include "WeatherPiSystem.hpp"
#include <iostream>

void As3935Interrupt(int gpio, int level, uint32_t tick);
void NrfInterrupt(int gpio, int level, uint32_t tick);
