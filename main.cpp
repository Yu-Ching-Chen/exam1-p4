#include "stim.h"
#include "filter.h"
#define CLOCK_PERIOD 1.0

using namespace sc_dt;
using namespace sc_core;

int sc_main(int argc, char *argv[]) {
  //Create modules and signals
  stim testbench("testbench");
  filter dut("dut");
  SimpleBus<1, 2> bus("bus");
  ram ram("ram", "t_sck", RAM_MM_SIZE - 1);
  bus.set_clock_period(sc_time(CLOCK_PERIOD, SC_NS));

  testbench.initiator.i_skt(bus.t_skt[0]);
  bus.setDecode(0, FILTER_MM_BASE, FILTER_MM_BASE + FILTER_MM_SIZE - 1);
  bus.i_skt[0](filter.t_skt);
  bus.setDecode(1, RAM_MM_BASE, RAM_MM_BASE + RAM_MM_SIZE - 1);
  bus.i_skt[1](ram.t_skt);

  sc_start(100, SC_NS);
  return 0;
}
