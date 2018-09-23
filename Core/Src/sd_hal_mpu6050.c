/*
 * sd_hal_mpu6050.c
 *
 *  Created on: Feb 19, 2016
 *      Author: Sina Darvishi
 */

/**
 * |----------------------------------------------------------------------
 * | Copyright (C) Sina Darvishi,2016
 * |
 * | This program is free software: you can redistribute it and/or modify
 * | it under the terms of the GNU General Public License as published by
 * | the Free Software Foundation, either version 3 of the License, or
 * | any later version.
 * |
 * | This program is distributed in the hope that it will be useful,
 * | but WITHOUT ANY WARRANTY; without even the implied warranty of
 * | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * | GNU General Public License for more details.
 * |
 * | You should have received a copy of the GNU General Public License
 * | along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * |----------------------------------------------------------------------
 */

#include "sd_hal_mpu6050.h"

/* Default I2C address */
#define MPU6050_I2C_ADDR			0xD0

/* Who I am register value */
#define MPU6050_I_AM				0x68

/* MPU6050 registers */
#define MPU6050_AUX_VDDIO			0x01
#define MPU6050_SMPLRT_DIV			0x19
#define MPU6050_CONFIG				0x1A
#define MPU6050_GYRO_CONFIG			0x1B
#define MPU6050_ACCEL_CONFIG		0x1C
#define MPU6050_MOTION_THRESH		0x1F
#define MPU6050_INT_PIN_CFG			0x37
#define MPU6050_INT_ENABLE			0x38
#define MPU6050_INT_STATUS			0x3A
#define MPU6050_ACCEL_XOUT_H		0x3B
#define MPU6050_ACCEL_XOUT_L		0x3C
#define MPU6050_ACCEL_YOUT_H		0x3D
#define MPU6050_ACCEL_YOUT_L		0x3E
#define MPU6050_ACCEL_ZOUT_H		0x3F
#define MPU6050_ACCEL_ZOUT_L		0x40
#define MPU6050_TEMP_OUT_H			0x41
#define MPU6050_TEMP_OUT_L			0x42
#define MPU6050_GYRO_XOUT_H			0x43
#define MPU6050_GYRO_XOUT_L			0x44
#define MPU6050_GYRO_YOUT_H			0x45
#define MPU6050_GYRO_YOUT_L			0x46
#define MPU6050_GYRO_ZOUT_H			0x47
#define MPU6050_GYRO_ZOUT_L			0x48
#define MPU6050_MOT_DETECT_STATUS	0x61
#define MPU6050_SIGNAL_PATH_RESET	0x68
#define MPU6050_MOT_DETECT_CTRL		0x69
#define MPU6050_USER_CTRL			0x6A
#define MPU6050_PWR_MGMT_1			0x6B
#define MPU6050_PWR_MGMT_2			0x6C
#define MPU6050_FIFO_COUNTH			0x72
#define MPU6050_FIFO_COUNTL			0x73
#define MPU6050_FIFO_R_W			0x74
#define MPU6050_WHO_AM_I			0x75

/* Gyro sensitivities in degrees/s */
#define MPU6050_GYRO_SENS_250		((float) 131)
#define MPU6050_GYRO_SENS_500		((float) 65.5)
#define MPU6050_GYRO_SENS_1000		((float) 32.8)
#define MPU6050_GYRO_SENS_2000		((float) 16.4)

/* Acce sensitivities in g/s */
#define MPU6050_ACCE_SENS_2			((float) 16384)
#define MPU6050_ACCE_SENS_4			((float) 8192)
#define MPU6050_ACCE_SENS_8			((float) 4096)
#define MPU6050_ACCE_SENS_16		((float) 2048)

/* ================================================================================================ *
 | Default MotionApps v4.1 48-byte FIFO packet structure:                                           |
 |                                                                                                  |
 | [QUAT W][      ][QUAT X][      ][QUAT Y][      ][QUAT Z][      ][GYRO X][      ][GYRO Y][      ] |
 |   0   1   2   3   4   5   6   7   8   9  10  11  12  13  14  15  16  17  18  19  20  21  22  23  |
 |                                                                                                  |
 | [GYRO Z][      ][MAG X ][MAG Y ][MAG Z ][ACC X ][      ][ACC Y ][      ][ACC Z ][      ][      ] |
 |  24  25  26  27  28  29  30  31  32  33  34  35  36  37  38  39  40  41  42  43  44  45  46  47  |
 * ================================================================================================ */

