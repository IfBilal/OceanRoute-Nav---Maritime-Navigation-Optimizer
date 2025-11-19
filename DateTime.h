#pragma once
#include <iostream>
struct DateTime
{
    int year;
    int month;
    int day;
    int hour;
    int minute;

    DateTime(int year = 0, int month = 0, int day = 0, int hour = 0, int minute = 0) : year(year), month(month), day(day), hour(hour), minute(minute) {}
};
