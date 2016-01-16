#define _X86_
#include <synchapi.h>
#undef max

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include "srt_delay.h"

using std::cerr;
using std::endl;
using std::string;
using std::ifstream;
using std::ofstream;

//small ugly convenient macro
#define CERROR cerr << __FUNCTION__ << ": "

/*
srt file format :
00:42:30,748 --> 00:42:31,908
*/
static const string TIME_SEPARATOR = " --> ";
static const string SSCANF_S_TIME_STRING_FORMAT = "%d:%d:%d,%d";
static const string SNPRINTF_TIME_STRING_FORMAT = "%.2d:%.2d:%.2d,%.3d";

bool add_delay(string& line, const long long& delay);

bool split_times(const string& line, string& begin_time_string, string& end_time_string);
bool delay_time(string& time_string, const long long& delay);
bool combine_times(string& line, const string& begin_time_string, const string& end_time_string);

bool parse_time(const string& time_string, int& hours, int& mins, int& secs, int& ms);
bool move_time(long long how_much, int& hours, int& mins, int& secs, int& ms);
bool print_time(string& time_string, const int& hours, const int& mins, const int& secs, const int& ms);

bool srt_delay(const std::string & in_filename, const std::string & out_filename, const long long & delay_ms)
{
	ifstream in(in_filename);
	if (!in.is_open())
	{
		CERROR << "Could not open file " << in_filename << endl;
		return false;
	}

	ofstream out(out_filename);
	if (!out.is_open())
	{
		CERROR << "Could not open file " << out_filename << endl;
		return false;
	}

	string line;
	while (std::getline(in, line))
	{
		if (line.find(TIME_SEPARATOR) != string::npos)
		{
			if (!add_delay(line, delay_ms)) return false;
		}
		out << line << endl;
	}
	return true;
}

bool add_delay(string& line, const long long& delay)
{
	string begin_time_string, end_time_string;
	if (!split_times(line, begin_time_string, end_time_string)) return false;
	if (!delay_time(begin_time_string, delay)) return false;
	if (!delay_time(end_time_string, delay)) return false;
	if (!combine_times(line, begin_time_string, end_time_string)) return false;
	return true;
}

bool split_times(const string& line, string& begin_time_string, string& end_time_string)
{
	size_t time_separator_position = line.find(TIME_SEPARATOR);
	begin_time_string = line.substr(0, time_separator_position);
	end_time_string = line.substr(time_separator_position + TIME_SEPARATOR.size(), string::npos);
	return true;
}

bool delay_time(string& time_string, const long long& delay)
{
	int hh = -1, mm = -1, ss = -1, milli = -1;
	if (!parse_time(time_string, hh, mm, ss, milli)) return false;
	if (!move_time(delay, hh, mm, ss, milli)) return false;
	if (!print_time(time_string, hh, mm, ss, milli)) return false;
	return true;
}

bool combine_times(string& line, const string& begin_time_string, const string& end_time_string)
{
	line = begin_time_string + TIME_SEPARATOR + end_time_string;
	return true;
}

bool parse_time(const string& time_string, int& hours, int& mins, int& secs, int& ms)
{
	hours = -1;
	mins = -1;
	secs = -1;
	ms = -1;
	sscanf_s(time_string.c_str(), SSCANF_S_TIME_STRING_FORMAT.c_str(), &hours, &mins, &secs, &ms);
	if (hours == -1 || mins == -1 || secs == -1 || ms == -1)
	{
		CERROR << "Invalid time passed " << time_string << endl;
		return false;
	}
	return true;
}

bool move_time(long long how_much, int& hours, int& mins, int& secs, int& ms)
{
	const int ms_in_one_sec = 1000;
	const int ms_in_one_min = 60 * ms_in_one_sec;
	const int ms_in_one_hour = 60 * ms_in_one_min;
	if (abs(how_much) >= ms_in_one_hour)
	{
		hours += static_cast<int>(how_much / ms_in_one_hour);
		how_much %= ms_in_one_hour;
	}
	if (abs(how_much) >= ms_in_one_min)
	{
		mins += static_cast<int>(how_much / ms_in_one_min);
		while (mins < 0 && hours > 0)
		{
			hours--;
			mins += 60;
		}
		while (mins >= 60)
		{
			hours++;
			mins -= 60;
		}
		how_much %= ms_in_one_min;
	}
	if (abs(how_much) >= ms_in_one_sec)
	{
		secs += static_cast<int>(how_much / ms_in_one_sec);
		while (secs < 0 && (mins > 0 || hours > 0))
		{
			if (mins == 0)
			{
				hours--;
				mins += 60;
			}
			mins--;
			secs += 60;
		}
		while (secs >= 60)
		{
			mins++;
			if (mins == 60)
			{
				hours++;
				mins = 0;
			}
			secs -= 60;
		}
		how_much %= ms_in_one_sec;
	}
	if (abs(how_much) >= 1)
	{
		ms += static_cast<int>(how_much);
		while (ms < 0 && (secs > 0 || mins > 0 || hours > 0))
		{
			if (secs == 0)
			{
				if (mins == 0)
				{
					hours--;
					mins += 60;
				}
				mins--;
				secs += 60;
			}
			secs--;
			ms += 1000;
		}
		while (ms > 1000)
		{
			secs++;
			if (secs == 60)
			{
				mins++;
				if (mins == 60)
				{
					hours++;
					mins = 0;
				}
				secs = 0;
			}
			ms -= 1000;
		}
		how_much = 0;
	}
	return true;
}

bool print_time(string& time_string, const int& hours, const int& mins, const int& secs, const int& ms)
{
	const int expected_buffer_size = 12;
	int buffer_size = snprintf(nullptr, 0, SNPRINTF_TIME_STRING_FORMAT.c_str(), hours, mins, secs, ms);
	if (buffer_size != expected_buffer_size)
	{
		CERROR << "Buffer size is not " << expected_buffer_size << " but " << buffer_size;
		return false;
	}
	char buffer[16];
	snprintf(buffer, 16, SNPRINTF_TIME_STRING_FORMAT.c_str(), hours, mins, secs, ms);
	time_string = buffer;
	return true;
}
