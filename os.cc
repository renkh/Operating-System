/******************************************************************************
 * Title          : os.cc
 * Author         : Renat Khalikov
 * Created on     : December 11, 2017
 * Description    : Program constantly listens for the user inputs.
 *                  The user inputs signal some system events.
 *                  Program simulates corresponding system behavior.
 * Purpose        : simulates some aspects of operating systems
 * Usage          : ./os
 * Build with     : make all
 */
#include "computer.h"
#include <cstdio>
#include <iostream>
#include <sstream>
#include <string>
#include <queue>
#include <map>

using namespace std;
using namespace OSProjects;

/**
 * ListProcessCurrentlyUsingHardDisk() lists all hard drives, process currently
 * using it, and the I/O queue of processes ready to use the hard disk. Here
 * I'm implying that the process at the head of I/O queue is "using" the hard
 * disk
 *
 * @param number_of_hard_disks: total number of hard disks available
 * @param hard_disk:            I/O queue containing processes that are ready
 *                              to use the hard disk
 */
void ListProcessCurrentlyUsingHardDisk(int number_of_hard_disks, std::vector<queue<HardDiskProcess>> hard_disk){
  for (int i = 0; i < number_of_hard_disks; ++i){
    cout << "disk" << i << ": ";
    if (!hard_disk[i].empty()){
      cout << "pid " << hard_disk[i].front().get_process().get_pid() << " " << hard_disk[i].front().get_file() << endl;
      cout << "queue: ";
      hard_disk[i].pop();
      while(!hard_disk[i].empty()){
        cout << "pid " << hard_disk[i].front().get_process().get_pid() << " " << hard_disk[i].front().get_file() << ", ";
        hard_disk[i].pop();
      }
    }
    else
      cout << "empty";
    cout << endl;
  }
}

/**
 * CoutFrameTable() displays the entire content of frame table to cout.
 * The frame table column is as follows:
 * Frame # | Page # | PID | Timestamp
 *
 * @param frame_table: contains memory frame, page number, pid, and timestamp
 *                     of process
 * @param no_of_rows:  number of rows in the frame table
 * @param no_of_cols:  number of columns in the frame table
 */
void CoutFrameTable(std::vector<std::vector<size_t>> frame_table, int no_of_rows, int no_of_cols){
  cout << "Frame # | Page # | PID | Timestamp" << endl;
  for (int i = 0; i < no_of_rows; ++i){
    if (frame_table[i][0] != -1){
      cout << i << " ";
      for (int j = 0; j < no_of_cols; ++j){
        cout << frame_table[i][j] << " ";
      }
      cout << endl;
    }
  }
}

