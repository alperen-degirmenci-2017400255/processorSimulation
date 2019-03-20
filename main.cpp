#include <iostream>
#include <iterator>
#include <sstream>
#include <fstream>
#include <vector>
#include <queue>
#include "Process.h"
using namespace std;


/* This function is a helper function that splits a line which is read.
 It splits the line with respect to the space symbols and puts the elements in given order
 to the container "cont". I used a string vector as container.
 I used it while reading definition.txt file and code*.txt files.*/

template <class Container>
void splitLine(const string& str, Container& cont)
{
    istringstream iss(str);
    copy(istream_iterator<string>(iss),
         istream_iterator<string>(),
         back_inserter(cont));
}


/*  This one is an operator overloader for the priority queue of Process class.
 *  It helps Process objects to be ordered in given priority queue according to
 *  their priorities.
 */
struct compByPriority{
    bool operator()(const Process* a, Process* b){
        if(a->priority == b->priority){
            return (a->arrTime > b->arrTime);
        }
        else{
            return (a->priority > b->priority);
        }
    }
};

/*  This one is also an operator overloader for the priority queue of Process class.
 *  It helps Process objects to be ordered in given priority queue according to
 *  their arrival times to the CPU. Early arrivals are prioritized.
 */
struct compByArrival{
    bool operator()(const Process* a, Process* b){
        if(a->arrTime == b->arrTime){

            return (a->priority > b->priority);
        }
        else{
            return (a->arrTime > b->arrTime);
        }
    }
};


/*  This is the helper function for output formatting.
 *  The function takes the current ready queue and the timer as parameters,
 *  and returns the formatted string according to the parameters.
 */

string outFormat(priority_queue <Process*, vector<Process*>, compByPriority > _readyQ, int _timer){

    string line = to_string(_timer) + ":HEAD";
    int i = 1;
    while( !(_readyQ.empty()) ){
        if(i == 1) {
            line = line + "-" + _readyQ.top()->id + "[" + to_string(_readyQ.top()->leftInstr + 1) + "]-";
        }
        else{
            line = line + _readyQ.top()->id + "[" + to_string(_readyQ.top()->leftInstr + 1) + "]-";
        }
        i++;
        _readyQ.pop();
        }
    line = line + "TAIL\n";
    return line;

}

