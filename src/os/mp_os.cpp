#include "mp_os.h"
#include <iostream>

using namespace std;

MP_OS::MP_OS() {
  mp_scheduler = new MP_Scheduler();

}

void MP_OS::thread_create(void (*start_routine)()) {
  start_routine();
}

void MP_OS::run() {
}
