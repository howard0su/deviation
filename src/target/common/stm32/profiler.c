#include "common.h"
#include <libopencm3/cm3/dwt.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/stm32/rcc.h>

#define SystemCoreClock 72000000

#define __PROF_STOPPED 0xFF
#define __PROF_WAITREPORT 0xFE

#define MAX_EVENT_COUNT 16

static uint32_t   time_start; // profiler start time
static uint32_t   time_event[MAX_EVENT_COUNT]; // events time
static const char *event_name[MAX_EVENT_COUNT]; // events name
static uint8_t    event_count = __PROF_STOPPED; // events counter
static uint32_t   counter;

void PROFILING_START()
{
    if (event_count != __PROF_STOPPED)
        return;
    memset(event_name, 0, sizeof(event_name));
    if (dwt_enable_cycle_counter()) {
        event_count = 0;
    }
}

void PROFILING_EVENT(const char *event)
{
    if (event_count >= __PROF_WAITREPORT)
        return;

    if (event_count < MAX_EVENT_COUNT) {
        time_event[event_count] = dwt_read_cycle_counter();
        event_name[event_count] = event;
        event_count++;
    }
}

void PROFILING_STOP()
{
    if (event_count >= __PROF_WAITREPORT)
        return;
    event_count = __PROF_WAITREPORT;
}

void PROFILING_REPORT()
{
  int32_t tick_per_1us;
  int32_t time_prev;
  int32_t timestamp;
  int32_t delta_t;

  tick_per_1us = SystemCoreClock / 1000000;

  printf("Profiling Result: sequence: \r\n"
               "--Event-----------------------|--timestamp--|----delta_t---\r\n");
  time_prev = 0;

  for (int i = 0; i < event_count; i++)
  {
    timestamp = (time_event[i] - time_start) / tick_per_1us;
    delta_t = timestamp - time_prev;
    time_prev = timestamp;
    printf("%-30s:%9d µs | +%9d µs\r\n", event_name[i], timestamp, delta_t);
  }
}
