#include "mp_scheduler.h"
#include <boost/algorithm/string.hpp>

MP_Scheduler::MP_Scheduler(schedule algo, int quantum, string filename) {
  m_algo    = algo;
  m_logger  = new MP_Logger(filename);
  m_quantum = -1;

  if(m_algo == ROUND_ROBIN) {
    m_quantum = quantum;
  } else if (m_algo == RERUN) {
    reschedule();
  }
}

bool MP_Scheduler::has_ready_threads() {
  if((m_algo == RERUN && rerun_queue.empty()) || (m_algo != RERUN && m_ready_queue.empty())) {
    return false;
  }

  return true;
}

MP_Thread* MP_Scheduler::get_next_thread() {
  if( ! has_ready_threads() ) {
    return NULL;
  }

  MP_Thread* next_thread = NULL;

  if( m_algo == FCFS || m_algo == ROUND_ROBIN) {
    next_thread = m_ready_queue.front();
    m_ready_queue.pop();
    m_logger->log<MP_Thread>(*next_thread);
  } else if ( m_algo == RERUN ) {

    string next = rerun_queue.front();
    rerun_queue.pop();

    vector<string> results;
    boost::split(results, next, [](char c){return c == ',';});

    string next_label = results[0];
    int quantum       = atoi(results[1].c_str());
    next_thread       = m_thread_map[next_label];

    next_thread->set_quantum(quantum);
  }

  return next_thread;
}

void MP_Scheduler::reschedule() {
  ifstream file = m_logger->ReadFile();
  string line;
  while(getline(file, line)){
    istringstream iss(line);
    string label;
    if(!(iss >> label)){
      cout << "ERROR READING IN LINE" << endl;
      break;
    }
    if(label == "") {
      break;
    }
    rerun_queue.push(label);
  }
}

void MP_Scheduler::add_ready(MP_Thread *thread) {
  thread->set_quantum(m_quantum);

  if(m_algo == RERUN) {
    m_thread_map[thread->getLabel()] = thread;
  } else {
    m_ready_queue.push(thread);
  }
}

void MP_Scheduler::clear_thread(string label) {
  m_thread_map.erase(label);
}

