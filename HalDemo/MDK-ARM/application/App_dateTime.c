#include "App_dateTime.h"
void App_dateTime_get(Clock_Date_Time_Type *date_time)
{
    // 1.读取年份
    uint8_t year_date = DS1302Z_Read_Byte(DS1302Z_YEAR_REG);
    date_time->year = (year_date >> 4) * 10 + (year_date & 0x0f) + 2000;
    // 2.读取月份
    uint8_t month_date = DS1302Z_Read_Byte(DS1302Z_MONTH_REG);
    date_time->month = (month_date >> 4) * 10 + (month_date & 0x0f);
    // 3.读取日
    uint8_t day_date = DS1302Z_Read_Byte(DS1302Z_DAY_REG);
    date_time->day = (day_date >> 4) * 10 + (day_date & 0x0f);
    // 4.读取星期
    uint8_t week_date = DS1302Z_Read_Byte(DS1302Z_WEEK_REG);
    date_time->week = (week_date & 0x0f);
    // 5.读取小时
    uint8_t hour_date = DS1302Z_Read_Byte(DS1302Z_HOUR_REG);
    if (hour_date & 0x80)
    {
        date_time->is_12_hour = 1; // 12小时制
        if (hour_date & 0x20)
        {
            date_time->is_pm = 1;
        }
        else
            date_time->is_pm = 0;
        if (hour_date & 0x10)
        {
            date_time->hour = 10 + (hour_date & 0x0f);
        }
        else
        {
            date_time->hour = (hour_date & 0x0f);
        }
    }
    else
    {
        date_time->is_12_hour = 0; // 24小时制
        if (hour_date & 0x20)
        {
            date_time->hour = 20 + (hour_date & 0x0f);
        }
        else if (hour_date & 0x10)
        {
            date_time->hour = 10 + (hour_date & 0x0f);
        }
        else
        {
            date_time->hour = (hour_date & 0x0f);
        }
    }
    // 统一使用24h保存时间
    if (date_time->is_12_hour == 1)
    {
        if (date_time->is_pm)
        {
            date_time->hour = date_time->hour + 12;
        }
        date_time->is_12_hour = 0;
    }
    //  6.读取分钟
    uint8_t min_date = DS1302Z_Read_Byte(DS1302Z_MINUTE_REG);
    date_time->min = (min_date >> 4) * 10 + (min_date & 0x0f);
}

void App_dateTime_set(Clock_Date_Time_Type *date_time)
{
    // 将date_time数据写入芯片
    // 打开写保护
    DS1302Z_Write_Byte(DS1302Z_CONTROL_REG, 0x00);

    uint8_t year_shi = (date_time->year % 100) / 10;
    uint8_t year_ge = (date_time->year % 10);
    DS1302Z_Write_Byte(DS1302Z_YEAR_REG, (year_shi << 4) | year_ge);

    uint8_t month_shi = date_time->month / 10;
    uint8_t month_ge = (date_time->month % 10);
    DS1302Z_Write_Byte(DS1302Z_MONTH_REG, (((month_shi & 0x01) << 4) | month_ge));

    uint8_t day_shi = date_time->day / 10;
    uint8_t day_ge = (date_time->day % 10);
    DS1302Z_Write_Byte(DS1302Z_DAY_REG, (((day_shi & 0x03) << 4) | day_ge));

    if (date_time->hour > 12)
    {
        uint8_t hour_shi = (date_time->hour - 12) / 10;
        uint8_t hour_ge = ((date_time->hour - 12) % 10);
        DS1302Z_Write_Byte(DS1302Z_HOUR_REG, (0x80 | (0x20) | (hour_shi << 4) | (hour_ge % 10)));
    }
    else
    {
        uint8_t hour_shi = (date_time->hour) / 10;
        uint8_t hour_ge = ((date_time->hour) % 10);
        DS1302Z_Write_Byte(DS1302Z_HOUR_REG, (0x80 | (hour_shi << 4) | (hour_ge % 10)));
    }

    uint8_t min_shi = date_time->min / 10;
    uint8_t min_ge = (date_time->min % 10);
    DS1302Z_Write_Byte(DS1302Z_MINUTE_REG, (((min_shi & 0x07) << 4) | min_ge));

    // 秒，默认为0
    DS1302Z_Write_Byte(DS1302Z_SECOND_REG, 0x00);
    // 计算星期几的公式（蔡勒公式）
    uint16_t y = date_time->year;
    uint8_t c = y / 100;
    uint8_t m = date_time->month;
    if (m == 1 || m == 2)
    {
        y--;
        m += 12;
    }
    int16_t w = y + y / 4 + c / 4 - 2 * c + 26 * (m + 1) / 10 + date_time->day - 1;
    while (w < 0)
    {
        w += 7;
    }
    w = w % 7;
    if (w == 0)
    {
        w = 7;
    }
    DS1302Z_Write_Byte(DS1302Z_WEEK_REG, w);
}