// this block of memory gets written to the MPU on start-up, and it seems
// to be volatile memory, so it has to be done each time (it only takes ~1
// second though)
const uint8_t dmpMemory[MPU6050_DMP_CODE_SIZE]  = {
    // bank 0, 256 bytes
    0xFB, 0x00, 0x00, 0x3E, 0x00, 0x0B, 0x00, 0x36, 0x00, 0x01, 0x00, 0x02, 0x00, 0x03, 0x00, 0x00,
    0x00, 0x65, 0x00, 0x54, 0xFF, 0xEF, 0x00, 0x00, 0xFA, 0x80, 0x00, 0x0B, 0x12, 0x82, 0x00, 0x01,
    0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x28, 0x00, 0x00, 0xFF, 0xFF, 0x45, 0x81, 0xFF, 0xFF, 0xFA, 0x72, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x03, 0xE8, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x7F, 0xFF, 0xFF, 0xFE, 0x80, 0x01,
    0x00, 0x1B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x3E, 0x03, 0x30, 0x40, 0x00, 0x00, 0x00, 0x02, 0xCA, 0xE3, 0x09, 0x3E, 0x80, 0x00, 0x00,
    0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x60, 0x00, 0x00, 0x00,
    0x41, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x0B, 0x2A, 0x00, 0x00, 0x16, 0x55, 0x00, 0x00, 0x21, 0x82,
    0xFD, 0x87, 0x26, 0x50, 0xFD, 0x80, 0x00, 0x00, 0x00, 0x1F, 0x00, 0x00, 0x00, 0x05, 0x80, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00,
    0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x6F, 0x00, 0x02, 0x65, 0x32, 0x00, 0x00, 0x5E, 0xC0,
    0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xFB, 0x8C, 0x6F, 0x5D, 0xFD, 0x5D, 0x08, 0xD9, 0x00, 0x7C, 0x73, 0x3B, 0x00, 0x6C, 0x12, 0xCC,
    0x32, 0x00, 0x13, 0x9D, 0x32, 0x00, 0xD0, 0xD6, 0x32, 0x00, 0x08, 0x00, 0x40, 0x00, 0x01, 0xF4,
    0xFF, 0xE6, 0x80, 0x79, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0xD0, 0xD6, 0x00, 0x00, 0x27, 0x10,

    // bank 1, 256 bytes
    0xFB, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00,
    0x00, 0x00, 0xFA, 0x36, 0xFF, 0xBC, 0x30, 0x8E, 0x00, 0x05, 0xFB, 0xF0, 0xFF, 0xD9, 0x5B, 0xC8,
    0xFF, 0xD0, 0x9A, 0xBE, 0x00, 0x00, 0x10, 0xA9, 0xFF, 0xF4, 0x1E, 0xB2, 0x00, 0xCE, 0xBB, 0xF7,
    0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x04, 0x00, 0x02, 0x00, 0x02, 0x02, 0x00, 0x00, 0x0C,
    0xFF, 0xC2, 0x80, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0xCF, 0x80, 0x00, 0x40, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x14,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x03, 0x3F, 0x68, 0xB6, 0x79, 0x35, 0x28, 0xBC, 0xC6, 0x7E, 0xD1, 0x6C,
    0x80, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0xB2, 0x6A, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3F, 0xF0, 0x00, 0x00, 0x00, 0x30,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x25, 0x4D, 0x00, 0x2F, 0x70, 0x6D, 0x00, 0x00, 0x05, 0xAE, 0x00, 0x0C, 0x02, 0xD0,

    // bank 2, 256 bytes
    0x00, 0x00, 0x00, 0x00, 0x00, 0x65, 0x00, 0x54, 0xFF, 0xEF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x01, 0x00, 0x00, 0x44, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x00, 0x00, 0x01, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x65, 0x00, 0x00, 0x00, 0x54, 0x00, 0x00, 0xFF, 0xEF, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x1B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00,
    0x00, 0x1B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x47, 0x78, 0xA2,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    // bank 3, 256 bytes
    0xD8, 0xDC, 0xF4, 0xD8, 0xB9, 0xAB, 0xF3, 0xF8, 0xFA, 0xF1, 0xBA, 0xA2, 0xDE, 0xB2, 0xB8, 0xB4,
    0xA8, 0x81, 0x98, 0xF7, 0x4A, 0x90, 0x7F, 0x91, 0x6A, 0xF3, 0xF9, 0xDB, 0xA8, 0xF9, 0xB0, 0xBA,
    0xA0, 0x80, 0xF2, 0xCE, 0x81, 0xF3, 0xC2, 0xF1, 0xC1, 0xF2, 0xC3, 0xF3, 0xCC, 0xA2, 0xB2, 0x80,
    0xF1, 0xC6, 0xD8, 0x80, 0xBA, 0xA7, 0xDF, 0xDF, 0xDF, 0xF2, 0xA7, 0xC3, 0xCB, 0xC5, 0xB6, 0xF0,
    0x87, 0xA2, 0x94, 0x24, 0x48, 0x70, 0x3C, 0x95, 0x40, 0x68, 0x34, 0x58, 0x9B, 0x78, 0xA2, 0xF1,
    0x83, 0x92, 0x2D, 0x55, 0x7D, 0xD8, 0xB1, 0xB4, 0xB8, 0xA1, 0xD0, 0x91, 0x80, 0xF2, 0x70, 0xF3,
    0x70, 0xF2, 0x7C, 0x80, 0xA8, 0xF1, 0x01, 0xB0, 0x98, 0x87, 0xD9, 0x43, 0xD8, 0x86, 0xC9, 0x88,
    0xBA, 0xA1, 0xF2, 0x0E, 0xB8, 0x97, 0x80, 0xF1, 0xA9, 0xDF, 0xDF, 0xDF, 0xAA, 0xDF, 0xDF, 0xDF,
    0xF2, 0xAA, 0xC5, 0xCD, 0xC7, 0xA9, 0x0C, 0xC9, 0x2C, 0x97, 0x97, 0x97, 0x97, 0xF1, 0xA9, 0x89,
    0x26, 0x46, 0x66, 0xB0, 0xB4, 0xBA, 0x80, 0xAC, 0xDE, 0xF2, 0xCA, 0xF1, 0xB2, 0x8C, 0x02, 0xA9,
    0xB6, 0x98, 0x00, 0x89, 0x0E, 0x16, 0x1E, 0xB8, 0xA9, 0xB4, 0x99, 0x2C, 0x54, 0x7C, 0xB0, 0x8A,
    0xA8, 0x96, 0x36, 0x56, 0x76, 0xF1, 0xB9, 0xAF, 0xB4, 0xB0, 0x83, 0xC0, 0xB8, 0xA8, 0x97, 0x11,
    0xB1, 0x8F, 0x98, 0xB9, 0xAF, 0xF0, 0x24, 0x08, 0x44, 0x10, 0x64, 0x18, 0xF1, 0xA3, 0x29, 0x55,
    0x7D, 0xAF, 0x83, 0xB5, 0x93, 0xF0, 0x00, 0x28, 0x50, 0xF5, 0xBA, 0xAD, 0x8F, 0x9F, 0x28, 0x54,
    0x7C, 0xB9, 0xF1, 0xA3, 0x86, 0x9F, 0x61, 0xA6, 0xDA, 0xDE, 0xDF, 0xDB, 0xB2, 0xB6, 0x8E, 0x9D,
    0xAE, 0xF5, 0x60, 0x68, 0x70, 0xB1, 0xB5, 0xF1, 0xDA, 0xA6, 0xDF, 0xD9, 0xA6, 0xFA, 0xA3, 0x86,

    // bank 4, 256 bytes
    0x96, 0xDB, 0x31, 0xA6, 0xD9, 0xF8, 0xDF, 0xBA, 0xA6, 0x8F, 0xC2, 0xC5, 0xC7, 0xB2, 0x8C, 0xC1,
    0xB8, 0xA2, 0xDF, 0xDF, 0xDF, 0xA3, 0xDF, 0xDF, 0xDF, 0xD8, 0xD8, 0xF1, 0xB8, 0xA8, 0xB2, 0x86,
    0xB4, 0x98, 0x0D, 0x35, 0x5D, 0xB8, 0xAA, 0x98, 0xB0, 0x87, 0x2D, 0x35, 0x3D, 0xB2, 0xB6, 0xBA,
    0xAF, 0x8C, 0x96, 0x19, 0x8F, 0x9F, 0xA7, 0x0E, 0x16, 0x1E, 0xB4, 0x9A, 0xB8, 0xAA, 0x87, 0x2C,
    0x54, 0x7C, 0xB9, 0xA3, 0xDE, 0xDF, 0xDF, 0xA3, 0xB1, 0x80, 0xF2, 0xC4, 0xCD, 0xC9, 0xF1, 0xB8,
    0xA9, 0xB4, 0x99, 0x83, 0x0D, 0x35, 0x5D, 0x89, 0xB9, 0xA3, 0x2D, 0x55, 0x7D, 0xB5, 0x93, 0xA3,
    0x0E, 0x16, 0x1E, 0xA9, 0x2C, 0x54, 0x7C, 0xB8, 0xB4, 0xB0, 0xF1, 0x97, 0x83, 0xA8, 0x11, 0x84,
    0xA5, 0x09, 0x98, 0xA3, 0x83, 0xF0, 0xDA, 0x24, 0x08, 0x44, 0x10, 0x64, 0x18, 0xD8, 0xF1, 0xA5,
    0x29, 0x55, 0x7D, 0xA5, 0x85, 0x95, 0x02, 0x1A, 0x2E, 0x3A, 0x56, 0x5A, 0x40, 0x48, 0xF9, 0xF3,
    0xA3, 0xD9, 0xF8, 0xF0, 0x98, 0x83, 0x24, 0x08, 0x44, 0x10, 0x64, 0x18, 0x97, 0x82, 0xA8, 0xF1,
    0x11, 0xF0, 0x98, 0xA2, 0x24, 0x08, 0x44, 0x10, 0x64, 0x18, 0xDA, 0xF3, 0xDE, 0xD8, 0x83, 0xA5,
    0x94, 0x01, 0xD9, 0xA3, 0x02, 0xF1, 0xA2, 0xC3, 0xC5, 0xC7, 0xD8, 0xF1, 0x84, 0x92, 0xA2, 0x4D,
    0xDA, 0x2A, 0xD8, 0x48, 0x69, 0xD9, 0x2A, 0xD8, 0x68, 0x55, 0xDA, 0x32, 0xD8, 0x50, 0x71, 0xD9,
    0x32, 0xD8, 0x70, 0x5D, 0xDA, 0x3A, 0xD8, 0x58, 0x79, 0xD9, 0x3A, 0xD8, 0x78, 0x93, 0xA3, 0x4D,
    0xDA, 0x2A, 0xD8, 0x48, 0x69, 0xD9, 0x2A, 0xD8, 0x68, 0x55, 0xDA, 0x32, 0xD8, 0x50, 0x71, 0xD9,
    0x32, 0xD8, 0x70, 0x5D, 0xDA, 0x3A, 0xD8, 0x58, 0x79, 0xD9, 0x3A, 0xD8, 0x78, 0xA8, 0x8A, 0x9A,

    // bank 5, 256 bytes
    0xF0, 0x28, 0x50, 0x78, 0x9E, 0xF3, 0x88, 0x18, 0xF1, 0x9F, 0x1D, 0x98, 0xA8, 0xD9, 0x08, 0xD8,
    0xC8, 0x9F, 0x12, 0x9E, 0xF3, 0x15, 0xA8, 0xDA, 0x12, 0x10, 0xD8, 0xF1, 0xAF, 0xC8, 0x97, 0x87,
    0x34, 0xB5, 0xB9, 0x94, 0xA4, 0x21, 0xF3, 0xD9, 0x22, 0xD8, 0xF2, 0x2D, 0xF3, 0xD9, 0x2A, 0xD8,
    0xF2, 0x35, 0xF3, 0xD9, 0x32, 0xD8, 0x81, 0xA4, 0x60, 0x60, 0x61, 0xD9, 0x61, 0xD8, 0x6C, 0x68,
    0x69, 0xD9, 0x69, 0xD8, 0x74, 0x70, 0x71, 0xD9, 0x71, 0xD8, 0xB1, 0xA3, 0x84, 0x19, 0x3D, 0x5D,
    0xA3, 0x83, 0x1A, 0x3E, 0x5E, 0x93, 0x10, 0x30, 0x81, 0x10, 0x11, 0xB8, 0xB0, 0xAF, 0x8F, 0x94,
    0xF2, 0xDA, 0x3E, 0xD8, 0xB4, 0x9A, 0xA8, 0x87, 0x29, 0xDA, 0xF8, 0xD8, 0x87, 0x9A, 0x35, 0xDA,
    0xF8, 0xD8, 0x87, 0x9A, 0x3D, 0xDA, 0xF8, 0xD8, 0xB1, 0xB9, 0xA4, 0x98, 0x85, 0x02, 0x2E, 0x56,
    0xA5, 0x81, 0x00, 0x0C, 0x14, 0xA3, 0x97, 0xB0, 0x8A, 0xF1, 0x2D, 0xD9, 0x28, 0xD8, 0x4D, 0xD9,
    0x48, 0xD8, 0x6D, 0xD9, 0x68, 0xD8, 0xB1, 0x84, 0x0D, 0xDA, 0x0E, 0xD8, 0xA3, 0x29, 0x83, 0xDA,
    0x2C, 0x0E, 0xD8, 0xA3, 0x84, 0x49, 0x83, 0xDA, 0x2C, 0x4C, 0x0E, 0xD8, 0xB8, 0xB0, 0x97, 0x86,
    0xA8, 0x31, 0x9B, 0x06, 0x99, 0x07, 0xAB, 0x97, 0x28, 0x88, 0x9B, 0xF0, 0x0C, 0x20, 0x14, 0x40,
    0xB9, 0xA3, 0x8A, 0xC3, 0xC5, 0xC7, 0x9A, 0xA3, 0x28, 0x50, 0x78, 0xF1, 0xB5, 0x93, 0x01, 0xD9,
    0xDF, 0xDF, 0xDF, 0xD8, 0xB8, 0xB4, 0xA8, 0x8C, 0x9C, 0xF0, 0x04, 0x28, 0x51, 0x79, 0x1D, 0x30,
    0x14, 0x38, 0xB2, 0x82, 0xAB, 0xD0, 0x98, 0x2C, 0x50, 0x50, 0x78, 0x78, 0x9B, 0xF1, 0x1A, 0xB0,
    0xF0, 0xB1, 0x83, 0x9C, 0xA8, 0x29, 0x51, 0x79, 0xB0, 0x8B, 0x29, 0x51, 0x79, 0xB1, 0x83, 0x24,

    // bank 6, 256 bytes
    0x70, 0x59, 0xB0, 0x8B, 0x20, 0x58, 0x71, 0xB1, 0x83, 0x44, 0x69, 0x38, 0xB0, 0x8B, 0x39, 0x40,
    0x68, 0xB1, 0x83, 0x64, 0x48, 0x31, 0xB0, 0x8B, 0x30, 0x49, 0x60, 0xA5, 0x88, 0x20, 0x09, 0x71,
    0x58, 0x44, 0x68, 0x11, 0x39, 0x64, 0x49, 0x30, 0x19, 0xF1, 0xAC, 0x00, 0x2C, 0x54, 0x7C, 0xF0,
    0x8C, 0xA8, 0x04, 0x28, 0x50, 0x78, 0xF1, 0x88, 0x97, 0x26, 0xA8, 0x59, 0x98, 0xAC, 0x8C, 0x02,
    0x26, 0x46, 0x66, 0xF0, 0x89, 0x9C, 0xA8, 0x29, 0x51, 0x79, 0x24, 0x70, 0x59, 0x44, 0x69, 0x38,
    0x64, 0x48, 0x31, 0xA9, 0x88, 0x09, 0x20, 0x59, 0x70, 0xAB, 0x11, 0x38, 0x40, 0x69, 0xA8, 0x19,
    0x31, 0x48, 0x60, 0x8C, 0xA8, 0x3C, 0x41, 0x5C, 0x20, 0x7C, 0x00, 0xF1, 0x87, 0x98, 0x19, 0x86,
    0xA8, 0x6E, 0x76, 0x7E, 0xA9, 0x99, 0x88, 0x2D, 0x55, 0x7D, 0x9E, 0xB9, 0xA3, 0x8A, 0x22, 0x8A,
    0x6E, 0x8A, 0x56, 0x8A, 0x5E, 0x9F, 0xB1, 0x83, 0x06, 0x26, 0x46, 0x66, 0x0E, 0x2E, 0x4E, 0x6E,
    0x9D, 0xB8, 0xAD, 0x00, 0x2C, 0x54, 0x7C, 0xF2, 0xB1, 0x8C, 0xB4, 0x99, 0xB9, 0xA3, 0x2D, 0x55,
    0x7D, 0x81, 0x91, 0xAC, 0x38, 0xAD, 0x3A, 0xB5, 0x83, 0x91, 0xAC, 0x2D, 0xD9, 0x28, 0xD8, 0x4D,
    0xD9, 0x48, 0xD8, 0x6D, 0xD9, 0x68, 0xD8, 0x8C, 0x9D, 0xAE, 0x29, 0xD9, 0x04, 0xAE, 0xD8, 0x51,
    0xD9, 0x04, 0xAE, 0xD8, 0x79, 0xD9, 0x04, 0xD8, 0x81, 0xF3, 0x9D, 0xAD, 0x00, 0x8D, 0xAE, 0x19,
    0x81, 0xAD, 0xD9, 0x01, 0xD8, 0xF2, 0xAE, 0xDA, 0x26, 0xD8, 0x8E, 0x91, 0x29, 0x83, 0xA7, 0xD9,
    0xAD, 0xAD, 0xAD, 0xAD, 0xF3, 0x2A, 0xD8, 0xD8, 0xF1, 0xB0, 0xAC, 0x89, 0x91, 0x3E, 0x5E, 0x76,
    0xF3, 0xAC, 0x2E, 0x2E, 0xF1, 0xB1, 0x8C, 0x5A, 0x9C, 0xAC, 0x2C, 0x28, 0x28, 0x28, 0x9C, 0xAC,

    // bank 7, 170 bytes (remainder)
    0x30, 0x18, 0xA8, 0x98, 0x81, 0x28, 0x34, 0x3C, 0x97, 0x24, 0xA7, 0x28, 0x34, 0x3C, 0x9C, 0x24,
    0xF2, 0xB0, 0x89, 0xAC, 0x91, 0x2C, 0x4C, 0x6C, 0x8A, 0x9B, 0x2D, 0xD9, 0xD8, 0xD8, 0x51, 0xD9,
    0xD8, 0xD8, 0x79, 0xD9, 0xD8, 0xD8, 0xF1, 0x9E, 0x88, 0xA3, 0x31, 0xDA, 0xD8, 0xD8, 0x91, 0x2D,
    0xD9, 0x28, 0xD8, 0x4D, 0xD9, 0x48, 0xD8, 0x6D, 0xD9, 0x68, 0xD8, 0xB1, 0x83, 0x93, 0x35, 0x3D,
    0x80, 0x25, 0xDA, 0xD8, 0xD8, 0x85, 0x69, 0xDA, 0xD8, 0xD8, 0xB4, 0x93, 0x81, 0xA3, 0x28, 0x34,
    0x3C, 0xF3, 0xAB, 0x8B, 0xA3, 0x91, 0xB6, 0x09, 0xB4, 0xD9, 0xAB, 0xDE, 0xB0, 0x87, 0x9C, 0xB9,
    0xA3, 0xDD, 0xF1, 0xA3, 0xA3, 0xA3, 0xA3, 0x95, 0xF1, 0xA3, 0xA3, 0xA3, 0x9D, 0xF1, 0xA3, 0xA3,
    0xA3, 0xA3, 0xF2, 0xA3, 0xB4, 0x90, 0x80, 0xF2, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3,
    0xA3, 0xA3, 0xB2, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3, 0xB0, 0x87, 0xB5, 0x99, 0xF1, 0xA3, 0xA3,
    0xA3, 0x98, 0xF1, 0xA3, 0xA3, 0xA3, 0xA3, 0x97, 0xA3, 0xA3, 0xA3, 0xA3, 0xF3, 0x9B, 0xA3, 0xA3,
    0xDC, 0xB9, 0xA7, 0xF1, 0x26, 0x26, 0x26, 0xD8, 0xD8, 0xFF
};