// main function.
int main(int argc, char* argv[]){

    ifstream inputFile(argv[1]); //input file as the first parameter.

    // output file as second parameter.
    ofstream fp;
    fp.open(argv[2]);

    string lineRead; // string used to take lines.

    // Process vector which stores Processes read from definition file.
    vector<Process> processList;

    /* Priority queues which store process pointers according to their arrival times.
     *  Early arrivals are prioritized.
     *  Output queue is used for storing processes for output file.
    */
    priority_queue <Process*, vector<Process*>, compByArrival> pq2;
    priority_queue <Process*, vector<Process*>, compByArrival> outputQueue;

    /*
     * Main Ready Queue which stores process pointers according to their priorities.
     * This one is used in execution part and it holds processes while they are executed.
    */
    priority_queue <Process*, vector<Process*>, compByPriority> readyQ;



    /* Reading process definitions from definiton.txt file and
     * storing them as Process objects. Fields such that process id, priority,
     * code file and arrival times are also defined here.
     * These objects are then pushed to Process vector named "processList".     *
     */

    while(getline(inputFile,lineRead)){
        vector<string> lineList;
        splitLine(lineRead, lineList);
        Process newProcess(lineList[0], stoi(lineList[1]),
                           lineList[2], stoi(lineList[3]));
        processList.push_back(newProcess);
    }


    /* Instructions of processes are read from code*.txt files and
     * pushed every instruction's time-length as integer to the
     * "instructions" vector of Process objects.
     * Then these Process objects are ready to use and they are pushed to the
     * priority queue storing them according to their arrival times.
     */
    for(int i=0; i<processList.size(); i++){

        string path = processList[i].codeFile + ".txt";
        ifstream codeFile(path);
        while(getline(codeFile,lineRead)){
            vector<string> lineList;
            splitLine(lineRead, lineList);
            processList[i].instructions.push_back(stoi(lineList[1]));
        }

        pq2.push(&processList[i]);
    }


    /* !! Process pointer "current" is used throughout the execution and represents the
     * current process executing. !!
     * It is popped from arrival time queue and pushed to Ready Queue.
     * Current process stays in Ready Queue until it is terminated.
     *
     */
    Process* current = pq2.top();
    pq2.pop();
    string outLine; // String used to format the output lines.
    readyQ.push(current);

    /*
     * "Timer" integer is used to keep track of current time. It is also used to record finishing time and starting time
     * of the processes.
     */
        int timer = 0;
        timer = current->arrTime;

    // This one integer decides if there are multiple processes arrive at the same time
    // and is used to keep track of them.
    int arbitrary = 1;


    /* -- Main loop which executes Processes' instructions and keep track of the CPU. --
     * Since the processes are held in Ready Queue, the loop is on while it is not empty.
     */
      while(  !(readyQ.empty()) ){

          /* One of control blocks in the loop. It keeps track if a process
           * or multiple processes arrive and decides if the current process
           * will be pre-empted or not. Actually, it pushes all processes arriving
           * and picks a new "current" process from top of Ready Queue.           *
           */
          if(! (pq2.empty()) ){
              if(timer >= pq2.top()->arrTime){
                  while(timer >= pq2.top()->arrTime && !pq2.empty() ) {

                      readyQ.pop();
                      readyQ.push(current);

                      /* New processes are pulled from "arrival priority queue" and
                       * pushed to ready queue here.
                       */
                      Process *temp = pq2.top();
                      pq2.pop();
                      readyQ.push(temp);

                      // Current process is picked again from top of ready queue.
                      current = readyQ.top();
                  }

                  /* Since there are changes in the ready queue,
                   * ready queue and timer is sent to output formatting function.
                   */
                  outLine = outFormat(readyQ,timer);
                  fp << outLine;
                  arbitrary++;
              }

          }

            /* This part decides the first few lines of the output format.
             * It decides between if any process-processes arrive at 0'th second
             * or not.
             */
          if( arbitrary == 1){
              if(timer == 0){
                  outLine = outFormat(readyQ,timer);
                  fp << outLine;
                  arbitrary++;
              }
              else{
                  fp << "0:HEAD--TAIL\n";
                  outLine = outFormat(readyQ,timer);
                  fp << outLine;
                  arbitrary++;
              }
          }


          // Increasing the timer by every atomic instruction executed.
          timer += current->instructions[current->leftInstr];

          // Calculating total process time of current process in every step.
          current->processedTime += current->instructions[current->leftInstr];

          //Incrementing the counter of current process's last instruction executed.
          current->leftInstr++;


          /* The other big control block which keeps track of changes when all instructions
           * of a process is done. There are a few actions to take according to the situations of
           * Ready Queue and Arrival Queue. They are listed and commented below.
           */
          if(  current->leftInstr  == current->instructions.size() ){

              current->endTime = timer; // Current process's finishing time is recorded.

              /* Finished process is pushed to the output queue.
               * Output queue is then used to print the
               * Waiting times and Turnaround times.
               */
              outputQueue.push(current);

              readyQ.pop(); // Finished process is popped off from Ready Queue.

              // Control block which checks if Ready Queue is empty.
              if(readyQ.empty()){
                  if(pq2.empty()){

                      /*
                       * If both Ready queue and Arrival queue are empty,
                       * then last time is printed and program breaks from loop.
                       */
                      fp << to_string(timer) + ":HEAD--TAIL\n";
                      break;
                  }

                  else{

                      /* If ready queue is empty but there are still processes in the Arrival Queue,
                       * they are popped off from Arrival Queue and pushed to Ready queue if their arrival time
                       * has come.
                       */
                      current = pq2.top();
                      pq2.pop();
                      readyQ.push(current);

                      if(timer < current->arrTime){

                          /* If a process finished at a certain time and no other process has arrived at that time,
                           * empty ready queue is printed.
                           */
                          fp << to_string(timer) + ":HEAD--TAIL\n";
                      }

                      // Timer is set to next process's arrival time.
                      timer = current->arrTime;

                      /* Popping processes from arrival queue and pushing them to
                       * ready queue.
                       */
                      while(timer >= pq2.top()->arrTime && !pq2.empty() ){

                          Process *temp = pq2.top();
                          pq2.pop();

                          readyQ.push(temp);
                          current = readyQ.top();

                      }

                      /* Since there are changes in the ready queue,
                       * ready queue and timer is sent to output formatting function.
                       */
                      timer = current->arrTime;
                      outLine = outFormat(readyQ,timer);
                      fp << outLine;
                      continue;
                  }
              }

              /* Current process is finished and there are still processes
               * in the ready queue. If there are more processes to come from arrival queue,
               * they are popped from there and pushed to the Ready Queue, new current process
               * is picked and loop continues.
               */
              else{
                  while(timer >= pq2.top()->arrTime && !pq2.empty() ) {

                      Process *temp = pq2.top();
                      pq2.pop();
                      readyQ.push(temp);

                  }
                  current = readyQ.top();

                  /* Since there are changes in the ready queue,
                   * ready queue and timer is sent to output formatting function.
                   */
                  outLine = outFormat(readyQ,timer);
                  fp << outLine;
                  continue;
              }
          }

      }
    /*
     * Lines to print the turnaround time and waiting time of processes.
     * Turnaround time is calculated as "Finishing time" - "Arrival Time".
     * Waiting time is calculated as "Turnaround time" - "total processed time".
     */
    fp << "\n";
    while(!outputQueue.empty()){
        Process* tmp = outputQueue.top();
        outputQueue.pop();
        fp << "Turnaround time for " + tmp->id + " = " + to_string((tmp->endTime-tmp->arrTime)) + " ms\n";
        fp << "Waiting time for " + tmp->id + " = " + to_string((tmp->endTime-tmp->arrTime) - tmp->processedTime) + "\n";
    }

    fp.close();

    return 0;

}