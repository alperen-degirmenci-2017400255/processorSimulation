
#include "Process.h"

Process::Process(string _id, int _priority, string _codeFile, int _arrTime){

    /*
     * Constructor implementation.
     */

    id = _id;
    priority = _priority;
    codeFile = _codeFile;
    arrTime = _arrTime;
    endTime = 0;
    processedTime = 0;
    leftInstr = 0;

}


/*
 *  Copy assignment operator implementation.
 */

Process& Process:: operator=(const Process &proc){
    this->id = proc.id;
    this->priority = proc.priority;
    this->codeFile = proc.codeFile;
    this->arrTime = proc.arrTime;
    this->endTime = proc.endTime;
    this->processedTime = proc.processedTime;
    this->leftInstr = proc.leftInstr;

}

// Destructor.
Process::~Process(){

}

