---
typora-root-url: image
---

# Gesture Recognition For HID

## Hardward

- NUCLEO-F767ZI
- X-NUCLEO-6180A1
- VL6180-SATEL

## Principle

VL6180 can accurately measure the time it takes for light to travel to the nearest object and reflect back to the sensor. In this solution, 4 sensors are arranged in four directions: up, down, left, and right. When the hand is waving or clicking on the sensor, the distance between the hand and the sensor will change. STM32 reads the required data through i2c, and judges the user's gestures based on the changes in the data. For example, the sensors on the left and right sides will measure that the distance on one side becomes larger, and the distance on the other side becomes smaller, and it can be judged as waving left and right. STM32 needs to convert the determined gesture into cursor movement, click and other operations. STM32 transmits these operations to the PC in real time via the USB HID protocol.