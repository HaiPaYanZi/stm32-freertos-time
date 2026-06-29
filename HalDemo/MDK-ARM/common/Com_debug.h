#ifndef __COM_DEBUG_H
#define __COM_DEBUG_H
#include "usart.h"
#include <stdio.h>
#include <stdarg.h>
// 1.设置日志打印开关 占CPU时间，在实际应用中，但可以在开发中使用
// 2.添加前缀，补全文件名、行号等信息，方便定位
#define DEBUG_ENABLE 1

#ifndef DEBUG_ENABLE
// 统一使用通用的宏去做日志输出
#define debug_printf(format, ...) printf("[%s:%d]" format "\r\n", __FILE__, __LINE__, ##__VA_ARGS__) // 参数format是格式字符串，__VA_ARGS__是可变参数列表，__FILE__和__LINE__是预定义的宏，分别表示当前文件名和行号。
// ##是优化可变参数__VA_ARGS__，当可变参数为空时，去掉前面的逗号

// 关闭日志打印输出
#else
#define debug_printf(...) // 如果DEBUG_ENABLE没有定义，则debug_printf宏将被定义为空，这样在代码中使用debug_printf时就不会产生任何输出。

#endif // DEBUG_ENABLE

#endif // __COM_DEBUG_H