const uint8_t dmpConfig[MPU6050_DMP_CONFIG_SIZE]  = {
//  BANK    OFFSET  LENGTH  [DATA]
    0x02,   0xEC,   0x04,   0x00, 0x47, 0x7D, 0x1A,   // ?
    0x03,   0x82,   0x03,   0x4C, 0xCD, 0x6C,         // FCFG_1 inv_set_gyro_calibration
    0x03,   0xB2,   0x03,   0x36, 0x56, 0x76,         // FCFG_3 inv_set_gyro_calibration
    0x00,   0x68,   0x04,   0x02, 0xCA, 0xE3, 0x09,   // D_0_104 inv_set_gyro_calibration
    0x01,   0x0C,   0x04,   0x00, 0x00, 0x00, 0x00,   // D_1_152 inv_set_accel_calibration
    0x03,   0x86,   0x03,   0x0C, 0xC9, 0x2C,         // FCFG_2 inv_set_accel_calibration
    0x03,   0x90,   0x03,   0x26, 0x46, 0x66,         //   (continued)...FCFG_2 inv_set_accel_calibration
    0x00,   0x6C,   0x02,   0x40, 0x00,               // D_0_108 inv_set_accel_calibration

    0x02,   0x40,   0x04,   0x00, 0x00, 0x00, 0x00,   // CPASS_MTX_00 inv_set_compass_calibration
    0x02,   0x44,   0x04,   0x40, 0x00, 0x00, 0x00,   // CPASS_MTX_01
    0x02,   0x48,   0x04,   0x00, 0x00, 0x00, 0x00,   // CPASS_MTX_02
    0x02,   0x4C,   0x04,   0x40, 0x00, 0x00, 0x00,   // CPASS_MTX_10
    0x02,   0x50,   0x04,   0x00, 0x00, 0x00, 0x00,   // CPASS_MTX_11
    0x02,   0x54,   0x04,   0x00, 0x00, 0x00, 0x00,   // CPASS_MTX_12
    0x02,   0x58,   0x04,   0x00, 0x00, 0x00, 0x00,   // CPASS_MTX_20
    0x02,   0x5C,   0x04,   0x00, 0x00, 0x00, 0x00,   // CPASS_MTX_21
    0x02,   0xBC,   0x04,   0xC0, 0x00, 0x00, 0x00,   // CPASS_MTX_22

    0x01,   0xEC,   0x04,   0x00, 0x00, 0x40, 0x00,   // D_1_236 inv_apply_endian_accel
    0x03,   0x86,   0x06,   0x0C, 0xC9, 0x2C, 0x97, 0x97, 0x97, // FCFG_2 inv_set_mpu_sensors
    0x04,   0x22,   0x03,   0x0D, 0x35, 0x5D,         // CFG_MOTION_BIAS inv_turn_on_bias_from_no_motion
    0x00,   0xA3,   0x01,   0x00,                     // ?
    0x04,   0x29,   0x04,   0x87, 0x2D, 0x35, 0x3D,   // FCFG_5 inv_set_bias_update
    0x07,   0x62,   0x05,   0xF1, 0x20, 0x28, 0x30, 0x38, // CFG_8 inv_send_quaternion
    0x07,   0x9F,   0x01,   0x30,                     // CFG_16 inv_set_footer
    0x07,   0x67,   0x01,   0x9A,                     // CFG_GYRO_SOURCE inv_send_gyro
    0x07,   0x68,   0x04,   0xF1, 0x28, 0x30, 0x38,   // CFG_9 inv_send_gyro -> inv_construct3_fifo
    0x07,   0x62,   0x05,   0xF1, 0x20, 0x28, 0x30, 0x38, // ?
    0x02,   0x0C,   0x04,   0x00, 0x00, 0x00, 0x00,   // ?
    0x07,   0x83,   0x06,   0xC2, 0xCA, 0xC4, 0xA3, 0xA3, 0xA3, // ?
                 // SPECIAL 0x01 = enable interrupts
    0x00,   0x00,   0x00,   0x01, // SET INT_ENABLE, SPECIAL INSTRUCTION
    0x07,   0xA7,   0x01,   0xFE,                     // ?
    0x07,   0x62,   0x05,   0xF1, 0x20, 0x28, 0x30, 0x38, // ?
    0x07,   0x67,   0x01,   0x9A,                     // ?
    0x07,   0x68,   0x04,   0xF1, 0x28, 0x30, 0x38,   // CFG_12 inv_send_accel -> inv_construct3_fifo
    0x07,   0x8D,   0x04,   0xF1, 0x28, 0x30, 0x38,   // ??? CFG_12 inv_send_mag -> inv_construct3_fifo
    0x02,   0x16,   0x02,   0x00, 0x03                // D_0_22 inv_set_fifo_rate

    // This very last 0x01 WAS a 0x09, which drops the FIFO rate down to 20 Hz. 0x07 is 25 Hz,
    // 0x01 is 100Hz. Going faster than 100Hz (0x00=200Hz) tends to result in very noisy data.
    // DMP output frequency is calculated easily using this equation: (200Hz / (1 + value))

    // It is important to make sure the host processor can keep up with reading and processing
    // the FIFO output at the desired rate. Handling FIFO overflow cleanly is also a good idea.
};

