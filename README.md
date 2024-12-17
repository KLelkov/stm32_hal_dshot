# stm32_hal_dshot

Modified library version. Main adjustments - added DShot-1200 support and bidirectional telemetry reading.

I've cut the 4 motors controls and kept the code for just a single motor for testing.

## Brief

__Dshot is drone ESC digital protocol__   

You need STM32 MCU, BLHeli_32 ESC, BLDC  
STM32 MCU makes Dshot signal using PWM and DMA

__KEYWORD__ - `DSHOT` `BLHeli_32 ESC` `BLDC` `STM32 HAL` `TIMER` `PWM` `DMA`

![dshot](https://user-images.githubusercontent.com/48342925/130811044-cabbd6df-1b0b-4482-be93-9c13b84f68b6.gif)


## Dev Environment  
- STM32CubeIDE
- STM32 HAL driver
- STM32F401
- BLHeli_32 ESC
- Drone BLDC


## Library Features
* Change motor throttle
    * Throttle range : 2000 steps (0% - 100%)
* Choose Dshot 150/300/600/1200
* Other command : no
* Telemetry : yes (bidirectional )


## ESC Firmware and Protocol

### ESC Firmware
||1st|2nd|3rd|
|:---:|:---:|:---:|:---:|
|ESC Firmware|BLHeli|BLHeli_S|`BLHeli_32`|

### ESC Protocol
||Analogue signal|`Digital signal`|
|:---:|:---:|:---:|
|ESC Protocol|PWM <br> Oneshot125 <br> Oneshot42 <br> Multishot|`Dshot150` <br>` Dshot300` <br> `Dshot600` <br> `Dshot1200` <br> Proshot1000|

## Dshot

![image](https://user-images.githubusercontent.com/48342925/105716366-0f1e2680-5f62-11eb-8d5c-651e15907a53.png)  
### 1. Data Frame  
* 11bits : 0 - 47 command, 48 - 2047 throttle
* 1bit : telemetry request
* 4bits : checksum

### 2. Bit 0 / 1
![image](https://user-images.githubusercontent.com/48342925/105717632-b780ba80-5f63-11eb-9508-b54be4bd544d.png)

||bit 0|bit 1|
|:---:|:---:|:---:|
|duty cycle|37.425%|74.850%|

### 3. Transmission Time
||bits / sec|sec / bit|sec / frame|  
|:---:|:---:|:---:|:---:|  
|Dshot150|150,000 bits/s|6.67us|106.7us|
|Dshot300|300,000  bits/s|3.33us|53.3us|
|Dshot600|600,000 bits/s|1.67us|26.7us|
|Dshot1200|1,200,000 bits/s|0.83us|13.28us|


### 4. Signal Example
<img src = https://user-images.githubusercontent.com/48342925/122077706-ec1fe080-ce36-11eb-9f24-a70c651b41c5.jpg width = "50%" height = "50%">

- throttle value : 11 / telemetry request : 0 (no) / checksum 4 bits
- 0 0 0 0 0 0 0 1 0 1 1 / 0 / 0 1 1 1 

### 5. Telemetry example

![dshot](https://github.com/KLelkov/stm32_hal_dshot/blob/main/dshot.gif?raw=true)

### 6. Arming sequence

- After power on, send zero throttle for a while until 1 high beep is ended.  
- If more than 50% throttle is detected at arm start, the ESC starts throttle calibration (I haven't check this)      


## STM32CubeMX

- Project Manager
![image](https://user-images.githubusercontent.com/48342925/124620697-a34acd00-deb4-11eb-8b47-8fe5a3dad001.png)

- TIM2 is used to control the motor, TIM5 is used to read the telemetry.

  

  For telemetry you will need uint32 pin (supports counting up to **4294967295**). On my board this is TIM5.

  ![image](https://user-images.githubusercontent.com/48342925/124617628-0e46d480-deb2-11eb-8aad-5a72027d4d35.png)
  ![image](https://user-images.githubusercontent.com/48342925/124617830-37fffb80-deb2-11eb-93e4-341fbcec2ac5.png)

- DMA is definitely needed for control. And I'm not sure that it is needed for telemetry, but I've set it up just in case.
![image](https://user-images.githubusercontent.com/48342925/124618725-fde32980-deb2-11eb-842a-e863431dd1b8.png)




## Example

### dshot.h
- TIM5, TIM2 are 84 MHz
- Motor IN - PA15, GND
- Telemetry in - PA0
- Since the control and telemetry are done over a single wire - we need to connect PA15 and PA0 together
- Uart6 - PC7 (I used this for debug only, so a single wire is enough), GND
```c
/* User Configuration */
// Timer Clock
#define TIMER_CLOCK				84000000	// 84MHz

// MOTOR 1 (PA3) - TIM5 Channel 4, DMA1 Stream 3
#define MOTOR_1_TIM             (&htim5)
#define MOTOR_1_TIM_CHANNEL     TIM_CHANNEL_4

// MOTOR 2 (PA2) - TIM2 Channel 3, DMA1 Stream 1
#define MOTOR_2_TIM             (&htim2)
#define MOTOR_2_TIM_CHANNEL     TIM_CHANNEL_3

// MOTOR 3 (PA0) - TIM2 Channel 1, DMA1 Stream 5
#define MOTOR_3_TIM             (&htim2)
#define MOTOR_3_TIM_CHANNEL     TIM_CHANNEL_1

// MOTOR 4 (PA1) - TIM5 Channel 2, DMA1 Stream 4
#define MOTOR_4_TIM             (&htim5)
#define MOTOR_4_TIM_CHANNEL     TIM_CHANNEL_2
```

### main.c
- only contain dshot things

```c
#include "dshot.h"

// motor value
uint16_t my_motor_value = 0;

int main (void)
{
    // initialize
    dshot_init(DSHOT1200);

    while (1)
    {
        // transmit new dshot signals
        dshot_write(my_motor_value);
        HAL_Delay(1);
    }
}
```

### tim.c

Since the dshot telemetry comes at very fast speeds the STM controller may be unable to process all the pulses. For example my f401 board has 84 MHz processor and this was enough only to process dshot-150 and dshot-300 telemetry data. The higher rates dshot protocols are too fast for this board (my code takes too many operations between pulses).

Keep in mind, that to receive the telemetry data - you need to send reversed dshot signal. This way the ESC will know to send you back telemetry data over the same wire.

To configure this, you need to change one variable in code and one cubeMX setting.
