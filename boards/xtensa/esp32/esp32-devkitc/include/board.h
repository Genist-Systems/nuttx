/****************************************************************************
 * boards/xtensa/esp32/esp32-devkitc/include/board.h
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 ****************************************************************************/



 /*
    Physical Layout of the DFRobot ESP32 WROOM-32UE
    
 *  RST
 *  3V3
 *
 *  GND                             
 *  EN                              VCC
 *  SCK   - 18                      3V3
 *  MOSI  - 23                      GND
 *  MISO  - 19                      SCL  - 22
 *  SCL   - 22                      SDA  - 21
 *  SDA   - 21                      GPIO - 15
 *  GPIO  -  2                      GPIO - 35
 *  GPIO  - 13                      GPIO - 34
 *  GPIO  - 14                      GPIO - 39
 *  GPIO  -  0                      GPIO - 36
 *  GPIO  - 26                      GPIO - 12
 *  GPIO  - 25                      GPIO -  4
 *  TX    -  1
 *  RX    -  3                      GPIO - 17

 
 
 */
#ifndef __BOARDS_XTENSA_ESP32_ESP32_DEVKITC_INCLUDE_BOARD_H
#define __BOARDS_XTENSA_ESP32_ESP32_DEVKITC_INCLUDE_BOARD_H

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* Clocking *****************************************************************/

/* The ESP32 Devkit board is fitted with either a 26 a 40MHz crystal */

#ifdef CONFIG_ESP32_XTAL_26MHz
#  define BOARD_XTAL_FREQUENCY  26000000
#else
#  define BOARD_XTAL_FREQUENCY  40000000
#endif

#ifdef CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ
#  define BOARD_CLOCK_FREQUENCY (CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ * 1000000)
#else
#  define BOARD_CLOCK_FREQUENCY 80000000
#endif

/* LED definitions **********************************************************/

/* Define how many LEDs this board has (needed by userleds) */

#define BOARD_NLEDS       1

/* GPIO pins used by the GPIO Subsystem */

#define BOARD_NGPIOOUT    1 /* Amount of GPIO Output pins */
#define BOARD_NGPIOIN     1 /* Amount of GPIO Input without Interruption */
#define BOARD_NGPIOINT    1 /* Amount of GPIO Input w/ Interruption pins */

#endif /* __BOARDS_XTENSA_ESP32_ESP32_DEVKITC_INCLUDE_BOARD_H */
