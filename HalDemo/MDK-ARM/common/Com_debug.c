#include "Com_debug.h"

// 重映射printf函数到串口
int fputc(int ch, FILE *f) // 参数：ch是要输出的字符，参数f是文件指针，通常指向stdout
{
    HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xFFFF);
    return ch;
}