const uint8_t dmpUpdates[MPU6050_DMP_UPDATES_SIZE] = {
    0x01,   0xB2,   0x02,   0xFF, 0xF5,
    0x01,   0x90,   0x04,   0x0A, 0x0D, 0x97, 0xC0,
    0x00,   0xA3,   0x01,   0x00,
    0x04,   0x29,   0x04,   0x87, 0x2D, 0x35, 0x3D,
    0x01,   0x6A,   0x02,   0x06, 0x00,
    0x01,   0x60,   0x08,   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00,   0x60,   0x04,   0x40, 0x00, 0x00, 0x00,
    0x02,   0x60,   0x0C,   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01,   0x08,   0x02,   0x01, 0x20,
    0x01,   0x0A,   0x02,   0x00, 0x4E,
    0x01,   0x02,   0x02,   0xFE, 0xB3,
    0x02,   0x6C,   0x04,   0x00, 0x00, 0x00, 0x00, // READ
    0x02,   0x6C,   0x04,   0xFA, 0xFE, 0x00, 0x00,
    0x02,   0x60,   0x0C,   0xFF, 0xFF, 0xCB, 0x4D, 0x00, 0x01, 0x08, 0xC1, 0xFF, 0xFF, 0xBC, 0x2C,
    0x02,   0xF4,   0x04,   0x00, 0x00, 0x00, 0x00,
    0x02,   0xF8,   0x04,   0x00, 0x00, 0x00, 0x00,
    0x02,   0xFC,   0x04,   0x00, 0x00, 0x00, 0x00,
    0x00,   0x60,   0x04,   0x40, 0x00, 0x00, 0x00,
    0x00,   0x60,   0x04,   0x00, 0x40, 0x00, 0x00
};

void SD_MPU6050_SetI2C_Device (I2C_HandleTypeDef* I2Cx, SD_MPU6050* DataStruct)
{
	// save i2c device
	DataStruct->_i2c_dev = I2Cx;
}

SD_MPU6050_Result SD_MPU6050_Init(I2C_HandleTypeDef* I2Cx,SD_MPU6050* DataStruct, SD_MPU6050_Device DeviceNumber, SD_MPU6050_Accelerometer AccelerometerSensitivity, SD_MPU6050_Gyroscope GyroscopeSensitivity)
{
	uint8_t WHO_AM_I = (uint8_t)MPU6050_WHO_AM_I;
	uint8_t temp;



	// save i2c device
	DataStruct->_i2c_dev = I2Cx;

	/* Format I2C address */
	DataStruct->Address = MPU6050_I2C_ADDR | (uint8_t)DeviceNumber;
	uint8_t address = DataStruct->Address;

	/* Check if device is connected */
	if(HAL_I2C_IsDeviceReady(DataStruct->_i2c_dev,address,2,5)!=HAL_OK)
	{
				return SD_MPU6050_Result_Error;
	}

	/* Check who am I */
	//------------------
	/* Send address */
	if(HAL_I2C_Master_Transmit(DataStruct->_i2c_dev, address, &WHO_AM_I, 1, 1000) != HAL_OK)
	{
		return SD_MPU6050_Result_Error;
	}

	/* Receive multiple byte */
	if(HAL_I2C_Master_Receive(DataStruct->_i2c_dev, address, &temp, 1, 1000) != HAL_OK)
	{
		return SD_MPU6050_Result_Error;
	}

	/* Checking */
	if (temp != MPU6050_I_AM)
	{
			/* Return error */
			return SD_MPU6050_Result_DeviceInvalid;
	}

	// reset mpu
	SD_MPU6050_Reset (DataStruct);

	//------------------
	// Set clock
	SD_MPU6050_SetClock(DataStruct, MPU6050_CLOCK_PLL_XGYRO);

	/* Set sample rate to 1kHz */
	SD_MPU6050_SetDataRate(DataStruct, SD_MPU6050_DataRate_100Hz);

	/* Config accelerometer */
	SD_MPU6050_SetAccelerometer(DataStruct, AccelerometerSensitivity);

	/* Config Gyroscope */
	SD_MPU6050_SetGyroscope(DataStruct, GyroscopeSensitivity);

	/* Return OK */
	return SD_MPU6050_Result_Ok;
}

SD_MPU6050_Result SD_MPU6050_SetDataRate(SD_MPU6050* DataStruct, uint8_t rate)
{
	uint8_t d[2];

	uint8_t address = DataStruct->Address;
	/* Format array to send */
	d[0] = MPU6050_SMPLRT_DIV;
	d[1] = rate;

	/* Set data sample rate */
	if(HAL_I2C_Master_Transmit(DataStruct->_i2c_dev,(uint16_t)address,(uint8_t *)d,2,1000)!=HAL_OK)
	{
				return SD_MPU6050_Result_Error;
	}

	/* Return OK */
	return SD_MPU6050_Result_Ok;
}

SD_MPU6050_Result SD_MPU6050_SetClock (SD_MPU6050* DataStruct, uint8_t clock)
{
	uint8_t reg[2];

	uint8_t address = DataStruct->Address;
	/* Format array to send */
	reg[0] = MPU6050_RA_PWR_MGMT_1;
	reg[1] = clock;

	 if (HAL_I2C_Master_Transmit(DataStruct->_i2c_dev, (uint16_t)address, &reg[0], 1, 1000) != HAL_OK)
		 return SD_MPU6050_Result_Error;

	 if (HAL_I2C_Master_Receive(DataStruct->_i2c_dev, (uint16_t)address, &reg[1], 1, 1000) != HAL_OK)
		 return SD_MPU6050_Result_Error;
	 reg[1] &= 0b11111000;
	 reg[1] |= clock;
	/* Set data sample rate */
	if(HAL_I2C_Master_Transmit(DataStruct->_i2c_dev,(uint16_t)address,(uint8_t *) reg, sizeof(reg), 1000)!=HAL_OK)
	{
				return SD_MPU6050_Result_Error;
	}

	/* Return OK */
	return SD_MPU6050_Result_Ok;
}

SD_MPU6050_Result SD_MPU6050_SetAccelerometer(SD_MPU6050* DataStruct, SD_MPU6050_Accelerometer AccelerometerSensitivity)
{
	uint8_t temp;
	uint8_t address = DataStruct->Address;
	uint8_t regAdd =(uint8_t )MPU6050_ACCEL_CONFIG;

	/* Config accelerometer */
	if (HAL_I2C_Master_Transmit(DataStruct->_i2c_dev, (uint16_t)address,&regAdd, 1, 1000) != HAL_OK)

				return SD_MPU6050_Result_Error;

	if (HAL_I2C_Master_Receive(DataStruct->_i2c_dev, (uint16_t)address, &temp, 1, 1000) != HAL_OK)

				return SD_MPU6050_Result_Error;

	temp = (temp & 0xE7) | (uint8_t)AccelerometerSensitivity << 3;
	if (HAL_I2C_Master_Transmit(DataStruct->_i2c_dev, (uint16_t)address,&temp, 1, 1000) != HAL_OK)

				return SD_MPU6050_Result_Error;


	/* Set sensitivities for multiplying gyro and accelerometer data */
	switch (AccelerometerSensitivity) {
		case SD_MPU6050_Accelerometer_2G:
			DataStruct->Acce_Mult = (float)1 / MPU6050_ACCE_SENS_2;
			break;
		case SD_MPU6050_Accelerometer_4G:
			DataStruct->Acce_Mult = (float)1 / MPU6050_ACCE_SENS_4;
			break;
		case SD_MPU6050_Accelerometer_8G:
			DataStruct->Acce_Mult = (float)1 / MPU6050_ACCE_SENS_8;
			break;
		case SD_MPU6050_Accelerometer_16G:
			DataStruct->Acce_Mult = (float)1 / MPU6050_ACCE_SENS_16;
			break;
		default:
			break;
		}

	/* Return OK */
	return SD_MPU6050_Result_Ok;
}

