# WeatherPi

This is the Rasperry pi zero part of the Home Climate Monitoring System.

Discription:
The raspberry pi zero served as an access point for sensor nodes each powered by a PIC18F26Q10.
The sensort nodes when 1st powered up will request the time and date from the WeatherPi. The WeatherPi will then return current system time and date.
*All communication between the WeatherPi and sensor nodes is done with NRF24L01+ modules.*
The Sensor nodes transmit the data at set intervals depending on the node ID. The node ID will also manage the timing when that node may trabsmit data.

The application used three port:
- UDP port. This port will block the main thread and controll remote shutdown of the application. The WeatherPi will also respond with it;s ip when requested to do so by the configuration application.
- TCP client. This cient st be TCP since in the future it will send data to a DB hosted on a website. At that time I will be sending a password hash that nees to be encrypted with SSL or TSL.
- TCP Server. This will be used by the configuration application. This need to be TCP because the data needs to be encrypted with SSL in the future.

The data received from the nodes will be serialized into JSON strings to be trasmitted to a DB (This will be implimented much later, initially the JSON will be transmitted to a desktop application to be displayed)

The WeatherPi also contains a set of sensors:
- BME280 for Humidity and Pressure
- DS18B20 for temperature
- AS3539 to detect incoming thunderstorms
- Several other devices which where used more as toys and learning tools and does not form part of the functional part of applcation.

How to use:
Requirements:
- Raspberrypi
    I used a Raspberrypi zero for this application however any raspberrpi should work
- BME280 module
- DS18B20 Temperature sensor
- AS3539 Lightnigh Detector (End of life decive, might be difficult to find)
- NRF24L01+ module to receive data from sensor nodes (Design and code for the sensor node found here: https://github.com/roughleaf/WeatherPiNode)

Set up the Raspberry pi:
- Install a light (server) version of Raspbian (no known as Raspberrypi OS).
  A normal distro will work but we do not need the desktop or GUI as this device will run headless.
- Enable SSH, Onewire, SPI & I2C in the raspberry pi config.
  To setup headless add a file named SSH into the root of the boot drive right after flashing the SD card and before inserting into the pi for the 1st time.
  At this point also setup the wireless information if you are going to connect to the raspberrypi via WiFi.
- Install the PIGPIO library.

Set up visual studio for cross compile.
- One the code have been cloned the sollution can be opened by opening WeatherPi.sln
- right click on the project and click on properties. rom here expand the linke tree and go to the command line options. In the additional options testbox add the "-pthread" without the quotes. This will enable the pthread library to compile.
- In Visual Studio go to Tools -> options and scroll down to Cross Platform -> Connection Manager. Add the SSH information of the raspberry pi and add the RaspberryPi.
  If everything went well you will no be able to compile the code on the RaspberryPi.
- There are several warnings when compiling the application, mostly type conversion and narrowing. This is impractical to avoid and great care has been taken to ensure that the warnings do not result in bugs.
  
The application must be run with sudo in order to gain direct access to hardware IO.

TODO:
a. Create the TCP threads.
b. Create the methods to serialise the data into JSON
c. Create C++ wrappers for the TCP and UDP funtions
d. The SensorNodes will shortly impliment a circural buffer to save data when the weatherpi is offline, the Weatherpi needs to be abkle to receive all the data and process it appropriately.
e. Syncronise an timer interrupt function to transmit the JSON at apropriate times.
f. Spell check and format this readme file properly.


**I started to create a driver for a small OLED display presumably using the SSD1306 controller.
I do not recommend using this driver for projects, as is the driver is unreliable.
Development of this driver is not priority at this time.
