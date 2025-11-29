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
    bool operator==(const DateTime &other) const
    {
        return year == other.year && month == other.month && day == other.day && hour == other.hour && minute == other.minute;
    }
    double timeDiff(const DateTime &other) const
    {
        int totalMinutes1 = ((((year - 2000) * 12 + month) * 31 + day) * 24 + hour) * 60 + minute;
        int totalMinutes2 = ((((other.year - 2000) * 12 + other.month) * 31 + other.day) * 24 + other.hour) * 60 + other.minute;
        return (totalMinutes2 - totalMinutes1) / 60.0;
    }

    // Add hours to current DateTime, handling day/month/year rollover
    DateTime addHours(double hours) const
    {
        DateTime result = *this;
        
        // Add the fractional hours directly
        double totalHours = result.hour + hours;
        double totalMinutes = result.minute + (totalHours - static_cast<int>(totalHours)) * 60;
        
        // Extract whole hours and handle minutes
        result.hour = static_cast<int>(totalHours);
        result.minute = static_cast<int>(totalMinutes);
        
        // Handle minute overflow
        if (result.minute >= 60)
        {
            result.minute -= 60;
            result.hour++;
        }
        
        // Handle day rollover
        while (result.hour >= 24)
        {
            result.hour -= 24;
            result.day++;
            
            // Handle month rollover
            if (result.day > 31)
            {
                result.day = 1;
                result.month++;
                
                // Handle year rollover
                if (result.month > 12)
                {
                    result.month = 1;
                    result.year++;
                }
            }
        }
        
        return result;
    }
};