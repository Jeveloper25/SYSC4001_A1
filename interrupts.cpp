/**
 *
 * @file interrupts.cpp
 * @author Sasisekhar Govind
 *
 */

#include"interrupts.hpp"
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
	std::string type;
	uint line_number = 1;
    /******************************************************************/

    //parse each line of the input trace file
    while(std::getline(input_file, trace)) {
        auto [activity, duration_intr] = parse_trace(trace);

        /******************ADD YOUR SIMULATION CODE HERE*************************/
	if (activity.compare("CPU") == 0) {
		type = "Processing instructions\n";	
		execution += std::to_string(sys_time) + ", " + std::to_string(duration_intr) + ", " + type;
		sys_time += duration_intr;
	} else if (activity.compare("SYSCALL") == 0){
		auto p = intr_boilerplate(sys_time, duration_intr, CONTEXT_TIME, vectors); 
		execution += std::get<0>(p);
		sys_time = std::get<1>(p);
	} else if (activity.compare("END_IO") == 0) {
		type = "end of I/O " + std::to_string(duration_intr) + ": interrupt\n";	
		execution += std::to_string(sys_time) + ", " + std::to_string(delays[duration_intr]) + ", " + type;
		sys_time += delays[duration_intr];
	} else {
		std::cout << "Invalid activity: '" << activity << "' on line " << std::to_string(line_number)+ " of input program." << std::endl;
	}
	line_number++;
        /************************************************************************/

    }

    input_file.close();

    write_output(execution);

    return 0;
}
