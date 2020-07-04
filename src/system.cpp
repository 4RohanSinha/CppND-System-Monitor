#include <unistd.h>
#include <cstddef>
#include <set>
#include <string>
#include <vector>
#include <iostream>

#include "process.h"
#include "processor.h"
#include "system.h"
#include "linux_parser.h"

using std::cout;
using std::set;
using std::size_t;
using std::string;
using std::vector;
//

//constructor
//set the os_ and kernel_ private members
//do not need to be updated
System::System() {
	os_ = LinuxParser::OperatingSystem();
	kernel_ = LinuxParser::Kernel();
}

// TODO: Return the system's CPU
Processor& System::Cpu() { return cpu_; }

// TODO: Return a container composed of the system's processes
vector<Process>& System::Processes() {
	processes_.clear(); //clear the processes list so that this method can update them (if a process ended for example)

	std::vector<int> pidsInts = LinuxParser::Pids(); //get the new PIDs
	
	//iterate through these PIDs
	for (int i: pidsInts) {
		Process curProc(i); //create new process for each PID
		processes_.emplace_back(curProc); //move them to the processes_ vector
	}

	//sort the processes
	//by default it uses the < operator
	//this operator has been overloaded in the Process class
	std::sort(processes_.begin(), processes_.end());

	//return the new vector of processes
	return processes_;
}

// TODO: Return the system's kernel identifier (string)
std::string System::Kernel() { 
	return kernel_; //a private member - data found in LinuxParser::Kernel()
}

// TODO: Return the system's memory utilization
float System::MemoryUtilization() { 
	return LinuxParser::MemoryUtilization();
}

// TODO: Return the operating system name
std::string System::OperatingSystem() { 
	return os_; //a private member (data found in LinuxParser::OperatingSystem())
}

// TODO: Return the number of processes actively running on the system
int System::RunningProcesses() { 
	return LinuxParser::RunningProcesses();
}

// TODO: Return the total number of processes on the system
int System::TotalProcesses() {
	return LinuxParser::TotalProcesses();
}

// TODO: Return the number of seconds since the system started running
long System::UpTime() { 
	return LinuxParser::UpTime();
}
