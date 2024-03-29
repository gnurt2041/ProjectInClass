# Arduino Voice Controlled Car

**Demo video:** https://drive.google.com/file/d/1U-NsWH8Pc0Fs_VYXnwnphVGh6ry_q3YD/view
## Illustration

<img src="https://github.com/gnurt2041/ProjectInClass/blob/main/Voice_control_car/image/diagram.jpg" width="650"></h2>

## Hardware info

| Hardware                        | Count |
|---------------------------------|-------|
| `Arduino Uno R3`                | 1     |
| `Module Bluetooth HC-05`        | 1     |
| `DC Gear Motor with weels`      | 4     |
| `L293D Motor Driver Shield`     | 1     |
| `3.9V Lithium-Ion Battery Pack` | 3     |
| `Switch`                        | 1     |

## Software Info

**OS:** Windows
| Software      | Uses                  |
|---------------|-----------------------|
| `Arduino IDE` | Coding and display    |
| `Proteus`     | Design and simulation |
| `LTSpice`     | Design and simulation |

## Circuit and simulation

<img src="https://github.com/gnurt2041/ProjectInClass/blob/main/Voice_control_car/image/circuit.png" width="450"> <img src="https://github.com/gnurt2041/ProjectInClass/blob/main/Voice_control_car/image/simulation.png" width="550"> 

## Result

<img src="https://github.com/gnurt2041/ProjectInClass/blob/main/Voice_control_car/image/result1.jpg" width="500"> <img src="https://github.com/gnurt2041/ProjectInClass/blob/main/Voice_control_car/image/result2.jpg" width="500">

## How to use

We use a mobile ardroid app '**BT Voice Control for Arduino**' to connect with **module bluetooth HC-05**. This module bluetooth will send the data to **Arduino Uno** by via the **UART** protocol. After the module connect successfully, we can start use voice to control our car. Voice commands include: **Turn Right**, **Turn Left**, **Move Forward**, **Move Backward**. Moreover, we apply multi-harmonic circuits to create two turn signals. These will make the car closer to the real life car.
