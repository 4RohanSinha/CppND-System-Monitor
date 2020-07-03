#include <string>

#include "format.h"

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) { 
	//get hours, minutes, seconds
	//use (int) to ensure that ints get stored in these variables
	int hoursInt = (int) (seconds/3600);
	int minutesInt = (int) (seconds/60) % 60;
	int secondsInt = (int) seconds % 60;

	//convert those ints to strings
	string hourString = std::to_string(hoursInt);
	string minuteString = std::to_string(minutesInt);
	string secondString = std::to_string(secondsInt);
	
	//add 0s before the string if the value is less than ten
	if (minutesInt < 10) {
		minuteString = "0" + minuteString;
	}
	if (secondsInt < 10) {
		secondString = "0" + secondString;
	}
	if (hoursInt < 10) {
		hourString = "0" + hourString;
	}

	//combine the three strings to create the final output
	string finalOutput =  hourString + ":" + minuteString + ":" + secondString;

	//return the final output
	return finalOutput;
}