SD_MPU6050_Result SD_MPU6050_SetGyroscope(SD_MPU6050* DataStruct, SD_MPU6050_Gyroscope GyroscopeSensitivity)
{
	uint8_t temp;
	uint8_t regAdd =(uint8_t )MPU6050_GYRO_CONFIG;

	/* Config gyroscope */
	while(HAL_I2C_Master_Transmit(DataStruct->_i2c_dev, (uint16_t)DataStruct->Address, &regAdd, 1, 1000) != HAL_OK);
	/*{
				return SD_MPU6050_Result_Error;
	}*/
	while(HAL_I2C_Master_Receive(DataStruct->_i2c_dev, (uint16_t)DataStruct->Address, &temp, 1, 1000) != HAL_OK);
	/*{
				return SD_MPU6050_Result_Error;
	}*/
	temp = (temp & 0xE7) | (uint8_t)GyroscopeSensitivity << 3;
	while(HAL_I2C_Master_Transmit(DataStruct->_i2c_dev, (uint16_t)DataStruct->Address,&temp, 1, 1000) != HAL_OK);
	/*{
				return SD_MPU6050_Result_Error;
	}*/

	switch (GyroscopeSensitivity) {
			case SD_MPU6050_Gyroscope_250s:
				DataStruct->Gyro_Mult = (float)1 / MPU6050_GYRO_SENS_250;
				break;
			case SD_MPU6050_Gyroscope_500s:
				DataStruct->Gyro_Mult = (float)1 / MPU6050_GYRO_SENS_500;
				break;
			case SD_MPU6050_Gyroscope_1000s:
				DataStruct->Gyro_Mult = (float)1 / MPU6050_GYRO_SENS_1000;
				break;
			case SD_MPU6050_Gyroscope_2000s:
				DataStruct->Gyro_Mult = (float)1 / MPU6050_GYRO_SENS_2000;
				break;
			default:
				break;
		}
	/* Return OK */
	return SD_MPU6050_Result_Ok;
}

SD_MPU6050_Result SD_MPU6050_ReadAccelerometer(SD_MPU6050* DataStruct)
{
	uint8_t data[6];
	uint8_t reg = MPU6050_ACCEL_XOUT_H;

	/* Read accelerometer data */
	while(HAL_I2C_Master_Transmit(DataStruct->_i2c_dev, (uint16_t) DataStruct->Address, &reg, 1, 1000) != HAL_OK);

	while(HAL_I2C_Master_Receive(DataStruct->_i2c_dev, (uint16_t) DataStruct->Address, data, 6, 1000) != HAL_OK);

	/* Format */
	DataStruct->Accelerometer_X = (int16_t)(data[0] << 8 | data[1]);
	DataStruct->Accelerometer_Y = (int16_t)(data[2] << 8 | data[3]);
	DataStruct->Accelerometer_Z = (int16_t)(data[4] << 8 | data[5]);

	/* Return OK */
	return SD_MPU6050_Result_Ok;
}
SD_MPU6050_Result SD_MPU6050_ReadGyroscope(SD_MPU6050* DataStruct)
{
	uint8_t data[6];
	uint8_t reg = MPU6050_GYRO_XOUT_H;

	/* Read gyroscope data */
	while(HAL_I2C_Master_Transmit(DataStruct->_i2c_dev, (uint16_t) DataStruct->Address, &reg, 1, 1000) != HAL_OK);

	while(HAL_I2C_Master_Receive(DataStruct->_i2c_dev, (uint16_t) DataStruct->Address, data, 6, 1000) != HAL_OK);

	/* Format */
	DataStruct->Gyroscope_X = (int16_t)(data[0] << 8 | data[1]);
	DataStruct->Gyroscope_Y = (int16_t)(data[2] << 8 | data[3]);
	DataStruct->Gyroscope_Z = (int16_t)(data[4] << 8 | data[5]);

	/* Return OK */
	return SD_MPU6050_Result_Ok;
}
SD_MPU6050_Result SD_MPU6050_ReadTemperature(SD_MPU6050* DataStruct)
{
	uint8_t data[2];
	int16_t temp;
	uint8_t reg = MPU6050_TEMP_OUT_H;

	/* Read temperature */
	while(HAL_I2C_Master_Transmit(DataStruct->_i2c_dev, (uint16_t) DataStruct->Address, &reg, 1, 1000) != HAL_OK);

	while(HAL_I2C_Master_Receive(DataStruct->_i2c_dev, (uint16_t) DataStruct->Address, data, 2, 1000) != HAL_OK);

	/* Format temperature */
	temp = (data[0] << 8 | data[1]);
	DataStruct->Temperature = (float)((int16_t)temp / (float)340.0 + (float)36.53);

	/* Return OK */
	return SD_MPU6050_Result_Ok;
}
SD_MPU6050_Result SD_MPU6050_ReadAll(SD_MPU6050* DataStruct)
{
	uint8_t data[14];
	int16_t temp;
	uint8_t reg = MPU6050_ACCEL_XOUT_H;

	/* Read full raw data, 14bytes */
	while(HAL_I2C_Master_Transmit(DataStruct->_i2c_dev, (uint16_t)DataStruct->Address, &reg, 1, 1000) != HAL_OK);

	while(HAL_I2C_Master_Receive(DataStruct->_i2c_dev, (uint16_t)DataStruct->Address, data, 14, 1000) != HAL_OK);

	/* Format accelerometer data */
	DataStruct->Accelerometer_X = (int16_t)(data[0] << 8 | data[1]);
	DataStruct->Accelerometer_Y = (int16_t)(data[2] << 8 | data[3]);
	DataStruct->Accelerometer_Z = (int16_t)(data[4] << 8 | data[5]);

	/* Format temperature */
	temp = (data[6] << 8 | data[7]);
	DataStruct->Temperature = (float)((float)((int16_t)temp) / (float)340.0 + (float)36.53);

	/* Format gyroscope data */
	DataStruct->Gyroscope_X = (int16_t)(data[8] << 8 | data[9]);
	DataStruct->Gyroscope_Y = (int16_t)(data[10] << 8 | data[11]);
	DataStruct->Gyroscope_Z = (int16_t)(data[12] << 8 | data[13]);

	/* Return OK */
	return SD_MPU6050_Result_Ok;
}


SD_MPU6050_Result SD_MPU6050_EnableInterrupts(SD_MPU6050* DataStruct)
{
	uint8_t temp;
	//uint8_t reg[2] = {MPU6050_INT_ENABLE,0x21};
	uint8_t reg[2] = {MPU6050_INT_ENABLE,0x1};
	uint8_t mpu_reg= MPU6050_INT_PIN_CFG;

	/* Enable interrupts for data ready and motion detect */
		if (HAL_I2C_Master_Transmit(DataStruct->_i2c_dev, (uint16_t)DataStruct->Address, reg, 2, 1000) != HAL_OK)
			return SD_MPU6050_Result_Error;

	/* Clear IRQ flag on any read operation */
	if (HAL_I2C_Master_Transmit(DataStruct->_i2c_dev, (uint16_t)DataStruct->Address, &mpu_reg, 1, 1000) != HAL_OK)
		return SD_MPU6050_Result_Error;


	if (HAL_I2C_Master_Receive(DataStruct->_i2c_dev, (uint16_t)DataStruct->Address, &temp, 1, 1000) != HAL_OK)
		return SD_MPU6050_Result_Error;
	temp |= 0x10;
	reg[0] = MPU6050_INT_PIN_CFG;
	reg[1] = temp;
	while(HAL_I2C_Master_Transmit(DataStruct->_i2c_dev, (uint16_t)DataStruct->Address, reg, 2, 1000) != HAL_OK);



	/* Return OK */
	return SD_MPU6050_Result_Ok;
}
SD_MPU6050_Result SD_MPU6050_DisableInterrupts(SD_MPU6050* DataStruct)
{
	uint8_t reg[2] = {MPU6050_INT_ENABLE,0x00};

	/* Disable interrupts */
	if (HAL_I2C_Master_Transmit(DataStruct->_i2c_dev,(uint16_t)DataStruct->Address,reg,2,1000)!=HAL_OK)
		return SD_MPU6050_Result_Error;
	/* Return OK */
	return SD_MPU6050_Result_Ok;
}
SD_MPU6050_Result SD_MPU6050_ReadInterrupts(SD_MPU6050* DataStruct, SD_MPU6050_Interrupt* InterruptsStruct)
{
	uint8_t read;

	/* Reset structure */
	InterruptsStruct->Status = 0;
	uint8_t reg = MPU6050_INT_STATUS;

	if (HAL_I2C_Master_Transmit(DataStruct->_i2c_dev, (uint16_t)DataStruct->Address, &reg, 1, 1000) != HAL_OK)
		return SD_MPU6050_Result_Error;
	if (HAL_I2C_Master_Receive(DataStruct->_i2c_dev, (uint16_t)DataStruct->Address, &read, 1, 1000) != HAL_OK)
		return SD_MPU6050_Result_Error;
	/* Fill value */
	InterruptsStruct->Status = read;
	/* Return OK */
	return SD_MPU6050_Result_Ok;
}

