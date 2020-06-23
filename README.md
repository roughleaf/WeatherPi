# WeatherPi

I used PIGPIO for accessing the GPIOs of the RaspberryPi

This is a project I started to monitor the weather.

It will consist of several satellite Raspberry pi zero W boards with the following sensors attached.
1. BME280 for relative humidity, air pressure and temperature.
2. DS18B20 for temperature.
3. MQ-135 gas sensor in combination with ADS1115 I2C ADC for air quality.
4. The satellite board outside will also feature a tipping bucket rain sensor.

A raspberry pi 3 with a 3.5" display will poll the satellite sensor raspberry pi zero boards every 10 minutes for data and display the data for each on the display.
I decided on UDP because dropped packets will not cause problems for this application.

TODO:

a. Create a web server and database to log all the sensor data.

b. Display the data on a website on my local network.

c. Develop an android app to display real time weather data on my phone when connected to my home Wi-Fi.



**I started to create a driver for a small OLED display presumably using the SSD1306 controller.
I do not recommend using this driver for projects, as is the driver is unreliable.
Development of this driver is not priority at this time.
