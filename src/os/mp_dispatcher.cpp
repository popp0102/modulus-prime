#include "mp_dispatcher.h"
#include <iostream>

MP_Dispatcher* MP_Dispatcher::dispatcher = NULL;

MP_Dispatcher::MP_Dispatcher(MP_Scheduler *scheduler, MP_Thread *os_thread) {
  this->m_scheduler       = scheduler;
  this->m_os_thread       = os_thread;
  this->m_running_thread  = os_thread;
  dispatcher              = this;

  init_context_switch_handler();
}

void MP_Dispatcher::run() {
  while( m_scheduler->has_ready_threads() ) {
    MP_Thread *next_thread = m_scheduler->get_next_thread();
    execute_thread(next_thread);
  }
}

void MP_Dispatcher::set_quantum() {
  if (m_scheduler->needs_quantum()) {
    init_timer();
    setitimer(ITIMER_REAL, &it, NULL);
  }
}

void MP_Dispatcher::context_switch() {
  std::cout << "CONTEXT SWITCH!\n";
  m_scheduler->add_ready(m_running_thread);
  execute_thread(m_os_thread);
}

void MP_Dispatcher::interrupt(int i) {
  dispatcher->context_switch();
}

void MP_Dispatcher::init_context_switch_handler() {
  struct sigaction act, oact;
  act.sa_handler = interrupt;
  sigemptyset(&act.sa_mask);
  act.sa_flags = 0;
  sigaction(SIGALRM, &act, &oact);
}

void MP_Dispatcher::init_timer() {
  it.it_interval.tv_sec = 1;
  it.it_interval.tv_usec = 50000;
  it.it_value.tv_sec = 1;
  it.it_value.tv_usec = 100000;
}

void MP_Dispatcher::execute_thread(MP_Thread *on_deck_thread) {
  MP_Thread *current_running = m_running_thread;
  m_running_thread = on_deck_thread;
  swapcontext(current_running->get_context(), on_deck_thread->get_context());
}