SD_MPU6050_Result SD_MPU6050_EnableAccelFifo(SD_MPU6050* DataStruct)
{
	uint8_t reg[2] = {MPU6050_RA_FIFO_EN,0x00};

	if (HAL_I2C_Master_Transmit(DataStruct->_i2c_dev, (uint16_t)DataStruct->Address, reg, 1, 1000) != HAL_OK)
		return SD_MPU6050_Result_Error;
	if (HAL_I2C_Master_Receive(DataStruct->_i2c_dev, (uint16_t)DataStruct->Address, &reg[1], 1, 1000) != HAL_OK)
		return SD_MPU6050_Result_Error;

	reg[1] = (1 << MPU6050_ACCEL_FIFO_EN_BIT);

	if (HAL_I2C_Master_Transmit(DataStruct->_i2c_dev, (uint16_t)DataStruct->Address, reg, sizeof(reg), 1000) != HAL_OK)
		return SD_MPU6050_Result_Error;
	/* Return OK */
	return SD_MPU6050_Result_Ok;
}


SD_MPU6050_Result SD_MPU6050_EnableFifo(SD_MPU6050* DataStruct)
{
	uint8_t reg[2] = {MPU6050_RA_USER_CTRL,0x00};

	// read register
	if (HAL_I2C_Master_Transmit(DataStruct->_i2c_dev, (uint16_t)DataStruct->Address, reg, 1, 1000) != HAL_OK)
		return SD_MPU6050_Result_Error;
	if (HAL_I2C_Master_Receive(DataStruct->_i2c_dev, (uint16_t)DataStruct->Address, &reg[1], 1, 1000) != HAL_OK)
		return SD_MPU6050_Result_Error;

	// activate fifo bit
	reg[1] |= (1 << MPU6050_USERCTRL_FIFO_EN_BIT);
	if (HAL_I2C_Master_Transmit(DataStruct->_i2c_dev, (uint16_t)DataStruct->Address, reg, sizeof(reg), 1000) != HAL_OK)
		return SD_MPU6050_Result_Error;

	/* Return OK */
	return SD_MPU6050_Result_Ok;

}


SD_MPU6050_Result SD_MPU6050_GetFifoCount (SD_MPU6050* DataStruct, uint16_t* size)
{
	uint8_t reg = MPU6050_RA_FIFO_COUNTH;
	uint8_t res[2];

/*
	if (HAL_I2C_Master_Transmit(DataStruct->_i2c_dev, (uint16_t)DataStruct->Address, &reg, 1, 1000) != HAL_OK)
		return SD_MPU6050_Result_Error;
	if (HAL_I2C_Master_Receive(DataStruct->_i2c_dev, (uint16_t)DataStruct->Address, &res[0], 1, 1000) != HAL_OK)
		return SD_MPU6050_Result_Error;
	reg = MPU6050_RA_FIFO_COUNTL;
	if (HAL_I2C_Master_Transmit(DataStruct->_i2c_dev, (uint16_t)DataStruct->Address, &reg, 1, 1000) != HAL_OK)
		return SD_MPU6050_Result_Error;
	if(HAL_I2C_Master_Receive(DataStruct->_i2c_dev, (uint16_t)DataStruct->Address, &res[1], 1, 1000) != HAL_OK)
		return SD_MPU6050_Result_Error;
*/
	reg = MPU6050_RA_FIFO_COUNTH;
	if (HAL_I2C_Master_Transmit(DataStruct->_i2c_dev, (uint16_t)DataStruct->Address, &reg, 1, 1000) != HAL_OK)
			return SD_MPU6050_Result_Error;
	if (HAL_I2C_Master_Receive(DataStruct->_i2c_dev, (uint16_t)DataStruct->Address, &res[0], 2, 1000) != HAL_OK)
			return SD_MPU6050_Result_Error;

	*size = (res[0] << 8) + res[1];


	/* Return OK */
	return SD_MPU6050_Result_Ok;
}
SD_MPU6050_Result SD_MPU6050_ReadFifo (SD_MPU6050* DataStruct, uint16_t size, uint8_t* pbuf)
{
	if (HAL_I2C_Mem_Read(DataStruct->_i2c_dev, (uint16_t)DataStruct->Address, MPU6050_RA_FIFO_R_W, 1, pbuf, size, 1000) != HAL_OK)
		return SD_MPU6050_Result_Error;

	return SD_MPU6050_Result_Ok;
}

SD_MPU6050_Result SD_MPU6050_Reset (SD_MPU6050* DataStruct)
{
	uint8_t reg[2];

	// reset mpu
	reg[0] = MPU6050_PWR_MGMT_1;
	reg[1] = 1 << MPU6050_PWR1_DEVICE_RESET_BIT;
	if(HAL_I2C_Master_Transmit(DataStruct->_i2c_dev,(uint16_t) DataStruct->Address , (uint8_t *) reg, 2, 1000) != HAL_OK)
	{
		return SD_MPU6050_Result_Error;
	}

	HAL_Delay(50); // wait reset

	// start mpu
	reg[0] = MPU6050_PWR_MGMT_1;
	reg[1] = 0x00;

	/* Try to transmit via I2C */
	if(HAL_I2C_Master_Transmit(DataStruct->_i2c_dev,(uint16_t) DataStruct->Address , (uint8_t *) reg, 2, 1000) != HAL_OK)
	{
				return SD_MPU6050_Result_Error;
	}

	return SD_MPU6050_Result_Ok;
}

SD_MPU6050_Result SD_MPU6050_setMemoryBank(SD_MPU6050* DataStruct, uint8_t bank, uint8_t prefetchEnabled, uint8_t userBank) {
	uint8_t reg[2];

	reg[0] = MPU6050_RA_BANK_SEL;

	bank &= 0x1F;
    if (userBank) bank |= 0x20;
    if (prefetchEnabled) bank |= 0x40;
    reg[1] = bank;

    if(HAL_I2C_Master_Transmit(DataStruct->_i2c_dev,(uint16_t) DataStruct->Address , (uint8_t *) reg, sizeof(reg), 1000) != HAL_OK)
	{
				return SD_MPU6050_Result_Error;
	}
    return SD_MPU6050_Result_Ok;
}

// MEM_START_ADDR register

SD_MPU6050_Result MPU6050_setMemoryStartAddress(SD_MPU6050* DataStruct, uint8_t address)
{
	uint8_t reg[2];
	reg[0] = MPU6050_RA_MEM_START_ADDR;
	reg[1] = address;

	if(HAL_I2C_Master_Transmit(DataStruct->_i2c_dev,(uint16_t) DataStruct->Address , (uint8_t *) reg, sizeof(reg), 1000) != HAL_OK)
	{
				return SD_MPU6050_Result_Error;
	}
	return SD_MPU6050_Result_Ok;
}

// MEM_R_W register

SD_MPU6050_Result MPU6050_readMemoryByte(SD_MPU6050* DataStruct, uint8_t* pbuf)
{

	uint8_t reg = MPU6050_RA_MEM_R_W;
	if (HAL_I2C_Master_Transmit(DataStruct->_i2c_dev, (uint16_t)DataStruct->Address, &reg, 1, 1000) != HAL_OK)
			return SD_MPU6050_Result_Error;
	if (HAL_I2C_Master_Receive(DataStruct->_i2c_dev, (uint16_t)DataStruct->Address, pbuf, 1, 1000) != HAL_OK)
			return SD_MPU6050_Result_Error;

    return SD_MPU6050_Result_Ok;
}

//void MPU6050_writeMemoryByte(uint8_t data) {
//    I2Cdev_writeByte(mpu6050.devAddr, MPU6050_RA_MEM_R_W, data);
//}
/*
void MPU6050_readMemoryBlock(uint8_t *data, uint16_t dataSize, uint8_t bank, uint8_t address) {
    MPU6050_setMemoryBank(bank, false, false);
    MPU6050_setMemoryStartAddress(address);
    uint8_t chunkSize;
    for (uint16_t i = 0; i < dataSize;) {
        // determine correct chunk size according to bank position and data size
        chunkSize = MPU6050_DMP_MEMORY_CHUNK_SIZE;

        // make sure we don't go past the data size
        if (i + chunkSize > dataSize) chunkSize = dataSize - i;

        // make sure this chunk doesn't go past the bank boundary (256 bytes)
        if (chunkSize > 256 - address) chunkSize = 256 - address;

        // read the chunk of data as specified
        I2Cdev_readBytes(mpu6050.devAddr, MPU6050_RA_MEM_R_W, chunkSize, data + i);

        // increase byte index by [chunkSize]
        i += chunkSize;

        // uint8_t automatically wraps to 0 at 256
        address += chunkSize;

        // if we aren't done, update bank (if necessary) and address
        if (i < dataSize) {
            if (address == 0) bank++;
            MPU6050_setMemoryBank(bank, false, false);
            MPU6050_setMemoryStartAddress(address);
        }
    }
}
*/
#if 0
SD_MPU6050_Result  SD_MPU6050_dmpInitialize(I2C_HandleTypeDef* I2Cx,SD_MPU6050* DataStruct, SD_MPU6050_Device DeviceNumber, SD_MPU6050_Accelerometer AccelerometerSensitivity, SD_MPU6050_Gyroscope GyroscopeSensitivity)
{
	// TODO: refactor init to avoid side effect
    // initialize device
	if (SD_MPU6050_Init(I2Cx, DataStruct, DeviceNumber, AccelerometerSensitivity, GyroscopeSensitivity) != SD_MPU6050_Result_Ok)
		return SD_MPU6050_Result_Error;


    // reset memory Bank
    setMemoryBank(0, 0, 0);

    // check OTP bank valid
#if _DEBUG
    printf ("Reading OTP bank valid flag...\n"));
    uint8_t otpValid = getOTPBankValid();
    printf (("OTP bank is "));
    printf (otpValid ? ("valid!") : ("invalid!"));
    printf ("\n");
