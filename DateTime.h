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

    bool operator<(const DateTime &other) const
    {
        if (year != other.year)
            return year < other.year;
        if (month != other.month)
            return month < other.month;
        if (day != other.day)
            return day < other.day;
        if (hour != other.hour)
            return hour < other.hour;
        return minute < other.minute;
    }
    bool operator>(const DateTime &other) const
    {
        return other < *this;
    }
    bool operator<=(const DateTime &other) const
    {
        return !(other < *this);
    }

    bool operator>=(const DateTime &other) const
    {
        return !(*this < other);
    }
};