int main(int argc, char **argv){
  size_t ram_memory, page_number;
  int hard_disks;
  cout << "How much RAM memory is there on the simulated computer? ";
  cin >> ram_memory;
  if (ram_memory > 4000000000){
    cerr << "error: RAM memory >4000000000\n";
    exit(1); // 1 indicates an error occurred
  }
  cout << "What is the size of a page? ";
  cin >> page_number;
  cout << "How many hard disks the simulated computer has? ";
  cin >> hard_disks;

  Computer a_computer{ram_memory, page_number, hard_disks};

  // hashtable for priority scheduling queue
  std::map<int,Process> cpu_queue;
  std::queue<Process> cpu;

  // representation of hard disk and its IO queue
  std::vector<queue<HardDiskProcess>> hard_disk (a_computer.num_hard_disks());

  // frame table
  std::vector<std::vector<size_t>> frame_table;
  size_t no_of_rows = ram_memory / page_number;
  int no_of_cols = 3;
  frame_table.resize(no_of_rows, std::vector<size_t>(no_of_cols, -1));

  int pid = 0;
  int timestamp = 0;
  string line;
  while(getline(cin, line)){
    stringstream input_stream(line);
    string first_command;
    string second_command = "empty";
    string third_command = "empty";
    input_stream >> first_command >> second_command >> third_command;
    if (first_command == "A"){
      // new process arrived
      if (second_command == "empty"){
        cerr << "invalid command\n";
      }
      else{
        // catch invalid stoi conversion
        bool try_block = true;
        try{
          stoi(second_command);
        }
        catch(invalid_argument){
          cerr << "caught exception: invalid input\n";
          try_block = false;
        }
        if (try_block){
          // create a new process and insert into cpu_queue
          Process new_process(++pid, stoi(second_command), 0);
          cpu_queue.insert(std::pair<int,Process>(stoi(second_command), new_process));

          // if cpu is empty push process into cpu
          if (cpu.empty()){
            auto it = cpu_queue.rbegin();
            cpu.push(it->second);
            cpu_queue.erase(std::prev(cpu_queue.end()));
          }

          // if cpu is not empty check if new process has higher priority
          else{
            auto currently_running_process = cpu.front();
            if (currently_running_process.get_priority() < stoi(second_command)){
              // preemptive
              // process with the higher priority gets the cpu
              cpu_queue.insert(std::pair<int,Process>(currently_running_process.get_priority(), currently_running_process));
              cpu.pop();

              // push next highest priority process into cpu
              if (cpu.empty()){
                auto it = cpu_queue.rbegin();
                cpu.push(it->second);
                cpu_queue.erase(std::prev(cpu_queue.end()));
              }
            }
          }
          // allocate memory for it’s first page
          int i = 0;
          for (; i < no_of_rows; ++i){
            if (frame_table[i][1] == -1){
              // update page #
              frame_table[i][0] = 0;
              // update pid
              frame_table[i][1] = new_process.get_pid();
              // update timestamp
              frame_table[i][2] = ++timestamp;
              break;
            }
          }
          // if memory is full,
          // take spot of memory that hasn't been used the longest
          if (i == no_of_rows){
            int lowest = 10000000;
            int lowest_row;
            for (i = 0; i < no_of_rows; ++i){
              if (frame_table[i][2] < lowest){
                lowest = frame_table[i][2];
                lowest_row = i;
              }
            }
            // update the frame table
            // update page #
            frame_table[lowest_row][0] = 0;
            // update pid
            frame_table[lowest_row][1] = cpu.front().get_pid();
            // update timestamp
            frame_table[lowest_row][2] = ++timestamp;
          }
        }
      }
    }
    else if (first_command == "t"){
      // terminate currently running process
      // first erase memory from frame table
      for (int i = 0; i < no_of_rows; ++i){
        if (frame_table[i][1] == cpu.front().get_pid()){
          frame_table[i][0] = -1;
          frame_table[i][1] = -1;
          frame_table[i][2] = -1;
        }
      }
      // terminate the process and
      // push next highest priority process into cpu
      if (!cpu.empty()){
        cpu.pop();
        if (!cpu_queue.empty()){
          auto it = cpu_queue.rbegin();
          cpu.push(it->second);
          cpu_queue.erase(std::prev(cpu_queue.end()));
        }
      }
      else
        cerr << "empty\n";
    }
    else if (first_command == "d"){
      // currently running process requests hard disk
      if (second_command == "empty" || third_command == "empty"){
        cerr << "invalid command\n";
      }
      else{
        if (cpu.empty()){
          cerr << "empty\n";
        }
        else{
          // catch invalid stoi conversion
          bool try_block = true;
          try{
            stoi(second_command);
          }
          catch(invalid_argument){
            cerr << "caught exception: invalid input\n";
            try_block = false;
          }
          if (try_block){
            // create a HardDiskProcess object storing the file name
            HardDiskProcess a_process(cpu.front(), third_command);
            cpu.pop();

            // push process to io_queue of specified hard disk
            // process at the front of io_queue will automatically
            // "use" the hard disk
            hard_disk[stoi(second_command)].push(a_process);

            // push next highest priority process into cpu
            if (!cpu_queue.empty()){
              auto it = cpu_queue.rbegin();
              cpu.push(it->second);
              cpu_queue.erase(std::prev(cpu_queue.end()));
            }
          }
        }
      }
    }
    else if (first_command == "D"){
      // hard disk has finished work for process
      if (second_command == "empty"){
        cerr << "invalid command\n";
      }
      else{
        // catch invalid stoi conversion
        bool try_block = true;
        try{
          stoi(second_command);
        }
        catch(invalid_argument){
          cerr << "caught exception: invalid input\n";
          try_block = false;
        }
        if (try_block){
          // pop it from the hard disk and insert back into cpu_queue
          HardDiskProcess a_process = hard_disk[stoi(second_command)].front();
          cpu_queue.insert(std::pair<int,Process>(a_process.get_process().get_priority(), a_process.get_process()));
          hard_disk[stoi(second_command)].pop();

          // pop process from cpu and reupdate cpu_queue
          if (!cpu.empty()){
            cpu_queue.insert(std::pair<int,Process>(cpu.front().get_priority(), cpu.front()));
            cpu.pop();
          }
          // push highest priority process into cpu
          auto it = cpu_queue.rbegin();
          cpu.push(it->second);
          cpu_queue.erase(std::prev(cpu_queue.end()));
        }
      }
    }
    else if (first_command == "m"){
      // currently running process requests memory operation
      if (second_command == "empty"){
        cerr << "invalid command\n";
      }
      else{
        // catch invalid stoi conversion
        bool try_block = true;
        try{
          stoi(second_command);
        }
        catch(invalid_argument){
          cerr << "caught exception: invalid input\n";
          try_block = false;
        }
        if (try_block){
          // update the next available memory
          int i = 0;
          for (; i < no_of_rows; ++i){
            // if page # already exists for process
            // don't add new entry, just update timestamp
            if (frame_table[i][1] == cpu.front().get_pid() && frame_table[i][0] == stoi(second_command) / 10)
            {
              frame_table[i][2] = ++timestamp;
              break;
            }
            else if (frame_table[i][0] == -1){
              // update page #
              frame_table[i][0] = stoi(second_command) / 10;
              // update pid
              frame_table[i][1] = cpu.front().get_pid();
              // update timestamp
              frame_table[i][2] = ++timestamp;
              break;
            }
          }
          // if memory is full,
          // take spot of memory that hasn't been used the longest
          if (i == no_of_rows){
            int lowest = 10000000;
            int lowest_row;
            for (i = 0; i < no_of_rows; ++i){
              if (frame_table[i][2] < lowest){
                lowest = frame_table[i][2];
                lowest_row = i;
              }
            }
            // update the frame table
            // update page #
            frame_table[lowest_row][0] = stoi(second_command) / 10;
            // update pid
            frame_table[lowest_row][1] = cpu.front().get_pid();
            // update timestamp
            frame_table[lowest_row][2] = ++timestamp;
          }
        }
      }
    }
    else if (first_command == "S"){
      if (second_command == "r"){
        // list process currently using CPU and ready queue
        if (cpu.empty()){
          cerr << "CPU: empty\n";
        }
        else{
          cout << "CPU: pid " << cpu.front().get_pid() << endl;
          cout << "Ready: ";
          for (auto const& it : cpu_queue){
            cout << "pid " << it.second.get_pid() << " priority " << it.second.get_priority() << ", ";
          }
          cout << endl;
        }
      }
      if (second_command == "i"){
        // list process currently using hard disk and their I/O queue
        ListProcessCurrentlyUsingHardDisk(a_computer.num_hard_disks(), hard_disk);
      }
      if (second_command == "m"){
        // for each used frame display the process number that occupies it
        // and the page number stored in it
        CoutFrameTable(frame_table, no_of_rows, no_of_cols);
      }
    }
  }
  return 0;
}