#endif

    // get X/Y/Z gyro offsets
    DEBUG_PRINTLN(F("Reading gyro offset values..."));
    int8_t xgOffset = getXGyroOffset();
    int8_t ygOffset = getYGyroOffset();
    int8_t zgOffset = getZGyroOffset();
    DEBUG_PRINT(F("X gyro offset = "));
    DEBUG_PRINTLN(xgOffset);
    DEBUG_PRINT(F("Y gyro offset = "));
    DEBUG_PRINTLN(ygOffset);
    DEBUG_PRINT(F("Z gyro offset = "));
    DEBUG_PRINTLN(zgOffset);

    I2Cdev::readByte(devAddr, MPU6050_RA_USER_CTRL, buffer); // ?

    DEBUG_PRINTLN(F("Enabling interrupt latch, clear on any read, AUX bypass enabled"));
    I2Cdev::writeByte(devAddr, MPU6050_RA_INT_PIN_CFG, 0x32);

    // enable MPU AUX I2C bypass mode
    //DEBUG_PRINTLN(F("Enabling AUX I2C bypass mode..."));
    //setI2CBypassEnabled(true);

    DEBUG_PRINTLN(F("Setting magnetometer mode to power-down..."));
    //mag -> setMode(0);
    I2Cdev::writeByte(0x0E, 0x0A, 0x00);

    DEBUG_PRINTLN(F("Setting magnetometer mode to fuse access..."));
    //mag -> setMode(0x0F);
    I2Cdev::writeByte(0x0E, 0x0A, 0x0F);

    DEBUG_PRINTLN(F("Reading mag magnetometer factory calibration..."));
    int8_t asax, asay, asaz;
    //mag -> getAdjustment(&asax, &asay, &asaz);
    I2Cdev::readBytes(0x0E, 0x10, 3, buffer);
    asax = (int8_t)buffer[0];
    asay = (int8_t)buffer[1];
    asaz = (int8_t)buffer[2];
    DEBUG_PRINT(F("Adjustment X/Y/Z = "));
    DEBUG_PRINT(asax);
    DEBUG_PRINT(F(" / "));
    DEBUG_PRINT(asay);
    DEBUG_PRINT(F(" / "));
    DEBUG_PRINTLN(asaz);

    DEBUG_PRINTLN(F("Setting magnetometer mode to power-down..."));
    //mag -> setMode(0);
    I2Cdev::writeByte(0x0E, 0x0A, 0x00);

    // load DMP code into memory banks
    DEBUG_PRINT(F("Writing DMP code to MPU memory banks ("));
    DEBUG_PRINT(MPU6050_DMP_CODE_SIZE);
    DEBUG_PRINTLN(F(" bytes)"));
    if (writeProgMemoryBlock(dmpMemory, MPU6050_DMP_CODE_SIZE)) {
        DEBUG_PRINTLN(F("Success! DMP code written and verified."));

        DEBUG_PRINTLN(F("Configuring DMP and related settings..."));

        // write DMP configuration
        DEBUG_PRINT(F("Writing DMP configuration to MPU memory banks ("));
        DEBUG_PRINT(MPU6050_DMP_CONFIG_SIZE);
        DEBUG_PRINTLN(F(" bytes in config def)"));
        if (writeProgDMPConfigurationSet(dmpConfig, MPU6050_DMP_CONFIG_SIZE)) {
            DEBUG_PRINTLN(F("Success! DMP configuration written and verified."));

            DEBUG_PRINTLN(F("Setting DMP and FIFO_OFLOW interrupts enabled..."));
            setIntEnabled(0x12);

            DEBUG_PRINTLN(F("Setting sample rate to 200Hz..."));
            setRate(4); // 1khz / (1 + 4) = 200 Hz

            DEBUG_PRINTLN(F("Setting clock source to Z Gyro..."));
            setClockSource(MPU6050_CLOCK_PLL_ZGYRO);

            DEBUG_PRINTLN(F("Setting DLPF bandwidth to 42Hz..."));
            setDLPFMode(MPU6050_DLPF_BW_42);

            DEBUG_PRINTLN(F("Setting external frame sync to TEMP_OUT_L[0]..."));
            setExternalFrameSync(MPU6050_EXT_SYNC_TEMP_OUT_L);

            DEBUG_PRINTLN(F("Setting gyro sensitivity to +/- 2000 deg/sec..."));
            setFullScaleGyroRange(MPU6050_GYRO_FS_2000);

            DEBUG_PRINTLN(F("Setting DMP configuration bytes (function unknown)..."));
            setDMPConfig1(0x03);
            setDMPConfig2(0x00);

            DEBUG_PRINTLN(F("Clearing OTP Bank flag..."));
            setOTPBankValid(false);

            DEBUG_PRINTLN(F("Setting X/Y/Z gyro offsets to previous values..."));
            setXGyroOffsetTC(xgOffset);
            setYGyroOffsetTC(ygOffset);
            setZGyroOffsetTC(zgOffset);

            //DEBUG_PRINTLN(F("Setting X/Y/Z gyro user offsets to zero..."));
            //setXGyroOffset(0);
            //setYGyroOffset(0);
            //setZGyroOffset(0);

            DEBUG_PRINTLN(F("Writing final memory update 1/19 (function unknown)..."));
            uint8_t dmpUpdate[16], j;
            uint16_t pos = 0;
            for (j = 0; j < 4 || j < dmpUpdate[2] + 3; j++, pos++) dmpUpdate[j] = pgm_read_byte(&dmpUpdates[pos]);
            writeMemoryBlock(dmpUpdate + 3, dmpUpdate[2], dmpUpdate[0], dmpUpdate[1]);

            DEBUG_PRINTLN(F("Writing final memory update 2/19 (function unknown)..."));
            for (j = 0; j < 4 || j < dmpUpdate[2] + 3; j++, pos++) dmpUpdate[j] = pgm_read_byte(&dmpUpdates[pos]);
            writeMemoryBlock(dmpUpdate + 3, dmpUpdate[2], dmpUpdate[0], dmpUpdate[1]);

            DEBUG_PRINTLN(F("Resetting FIFO..."));
            resetFIFO();

            DEBUG_PRINTLN(F("Reading FIFO count..."));
            uint8_t fifoCount = getFIFOCount();

            DEBUG_PRINT(F("Current FIFO count="));
            DEBUG_PRINTLN(fifoCount);
            uint8_t fifoBuffer[128];
            //getFIFOBytes(fifoBuffer, fifoCount);

            DEBUG_PRINTLN(F("Writing final memory update 3/19 (function unknown)..."));
            for (j = 0; j < 4 || j < dmpUpdate[2] + 3; j++, pos++) dmpUpdate[j] = pgm_read_byte(&dmpUpdates[pos]);
            writeMemoryBlock(dmpUpdate + 3, dmpUpdate[2], dmpUpdate[0], dmpUpdate[1]);

            DEBUG_PRINTLN(F("Writing final memory update 4/19 (function unknown)..."));
            for (j = 0; j < 4 || j < dmpUpdate[2] + 3; j++, pos++) dmpUpdate[j] = pgm_read_byte(&dmpUpdates[pos]);
            writeMemoryBlock(dmpUpdate + 3, dmpUpdate[2], dmpUpdate[0], dmpUpdate[1]);

            DEBUG_PRINTLN(F("Disabling all standby flags..."));
            I2Cdev::writeByte(0x68, MPU6050_RA_PWR_MGMT_2, 0x00);

            DEBUG_PRINTLN(F("Setting accelerometer sensitivity to +/- 2g..."));
            I2Cdev::writeByte(0x68, MPU6050_RA_ACCEL_CONFIG, 0x00);

            DEBUG_PRINTLN(F("Setting motion detection threshold to 2..."));
            setMotionDetectionThreshold(2);

            DEBUG_PRINTLN(F("Setting zero-motion detection threshold to 156..."));
            setZeroMotionDetectionThreshold(156);

            DEBUG_PRINTLN(F("Setting motion detection duration to 80..."));
            setMotionDetectionDuration(80);

            DEBUG_PRINTLN(F("Setting zero-motion detection duration to 0..."));
            setZeroMotionDetectionDuration(0);

            DEBUG_PRINTLN(F("Setting AK8975 to single measurement mode..."));
            //mag -> setMode(1);
            I2Cdev::writeByte(0x0E, 0x0A, 0x01);

            // setup AK8975 (0x0E) as Slave 0 in read mode
            DEBUG_PRINTLN(F("Setting up AK8975 read slave 0..."));
            I2Cdev::writeByte(0x68, MPU6050_RA_I2C_SLV0_ADDR, 0x8E);
            I2Cdev::writeByte(0x68, MPU6050_RA_I2C_SLV0_REG,  0x01);
            I2Cdev::writeByte(0x68, MPU6050_RA_I2C_SLV0_CTRL, 0xDA);

            // setup AK8975 (0x0E) as Slave 2 in write mode
            DEBUG_PRINTLN(F("Setting up AK8975 write slave 2..."));
            I2Cdev::writeByte(0x68, MPU6050_RA_I2C_SLV2_ADDR, 0x0E);
            I2Cdev::writeByte(0x68, MPU6050_RA_I2C_SLV2_REG,  0x0A);
            I2Cdev::writeByte(0x68, MPU6050_RA_I2C_SLV2_CTRL, 0x81);
            I2Cdev::writeByte(0x68, MPU6050_RA_I2C_SLV2_DO,   0x01);

            // setup I2C timing/delay control
            DEBUG_PRINTLN(F("Setting up slave access delay..."));
            I2Cdev::writeByte(0x68, MPU6050_RA_I2C_SLV4_CTRL, 0x18);
            I2Cdev::writeByte(0x68, MPU6050_RA_I2C_MST_DELAY_CTRL, 0x05);

            // enable interrupts
            DEBUG_PRINTLN(F("Enabling default interrupt behavior/no bypass..."));
            I2Cdev::writeByte(0x68, MPU6050_RA_INT_PIN_CFG, 0x00);

            // enable I2C master mode and reset DMP/FIFO
            DEBUG_PRINTLN(F("Enabling I2C master mode..."));
            I2Cdev::writeByte(0x68, MPU6050_RA_USER_CTRL, 0x20);
            DEBUG_PRINTLN(F("Resetting FIFO..."));
            I2Cdev::writeByte(0x68, MPU6050_RA_USER_CTRL, 0x24);
            DEBUG_PRINTLN(F("Rewriting I2C master mode enabled because...I don't know"));
            I2Cdev::writeByte(0x68, MPU6050_RA_USER_CTRL, 0x20);
            DEBUG_PRINTLN(F("Enabling and resetting DMP/FIFO..."));
            I2Cdev::writeByte(0x68, MPU6050_RA_USER_CTRL, 0xE8);

            DEBUG_PRINTLN(F("Writing final memory update 5/19 (function unknown)..."));
            for (j = 0; j < 4 || j < dmpUpdate[2] + 3; j++, pos++) dmpUpdate[j] = pgm_read_byte(&dmpUpdates[pos]);
            writeMemoryBlock(dmpUpdate + 3, dmpUpdate[2], dmpUpdate[0], dmpUpdate[1]);
            DEBUG_PRINTLN(F("Writing final memory update 6/19 (function unknown)..."));
            for (j = 0; j < 4 || j < dmpUpdate[2] + 3; j++, pos++) dmpUpdate[j] = pgm_read_byte(&dmpUpdates[pos]);
            writeMemoryBlock(dmpUpdate + 3, dmpUpdate[2], dmpUpdate[0], dmpUpdate[1]);
            DEBUG_PRINTLN(F("Writing final memory update 7/19 (function unknown)..."));
            for (j = 0; j < 4 || j < dmpUpdate[2] + 3; j++, pos++) dmpUpdate[j] = pgm_read_byte(&dmpUpdates[pos]);
            writeMemoryBlock(dmpUpdate + 3, dmpUpdate[2], dmpUpdate[0], dmpUpdate[1]);
            DEBUG_PRINTLN(F("Writing final memory update 8/19 (function unknown)..."));
            for (j = 0; j < 4 || j < dmpUpdate[2] + 3; j++, pos++) dmpUpdate[j] = pgm_read_byte(&dmpUpdates[pos]);
            writeMemoryBlock(dmpUpdate + 3, dmpUpdate[2], dmpUpdate[0], dmpUpdate[1]);
            DEBUG_PRINTLN(F("Writing final memory update 9/19 (function unknown)..."));
            for (j = 0; j < 4 || j < dmpUpdate[2] + 3; j++, pos++) dmpUpdate[j] = pgm_read_byte(&dmpUpdates[pos]);
            writeMemoryBlock(dmpUpdate + 3, dmpUpdate[2], dmpUpdate[0], dmpUpdate[1]);
            DEBUG_PRINTLN(F("Writing final memory update 10/19 (function unknown)..."));
            for (j = 0; j < 4 || j < dmpUpdate[2] + 3; j++, pos++) dmpUpdate[j] = pgm_read_byte(&dmpUpdates[pos]);
            writeMemoryBlock(dmpUpdate + 3, dmpUpdate[2], dmpUpdate[0], dmpUpdate[1]);
            DEBUG_PRINTLN(F("Writing final memory update 11/19 (function unknown)..."));
            for (j = 0; j < 4 || j < dmpUpdate[2] + 3; j++, pos++) dmpUpdate[j] = pgm_read_byte(&dmpUpdates[pos]);
            writeMemoryBlock(dmpUpdate + 3, dmpUpdate[2], dmpUpdate[0], dmpUpdate[1]);

            DEBUG_PRINTLN(F("Reading final memory update 12/19 (function unknown)..."));
            for (j = 0; j < 4 || j < dmpUpdate[2] + 3; j++, pos++) dmpUpdate[j] = pgm_read_byte(&dmpUpdates[pos]);
            readMemoryBlock(dmpUpdate + 3, dmpUpdate[2], dmpUpdate[0], dmpUpdate[1]);
            #ifdef DEBUG
                DEBUG_PRINT(F("Read bytes: "));
                for (j = 0; j < 4; j++) {
                    DEBUG_PRINTF(dmpUpdate[3 + j], HEX);
                    DEBUG_PRINT(" ");
                }
                DEBUG_PRINTLN("");
            #endif

            DEBUG_PRINTLN(F("Writing final memory update 13/19 (function unknown)..."));
            for (j = 0; j < 4 || j < dmpUpdate[2] + 3; j++, pos++) dmpUpdate[j] = pgm_read_byte(&dmpUpdates[pos]);
            writeMemoryBlock(dmpUpdate + 3, dmpUpdate[2], dmpUpdate[0], dmpUpdate[1]);
            DEBUG_PRINTLN(F("Writing final memory update 14/19 (function unknown)..."));
            for (j = 0; j < 4 || j < dmpUpdate[2] + 3; j++, pos++) dmpUpdate[j] = pgm_read_byte(&dmpUpdates[pos]);
            writeMemoryBlock(dmpUpdate + 3, dmpUpdate[2], dmpUpdate[0], dmpUpdate[1]);
            DEBUG_PRINTLN(F("Writing final memory update 15/19 (function unknown)..."));
            for (j = 0; j < 4 || j < dmpUpdate[2] + 3; j++, pos++) dmpUpdate[j] = pgm_read_byte(&dmpUpdates[pos]);
            writeMemoryBlock(dmpUpdate + 3, dmpUpdate[2], dmpUpdate[0], dmpUpdate[1]);
            DEBUG_PRINTLN(F("Writing final memory update 16/19 (function unknown)..."));
            for (j = 0; j < 4 || j < dmpUpdate[2] + 3; j++, pos++) dmpUpdate[j] = pgm_read_byte(&dmpUpdates[pos]);
            writeMemoryBlock(dmpUpdate + 3, dmpUpdate[2], dmpUpdate[0], dmpUpdate[1]);
            DEBUG_PRINTLN(F("Writing final memory update 17/19 (function unknown)..."));
            for (j = 0; j < 4 || j < dmpUpdate[2] + 3; j++, pos++) dmpUpdate[j] = pgm_read_byte(&dmpUpdates[pos]);
            writeMemoryBlock(dmpUpdate + 3, dmpUpdate[2], dmpUpdate[0], dmpUpdate[1]);

            DEBUG_PRINTLN(F("Waiting for FIRO count >= 46..."));
            while ((fifoCount = getFIFOCount()) < 46);
            DEBUG_PRINTLN(F("Reading FIFO..."));
            getFIFOBytes(fifoBuffer, min(fifoCount, 128)); // safeguard only 128 bytes
            DEBUG_PRINTLN(F("Reading interrupt status..."));
            getIntStatus();

            DEBUG_PRINTLN(F("Writing final memory update 18/19 (function unknown)..."));
            for (j = 0; j < 4 || j < dmpUpdate[2] + 3; j++, pos++) dmpUpdate[j] = pgm_read_byte(&dmpUpdates[pos]);
            writeMemoryBlock(dmpUpdate + 3, dmpUpdate[2], dmpUpdate[0], dmpUpdate[1]);

            DEBUG_PRINTLN(F("Waiting for FIRO count >= 48..."));
            while ((fifoCount = getFIFOCount()) < 48);
            DEBUG_PRINTLN(F("Reading FIFO..."));
            getFIFOBytes(fifoBuffer, min(fifoCount, 128)); // safeguard only 128 bytes
            DEBUG_PRINTLN(F("Reading interrupt status..."));
            getIntStatus();
            DEBUG_PRINTLN(F("Waiting for FIRO count >= 48..."));
            while ((fifoCount = getFIFOCount()) < 48);
            DEBUG_PRINTLN(F("Reading FIFO..."));
            getFIFOBytes(fifoBuffer, min(fifoCount, 128)); // safeguard only 128 bytes
            DEBUG_PRINTLN(F("Reading interrupt status..."));
            getIntStatus();

            DEBUG_PRINTLN(F("Writing final memory update 19/19 (function unknown)..."));
            for (j = 0; j < 4 || j < dmpUpdate[2] + 3; j++, pos++) dmpUpdate[j] = pgm_read_byte(&dmpUpdates[pos]);
            writeMemoryBlock(dmpUpdate + 3, dmpUpdate[2], dmpUpdate[0], dmpUpdate[1]);

            DEBUG_PRINTLN(F("Disabling DMP (you turn it on later)..."));
            setDMPEnabled(false);

            DEBUG_PRINTLN(F("Setting up internal 48-byte (default) DMP packet buffer..."));
            dmpPacketSize = 48;
            /*if ((dmpPacketBuffer = (uint8_t *)malloc(42)) == 0) {
                return 3; // TODO: proper error code for no memory
            }*/

            DEBUG_PRINTLN(F("Resetting FIFO and clearing INT status one last time..."));
            resetFIFO();
            getIntStatus();
        } else {
            DEBUG_PRINTLN(F("ERROR! DMP configuration verification failed."));
            return 2; // configuration block loading failed
        }
    } else {
        DEBUG_PRINTLN(F("ERROR! DMP code verification failed."));
        return 1; // main binary block loading failed
    }
    return 0; // success
}
#endif
