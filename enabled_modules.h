#ifndef _ENABLED_MODULES_H
#define _ENABLED_MODULES_H

/*!
*   @brief Fast Malloc module for allocated HEAP data onto RAMBANK1 on Teensy4
*   @note This is threadsafe module, non-thread-safe module is always availabe since it's used in scheduler
*/
//#define OS_FAST_MALLOC_MODULE

/*!
*   @brief Low priority work thread module
*/
//#define LPWORK_MODULE

/*!
*   @brief Protobuffer Callbacks Handler module
*/
//#define PROTOCALLBACKS_MODULE

/*!
*   @brief Operating System Semaphore Module
*/
#define SEMAPHORE_MODULE

/*!
*   @brief Operating System Mutex Module
*/
#define MUTEX_MODULE

/*!
*   @brief Operating System Signaling Module
*/
#define SIGNALING_MODULE

/*!
*   @brief Included SSD1351 OLED Module for easy DMA manipulation
*   @note Depends on OSThreadKernel Module
*/
// #define SSD1351_MODULE

/*!
*   @brief Included APA102 SPI Based LED strip module for easy DMA manipulation
*   @note Depends on OSThreadKernel Module
*/
// #define APA102_MODULE

/*!
*   @brief IMU module for dealing with mpu6050 stuff
*/
//#define MPU6050_MODULE

/*!
*   @brief Module that controls the max7219 dotmatrix module
*/
#define MAX7219_DOTMATRIX_MODULE 

/*!
*   @brief Module that controls the LiquidCrystalI2C Library
*/
#define LIQUIDCRYSTAL_I2C_MODULE

/*!
*   @brief Easy module that allows us to read voltages 
*/
//#define VOLTAGE_READ_MODULE

/*!
*   @brief Threadsafe UART Module
*/
//#define SERIAL_MODULE 

/*!
*   @brief Priority Queue(Heap approach) module
*   @note Decently fast(O(log(n)) implementation speed), but sifting through array could take more cpu cycles than 
*   @note naive approach. Use this if you are popping off AND pushing data often. Otherwise if you are just adding a bunch of data
*   @note And reading through it, it's recomended to just use the naive approach if you are gonna add a bunch of elements to a list
*   @note and read through them often
*/
//#define PRIORITY_QUEUE_HEAP

/*!
*   @brief Red Black tree node self balancing tree module
*/
// #define RED_BLACK_TREE_MODULE

/*!
*   @brief AVL self balancing tree module
*/
// #define AVL_TREE_MODULE

/*!
*   @brief Circular string buffer module. 
*   @note allows us to save a bunch of strings into a circular buffer for debugging purposes. 
*/
// #define CIRCULAR_BUFFER_STRING_MODULE 

/*!
*   @brief Circular Buffer Queue Module 
*/
// #define CIRCULAR_BUFFER_POINTER_MODULE 

/*!
*   @brief Binary Search Tree Module
*   @note Can lead to an ubalanced tree, since there's no balancing happening
*   @note However, will use less stack space then recursive methods of tree sorting
*/
// #define BINARY_SEARCH_TREE_MODULE

#endif 