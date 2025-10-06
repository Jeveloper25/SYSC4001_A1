/**
 *
 * @file interrupts.cpp
 * @author Sasisekhar Govind
 *
 */

#include"interrupts.hpp"
#include <algorithm>
#include <cmath>
#include <iterator>
#include <string>

int main(int argc, char** argv) {

    //vectors is a C++ std::vector of strings that contain the address of the ISR
    //delays  is a C++ std::vector of ints that contain the delays of each device
    //the index of these elemens is the device number, starting from 0
    auto [vectors, delays] = parse_args(argc, argv);
    std::ifstream input_file(argv[1]);

    std::string trace;      //!< string to store single line of trace file
    std::string execution;  //!< string to accumulate the execution output

    /******************ADD YOUR VARIABLES HERE*************************/

	int sys_time = 0;
	int isr_time = 40;
	int save_restore_context_time = 10;
	int set_step_value = 1;
	/*set_step_value is used for mode switching, memory start calculation,
	getting ISR address, and IRET execution*/
	std::string type;
	unsigned int line_number = 1;
	
    /******************************************************************/

    //parse each line of the input trace file
    while(std::getline(input_file, trace)) {
        auto [activity, duration_intr] = parse_trace(trace);

        /******************ADD YOUR SIMULATION CODE HERE*************************/
	if (activity.compare("CPU") == 0) {
		type = "CPU Burst\n";	
		execution += std::to_string(sys_time) + ", " + std::to_string(duration_intr) + ", " + type;
		sys_time += duration_intr;
	} else if (activity.compare("SYSCALL") == 0){
		if (duration_intr >= std::min(delays.size(), vectors.size()) || duration_intr < 0) {
				std::cout << "Line "<< line_number << "\nInvalid device number: " << duration_intr
				<< "\nDevice number must be between 0 and " << std::min(delays.size(), vectors.size()) << std::endl;
		}
		auto p = intr_boilerplate(sys_time, duration_intr, CONTEXT_TIME, vectors); 
		execution += std::get<0>(p);
		sys_time = std::get<1>(p);
		int isr_duration = delays.at(duration_intr);
		int isr_times[4];
		isr_times[0] = std::round(isr_duration * 0.1);
		isr_times[1] = std::round(isr_duration * 0.2);
		isr_times[2] = std::round(isr_duration * 0.4);
		isr_times[3] = std::round(isr_duration * 0.3);
		execution += std::to_string(sys_time) + ", " + std::to_string(isr_times[0]) + ", " + "Check device status\n";
		sys_time += isr_times[0];
		execution += std::to_string(sys_time) + ", " + std::to_string(isr_times[1]) + ", " + "Call device driver\n";
		sys_time += isr_times[1];
		execution += std::to_string(sys_time) + ", " + std::to_string(isr_times[2]) + ", " + "Initiate DMA transfer\n";
		sys_time += isr_times[2];
		execution += std::to_string(sys_time) + ", " + std::to_string(isr_times[3]) + ", " + "Mark device as waiting\n";
		sys_time += isr_times[3];
		execution += std::to_string(sys_time) + ", " + std::to_string(1) + ", " + "IRET\n";
		sys_time++;
	} else if (activity.compare("END_IO") == 0) {
		auto p = intr_boilerplate(sys_time, duration_intr, CONTEXT_TIME, vectors); 
		execution += std::get<0>(p);
		sys_time = std::get<1>(p);
		execution += "N/A, N/A, Placeholder\n";
	} else {
		std::cout << "Line: " << line_number << "\nInvalid activity: " << activity
		<< "\nActivity must be one of 'CPU', 'SYSCALL', or 'END_IO." << std::endl;
	}
	line_number++;
        /************************************************************************/

    }

    input_file.close();

    write_output(execution);

    return 0;
}
