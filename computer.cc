/******************************************************************************
 * Title          : computer.cc
 * Author         : Renat Khalikov
 * Created on     : December 11, 2017
 * Description    : Classes for representing a computer, process and a
 *                  process that is using hard drive
 * Purpose        : simulates some aspects of operating systems
 * Usage          :
 * Build with     :
 */

#include "computer.h"
#include <cstdio>
#include <cstdlib>

using namespace std;

namespace OSProjects {

Computer::Computer(const size_t ram, const size_t page, const int hdd){
  num_ram_memory_ = ram;
  num_page_ = page;
  num_hard_disks_ = hdd;
}

Computer::~Computer(){

}

Process::Process(const int pid, const int priority, const int page){
  pid_ = pid;
  priority_ = priority;
  page_ = page;
}

Process::~Process(){}

HardDiskProcess::HardDiskProcess(const Process a_process, const std::string a_file){
  process_ = a_process;
  file_ = a_file;
}

HardDiskProcess::~HardDiskProcess(){}

}  // namespace OSProjects
