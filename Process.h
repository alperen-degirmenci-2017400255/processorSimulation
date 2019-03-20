
#include <vector>
#include <iostream>
#include <iterator>
#include <sstream>
#include <fstream>
#include <iterator>

using namespace std;

/*
 * Definition of process class.
 * There are primitive fields and an instructions vector inside.
 */
class Process{
public:
    string id; // Process id, such as P1, P2...

    int priority; // Process priority as integer.

    string codeFile; // Codefile string to hold codefile.txt string.

    int leftInstr;  //last instruction executed.

    int arrTime; // Arrival time of process.

    int processedTime; // Total processed time of instructions of the process.

    int endTime; // Finishing time of process' last instruction.

    vector<int> instructions;

    // Constructor definition.
    Process(string _id, int _priority, string _codeFile, int _arrTime);

    // Copy assignment operator definition.
    Process &operator=(const Process &proc);

    //  Destructor definition.
    ~Process();

};




