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

#ifndef OS_PROJECTS_COMPUTER_H_
#define OS_PROJECTS_COMPUTER_H_

#include <cstdlib>
#include <string>

namespace OSProjects {

class Computer {
 public:
  Computer(): num_ram_memory_{0}, num_page_{0},
	   num_hard_disks_{-1} { }

  Computer(const size_t ram, const size_t page, const int hdd);
  ~Computer();

  size_t num_ram() const { return num_ram_memory_; }
  size_t num_page() const { return num_page_; }
  int num_hard_disks() const { return num_hard_disks_; }

 private:
  size_t num_ram_memory_;
  size_t num_page_;
  int num_hard_disks_;
};

class Process
{
 public:
  Process(): pid_{0}, priority_{0}, page_{0} { }
  Process(const int pid, const int priority, const int page);
  ~Process();

  int get_pid() const { return pid_; }
  int get_priority() const { return priority_; }
  int get_page() const { return page_; }

 private:
  int pid_;
  int priority_;
  int page_;
};

class HardDiskProcess
{
 public:
  HardDiskProcess();
  HardDiskProcess(const Process a_process, const std::string a_file);
  ~HardDiskProcess();

  Process get_process() const { return process_; }
  std::string get_file() const { return file_; }

 private:
  Process process_;
  std::string file_;

};

}  // namespace OSProjects

#endif  // OS_PROJECTS_COMPUTER_H_
