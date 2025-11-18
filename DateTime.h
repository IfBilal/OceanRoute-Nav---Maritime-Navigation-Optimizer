#pragma once
#include <iostream>
struct DateTime
{
    int year;
    int month;
    int day;
    int hour;
    int minute;

    DateTime(int year, int month, int day, int hour, int minute) : year(year), month(month), day(day), hour(hour), minute(minute) {}
};
