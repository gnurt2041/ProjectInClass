# Use STM32F103C8T6 to read data(temperature, humidity) from DHT11 then send it to the web to analysis

## Illustration

<img src="https://github.com/gnurt2041/ProjectInClass/blob/main/DHT11_STM32/Image/Diagram.png" width="900">

## Hardware info

| Hardware                        | Count | Uses                                     |
|---------------------------------|-------|------------------------------------------|
| `STM32F103C8T6`                 | 1     | Receive data and process                 |
| `DHT11`                         | 1     | Read temperature and humidity            |
| `LCD 16x2`                      | 1     | Display the data                         |
| `Module I2C Arduino`            | 1     | Connect the LCD to STM32 microcontroller |
| `ESP8266`                       | 1     | Connect to the internet                  |
| `3.7V Lithium-Ion Battery Pack` | 2     | Power                                    |

## Software Info

**OS:** Windows
| Software        | Uses                          |
|-----------------|-------------------------------|
| `STM32CubeMX`   | Setting and define pins, bus  |
| `Keil uVision5` | Coding and debug              |
| `Arduino IDE`   | Coding and debug              |
| `ST-Link STM32` | Load the code to STM32        |
| `Proteus`       | Circuit design and Simulation |

## Circuit Schematic :
<img align='center' src="https://github.com/gnurt2041/ProjectInClass/blob/main/DHT11_STM32/Image/Circuit%20Diagram.png" width="500">

## Result: 
<img align='center' src="https://github.com/gnurt2041/ProjectInClass/blob/main/DHT11_STM32/Image/result.png" width="500">

## How to use:
