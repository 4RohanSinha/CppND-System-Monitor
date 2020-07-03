#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>
#include <cmath>
#include "process.h"
#include "linux_parser.h"

using std::cout;
using std::string;
using std::to_string;
using std::vector;

// TODO: Return this process's ID
int Process::Pid() { return pid_; }

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() { 
	long total = LinuxParser::ActiveJiffies(pid_); //get total jiffies for PID from LinuxParser

	float seconds = UpTime(); //get uptime of process
	
	cpu_util_ = (float) ((1.0 * total)/sysconf(_SC_CLK_TCK)/seconds); //calculate CPU util based on the previous values
	//cpu_util_ is a private member variable for Process

	return cpu_util_; //return cpu_util_
}

// TODO: Return the command that generated this process
string Process::Command() { 
	return LinuxParser::Command(pid_); //use LinuxParser::Command(pid) function to get command for given PID
}

// TODO: Return this process's memory utilization
string Process::Ram() {
	
	int data = 0; //data stores the process's memory utilization
	
	string ramData = LinuxParser::Ram(pid_); //get the RAM data from LinuxParser::Ram(pid), which returns a string

	//ensure that the RAM data from LinuxParser consists of digits only and therefore can be converted to an int
	//also make sure string is not empty
	
	if (std::all_of(ramData.begin(), ramData.end(), isdigit) && ramData != "") {
		//convert ramData to an int
		data = std::stoi(ramData);
	}
	
	float dataMbs = (data*0.001); //convert to megabytes


	//convert dataMbs to an int to decrease the length of the resulting string
	//convert the string to an int with std::to_string
	
	string dataStr = std::to_string((int) dataMbs);

	return dataStr; //return string value
}

// TODO: Return the user (name) that generated this process
string Process::User() { return LinuxParser::User(pid_); }

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() {
	//LinuxParser::UpTime(pid) returns the start time of the process
	//subtract it from the system uptime to get process uptime
	long uptime = LinuxParser::UpTime() - LinuxParser::UpTime(pid_);

	return uptime; //return this value
}

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
//bool Process::operator<(Process const& a) const {
bool Process::operator<(const Process &a) const {
	//used for sorting processes in the System class
	return a.cpu_util_ < cpu_util_; //allows for the System class to sort in descending order
}
