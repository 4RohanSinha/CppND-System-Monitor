#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}


// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() { 
	//set default values in case the if statement below is not executed
	float MemTotal = 0.0; //found on first line of file
	float MemFree = 0.0; //found on second line of file
	float MemAvailable = 0.0; //found on third line of file
	float Buffers = 0.0; //found on fourth line of file
	float MemUsage = 0.0; //calculate based on previous values

	string line1, line2, line3, line4; //store the first four lines in /proc/meminfo
	
	string label1, label2, label3, label4; //the labels in the first four lines


	std::ifstream stream(kProcDirectory + kMeminfoFilename); //file stream for /proc/meminfo

	if (stream.is_open()) {
		//Get MemTotal
		std::getline(stream, line1); //get first line
		std::istringstream line1stream(line1); //create linestream for line 1
		line1stream >> label1 >> MemTotal; //store label 1 and MemTotal in the first line

		//same process as MemTotal for next three values
		//
		//Get MemFree
		std::getline(stream, line2);
		std::istringstream line2stream(line2);
		line2stream >> label2 >> MemFree;

		//Get MemAvailable
		std::getline(stream, line3);
		std::istringstream line3stream(line3);
		line3stream >> label3 >> MemAvailable;

		//Get Buffers
		std::getline(stream, line4);
		std::istringstream line4stream(line4);
		line4stream >> label4 >> Buffers;

		//Calculate MemUsage
		MemUsage = 1.0 - (MemFree/(MemTotal - Buffers));
	}
	
	return MemUsage;
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() { 
	//variables to store info from /proc/uptime
	long uptime = 0; //uptime variable

	long otherNum = 0; //otherNum contains the other number from the file
	string line; //the only line in /proc/uptime

	std::ifstream stream(kProcDirectory + kUptimeFilename); //filestream for the file

	if (stream.is_open()) {
		std::getline(stream, line); //get the first (and only) line from filestream
		std::istringstream linestream(line); //create a stringstream
		linestream >> uptime >> otherNum; //get the two numbers from the linestream variable
		return uptime; //return the uptime value
	}
	return uptime; //uptime = 0 if the if statement was not executed, so return 0 (essentially suggesting that there was an error opening the file).
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
	//return sum of active and idle jiffies (this is the total number of jiffies)
	return ActiveJiffies() + IdleJiffies();
}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) {
	string line; //store each line here
        std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename); //file stream for /proc/<PID>/stat

        long uTime = 0; //store uTime of process here
	long sTime = 0; //store sTime of process here
	long cuTime = 0; //store cuTime of process here
	long csTime = 0; //store csTime of process here

        if (stream.is_open()) {
		std::getline(stream, line); //get next line of file
                std::istringstream linestream(line); //create string stream for line
                string buffer; //store each component of the line while iterating through the line

		//iterate until the last value that is needed has been reached
                for (int i = 0; i < 17; i++) {
                        linestream >> buffer; //store current value in buffer

			//only even consider converting to long if each character is a digit
			if (std::all_of(buffer.begin(), buffer.end(), isdigit) && buffer != "") {
				//uTime is at the 13th index, sTime 14th, cuTime 15th, and csTime 16th
				//check if i is one of those indices and store the value accordingly
                        	if (i == 13) uTime = std::stol(buffer);
                        	if (i == 14) sTime = std::stol(buffer);
				if (i == 15) cuTime = std::stol(buffer);
				if (i == 16) csTime = std::stol(buffer);
			}
                }
        }

	//total is the sum of these values
	long total = uTime + sTime + cuTime + csTime;

	//return the total
	return total;
}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
	auto cpuUtilOriginal = CpuUtilization(); //get the cpu util values from the function

	//add only the nonidle components of the CpuUtilization() information to the vector
	//this vector will be added, and the sum will be returned
	//store only the values needed to find the number of active jiffies
	vector<string> cpuUtil = {cpuUtilOriginal[0], cpuUtilOriginal[1], cpuUtilOriginal[2], cpuUtilOriginal[5], cpuUtilOriginal[6], cpuUtilOriginal[7]};

	long sumJiffies = 0; //store the active jiffies here

	//iterate through the cpuUtil vector
	for (auto i = 0u; i < cpuUtil.size(); i++) {
		//each element in the vector is a string so check if it can be converted to a long
		if (std::all_of(cpuUtil[i].begin(), cpuUtil[i].end(), isdigit) && cpuUtil[i] != "") {
			//if the condition above holds true, convert it to a long and add it to the sum
			sumJiffies += std::stol(cpuUtil[i]);	
		}
	}

	//return the sum
	return sumJiffies;
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
	auto cpuUtilOriginal = CpuUtilization(); //get all the values needed for CPU Utilization

	//store only the items needed
	vector<string> cpuUtil = {cpuUtilOriginal[3], cpuUtilOriginal[4]};

	long sumJiffies = 0; //store the idle jiffies here

	//iterate through the cpuUtil vector
	for (auto i = 0u; i < cpuUtil.size(); i++) {
		//each element in vector is a string so check if each one can be converted to a long
		if (std::all_of(cpuUtil[i].begin(), cpuUtil[i].end(), isdigit) && cpuUtil[i] != "") {
			//if the condition above holds true, convert the element to a long and then add it to the sum
			sumJiffies += std::stol(cpuUtil[i]);
		}
	}
	//return the sum of the jiffies
	return sumJiffies;
}

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { 
	vector<string> cpuUtil = {}; //create vector of strings for each value used for CPU Utilization

	string line; //store each line of file here

	string cpuLabel; //cpuLabel - a string value at the beginning not needed for cpu utilization

	//the values needed for CPU Utilization
	string user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;

	// file stream for /proc/stat
	std::ifstream stream(kProcDirectory + kStatFilename);

	if (stream.is_open()) {
		std::getline(stream, line); //get next line
		std::istringstream linestream(line); //create istringstream for current line
		linestream >> cpuLabel >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal >> guest >> guest_nice; //store the components of each strings in these variables

		cpuUtil = {user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice}; //store the values retrieved from the file above in the vector of strings
	}

	return cpuUtil; //return the vector<string>
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
        std::ifstream stream(kProcDirectory + kStatFilename); //file stream for /proc/stat
        string line; //store each line of file here

        string category; //store each category here
        string value; //store the value (the second item in each line) here
	//continue looking through the file as long as EOF has not been reached
        while (!stream.eof()) {
                std::getline(stream, line); //get next line
                std::istringstream linestream(line); //create string stream for current line
                linestream >> category >> value; //push the first two values of the stream to category and value

		//if the current category is processes (where total processes is stored)
                if (category == "processes") {
			//check if the value after processes can be converted to an int
			if (std::all_of(value.begin(), value.end(), isdigit) && value != "") {
				//if so, convert it to an int and return it
                        	return std::stoi(value);
			} else {
				//otherwise return 0
				return 0;
			}
                }
        }
	//return 0
	return 0;
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() { 
        std::ifstream stream(kProcDirectory + kStatFilename); //file stream for /proc/stat

        string line; //this variable stores each line of the file
        string category; //stores each label in the file
        string value; //stores each value (right after the label) in the file

	//continue reading through the file searching for the right label as long as the EOF has not been reached
        while (!stream.eof()) {
                std::getline(stream, line); //get next line
                std::istringstream linestream(line); //open string stream for line
                linestream >> category >> value; //store the first two values of the line
		//check if the category is called procs_running - this stores the number of running processes
                if (category == "procs_running") {
			//ensure that the value after it can be converted to an integer value - if not, there was an error with parsing
			if (std::all_of(value.begin(), value.end(), isdigit) && value != "") {
				//convert the value string to an int and return it
                        	return std::stoi(value);
			} else {
				return 0; //otherwise return 0
			}
                }
        }
	return 0; //return 0 if the opening of the file was unsuccessful
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) {
	std::ifstream stream(kProcDirectory + std::to_string(pid) + kCmdlineFilename); //file stream for /proc/<PID>/cmdline

	string line; //this variable will store each line in the file

	//only access the file if the stream is open and works
	if (stream.is_open()) {
		std::getline(stream, line); //get the first (and only) line

		return line; //return that line (the entire line contains the entire command used to launch the process)
	}

	//if the function has not returned yet, it has not successfully opened the stream
	//therefore just return string()
	return string();
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) {
	//create file stream for /proc/<PID>/status
	std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);

	string line; //store each line of file here
	string category; //store each category (include colon) here
	string value; //store each value after colon

	if (stream.is_open()) {
		while (!stream.eof() && category != "VmSize:") { //continue looping as long as EOF or VmSize has not been reached
			std::getline(stream, line); //get next line
			std::istringstream linestream(line); //create istringstream for line
			linestream >> category >> value; //store first two values in line (category & value)
		}
		return value; //return value found
	}
	return string(); //if the file could not be opened, return string()
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) { 
	//create file stream for /proc/<PID>/status
	std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);
	string line; //store each line of the file in this variable
	string category; //store the category (including the colon) in this variable
	string value; //store the value after the colon here

	if (stream.is_open()) {
		while (!stream.eof() && category != "Uid:") { //continue iterating through the while loop while the EOF has not been reached and the Uid category has not been found
			std::getline(stream, line); //get next line
			std::istringstream linestream(line); //create string stream for current line
			linestream >> category >> value; //move first two items in the linestream into category and value
		}
		//return the value
		return value;
	}

	//return an empty string if the stream could not be opened
	return string();
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) {
	string uid = Uid(pid); //get the UID from the LinuxParser::Uid(int pid) metho
	
	string line; //variable to store each line of the file
	
	std::ifstream stream(kPasswordPath); //open file stream for the file /etc/passwd, which contains the info for this method
	
	string user; //variable to store each user listed on each line
	
	string possibleUid; //variable to store the UID listed before each user - the uid variable must match with one of the values this variable takes on to return a value other than string()

	if (stream.is_open()) {
		while (!stream.eof()) { //continue reading the file as long as EOF has not been reached yet
			
			std::getline(stream, line); //get the next line of the file

			//replace a few parts of the string for string parsing
			std::replace(line.begin(), line.end(), ':', ' ');
			std::replace(line.begin(), line.end(), 'x', ' ');
			
			//set up variable for string parsing (an istringstream).
			//the istringstream will parse the line
			std::istringstream linestream(line);

			//place the first two components of the line (which is being streamed with the linestream) in user and then possibleUid
			linestream >> user >> possibleUid;

			//if the possibleUid matches with the UID for the process, break out of the loop and return the user it was in the file with
			if (possibleUid == uid) return user;
		}
	}

	//if the stream was not open or the function was unable to match the correct uid, just return empty string().
	return string();
}


// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function

//this function returns the start time of a process
//Process::UpTime() returns the actual uptime of a process by subtracting the start time of the process from the system uptime
long LinuxParser::UpTime(int pid) { 
	string line; //when streaming the file, this variable will store each line
	string buffer; //this variable will be used to store each part of the istringstream that will be created for the line variable

	long time = 0; //this variable will store the final time that the function finds

	std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename); //this stream variable will store the file stream for the /proc/<PID>/stat, where the uptime is found

	//only look through the file if the stream is working
	if (stream.is_open()) {

		std::getline(stream, line); //get next line in file
		
		std::istringstream linestream(line); //create stringstream for line
		
		//the uptime is at the 21st index of the file
		//keep iterating through the loop to get to it
		//break out of the loop once the loop has gotten to the 21st value
		for (int i = 0; i < 22; i++) {
			linestream >> buffer;
		}

		//if the value can be converted to an int (if not, there was a problem with parsing), convert it to a long and store this value in time
		if (std::all_of(buffer.begin(), buffer.end(), isdigit) && buffer != "") {
		
			time = std::stol(buffer); //converting string buffer to a long
		
		}
		//convert to seconds and return
		return time/sysconf(_SC_CLK_TCK);
	}

	//if the file couldn't be opened, return 0
	return 0; 
}
