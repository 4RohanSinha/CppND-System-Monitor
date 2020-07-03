#include "processor.h"
#include "linux_parser.h"
#include <string>
#include <sstream>
#include <fstream>
#include <unistd.h>

using std::vector;
using std::string;

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() {
	float idle = LinuxParser::IdleJiffies(); //get idle jiffies
	float total = LinuxParser::Jiffies(); //get total jiffies

	//prevTotal_ and prevIdle_ are private members of the Processor class

	float totald = total - prevTotal_; //calculate change in total
	float idled = idle - prevIdle_; //calculate change in idle

	prevTotal_ = total; //assign previous total to current total
	prevIdle_ = idle; //assign previous idle to current idle

	return (totald - idled)/totald; //return the system CPU utilization

}
