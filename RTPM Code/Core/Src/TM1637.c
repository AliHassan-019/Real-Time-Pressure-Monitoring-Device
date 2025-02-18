#include "stm32l4xx_hal.h"
#include "tm1637.h"

// Define the GPIO pins for CLK and DIO
#define TM1637_CLK_PIN GPIO_PIN_0
#define TM1637_CLK_PORT GPIOB
#define TM1637_DIO_PIN GPIO_PIN_1
#define TM1637_DIO_PORT GPIOB

// Initialize the GPIO pins for the TM1637
void TM1637_GPIO_Init(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // Enable the GPIO clocks
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    // Configure GPIO pin for CLK
    GPIO_InitStruct.Pin = TM1637_CLK_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(TM1637_CLK_PORT, &GPIO_InitStruct);

    // Configure GPIO pin for DIO
    GPIO_InitStruct.Pin = TM1637_DIO_PIN;
    HAL_GPIO_Init(TM1637_DIO_PORT, &GPIO_InitStruct);
}

// Set the state of CLK pin
void TM1637_Set_CLK(GPIO_PinState state) {
    HAL_GPIO_WritePin(TM1637_CLK_PORT, TM1637_CLK_PIN, state);
}

// Set the state of DIO pin
void TM1637_Set_DIO(GPIO_PinState state) {
    HAL_GPIO_WritePin(TM1637_DIO_PORT, TM1637_DIO_PIN, state);
}

// Read the state of DIO pin (for acknowledge)
GPIO_PinState TM1637_Read_DIO(void) {
    return HAL_GPIO_ReadPin(TM1637_DIO_PORT, TM1637_DIO_PIN);
}

// Small delay in microseconds
void TM1637_Delay_us(uint16_t us) {
    uint32_t start = DWT->CYCCNT;
    uint32_t cycles = (SystemCoreClock / 1000000L) * us;  // Convert microseconds to cycles
    while ((DWT->CYCCNT - start) < cycles);
}

// Start condition for TM1637
void TM1637_Start(void) {
    TM1637_Set_CLK(GPIO_PIN_SET);
    TM1637_Set_DIO(GPIO_PIN_SET);
    TM1637_Delay_us(2);
    TM1637_Set_DIO(GPIO_PIN_RESET);
}

// Stop condition for TM1637
void TM1637_Stop(void) {
    TM1637_Set_CLK(GPIO_PIN_RESET);
    TM1637_Set_DIO(GPIO_PIN_RESET);
    TM1637_Set_CLK(GPIO_PIN_SET);
    TM1637_Delay_us(2);
    TM1637_Set_DIO(GPIO_PIN_SET);
}

// Write a byte to TM1637
uint8_t TM1637_WriteByte(uint8_t data) {
    for (int i = 0; i < 8; i++) {
        TM1637_Set_CLK(GPIO_PIN_RESET);
        if (data & 0x01) {
            TM1637_Set_DIO(GPIO_PIN_SET);
        } else {
            TM1637_Set_DIO(GPIO_PIN_RESET);
        }
        data >>= 1;
        TM1637_Set_CLK(GPIO_PIN_SET);
        TM1637_Delay_us(2);
    }

    // Acknowledge bit
    TM1637_Set_CLK(GPIO_PIN_RESET);
    TM1637_Set_DIO(GPIO_PIN_SET);
    TM1637_Set_CLK(GPIO_PIN_SET);
    TM1637_Delay_us(2);

    // Acknowledge response from TM1637
    GPIO_PinState ack = TM1637_Read_DIO();
    TM1637_Set_CLK(GPIO_PIN_RESET);
    return ack == GPIO_PIN_RESET;  // Return 1 if acknowledged
}

// Display data on TM1637 (4 digits)
void TM1637_Display(uint8_t segments[4]) {
    TM1637_Start();
    TM1637_WriteByte(0x40);  // Command to set data
    TM1637_Stop();

    TM1637_Start();
    TM1637_WriteByte(0xC0);  // Command to set address
    for (int i = 0; i < 4; i++) {
        TM1637_WriteByte(segments[i]);
    }
    TM1637_Stop();

    TM1637_Start();
    TM1637_WriteByte(0x88 | 0x07);  // Display control (brightness)
    TM1637_Stop();
}

// Convert number to 7-segment display pattern
uint8_t TM1637_EncodeDigit(uint8_t digit) {
    static const uint8_t digitToSegment[] = {
        0x3F, // 0
        0x06, // 1
        0x5B, // 2
        0x4F, // 3
        0x66, // 4
        0x6D, // 5
        0x7D, // 6
        0x07, // 7
        0x7F, // 8
        0x6F  // 9
    };
    return digitToSegment[digit];
}

// Example: Display a number on TM1637 (0-9999)
void TM1637_DisplayNumber(int num) {
    uint8_t digits[4];
    digits[0] = TM1637_EncodeDigit((num / 1000) % 10);
    digits[1] = TM1637_EncodeDigit((num / 100) % 10);
    digits[2] = TM1637_EncodeDigit((num / 10) % 10);
    digits[3] = TM1637_EncodeDigit(num % 10);
    TM1637_Display(digits);
}
