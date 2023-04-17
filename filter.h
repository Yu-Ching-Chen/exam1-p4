#ifndef _FILTER_H_
#define _FILTER_H_

#define CLOCK_PERIOD 1.0

// Needed for the simple_target_socket
#define SC_INCLUDE_DYNAMIC_PROCESSES


#include <iostream>
using namespace std;

#include <systemc>
using namespace sc_dt;
using namespace sc_core;
using namespace std;

#include "filter_def.h"
#include "tlm.h"
#include "tlm_utils/simple_target_socket.h"
#include <cstdint>
#include "map.h"

class filter : public sc_module {
public:
  sc_in_clk i_clk;
  sc_in<bool> i_rst;
  
  sc_fifo<unsigned char> i_x_port;
  sc_fifo<int> o_y_port;

  tlm_utils::simple_target_socket<Adder> socket;
  enum { SIZE = 2 };

  void do_filter() {
    while (true) {
        _o_y = 0;

        _i_x = i_x_port.read();
        wait(1*CLOCK_PERIOD, SC_NS);

        _o_y = _o_y + _i_x * filter_coef[2];
        wait(1*CLOCK_PERIOD, SC_NS);

        _i_x = i_x_port.read();
        wait(1*CLOCK_PERIOD, SC_NS);

        _o_y = _o_y + _i_x * filter_coef[1];
        wait(1*CLOCK_PERIOD, SC_NS);

        _i_x = i_x_port.read();
        wait(1*CLOCK_PERIOD, SC_NS);

        _o_y = _o_y + _i_x * filter_coef[0];
        o_y_port.write(_o_y);
        wait();
    }
  }

  SC_HAS_PROCESS(filter);
  filter(sc_module_name name, int i) : 
  sc_module(n), t_skt("t_skt"), base_offset(0){ 

    
    socket.register_b_transport(this, &Adder::b_transport);

    // Initialize input and output data register
    for (int i = 0; i < SIZE; i++) i_data[i] =0;


    SC_THREAD(do_filter);  
    sensitive << i_clk.pos();
    dont_initialize();
    //reset_signal_is(i_rst, false); 
  }

  sc_ufixed_fast<5, 0> i_x() { return _i_x;}
  sc_ufixed_fast<8, 3> o_y() { return _o_y; }

private:
  const int id;
  sc_ufixed_fast<5, 0> _i_x;
  sc_ufixed_fast<8, 3> _o_y;
  unsigned int base_offset;

  void blocking_transport(tlm::tlm_generic_payload &payload,
                                      sc_core::sc_time &delay) {
    sc_dt::uint64 addr = payload.get_address();
    addr = addr - base_offset;
    unsigned char *mask_ptr = payload.get_byte_enable_ptr();
    unsigned char *data_ptr = payload.get_data_ptr();
    word buffer;
    switch (payload.get_command()) {
    case tlm::TLM_READ_COMMAND:
      switch (addr) {
      case FILTER_RESULT_ADDR:
        buffer.uint = o_y_port.read();
        break;
      case FILTER_CHECK_ADDR:
        buffer.uint = o_y_port.num_available();
        break;
      default:
        std::cerr << "Error! SobelFilter::blocking_transport: address 0x"
                  << std::setfill('0') << std::setw(8) << std::hex << addr
                  << std::dec << " is not valid" << std::endl;
        break;
      }
      data_ptr[0] = buffer.uc[0];
      data_ptr[1] = buffer.uc[1];
      data_ptr[2] = buffer.uc[2];
      data_ptr[3] = buffer.uc[3];
      break;

    case tlm::TLM_WRITE_COMMAND:
      switch (addr) {
      case FILTER_R_ADDR:
        if (mask_ptr[0] == 0xff) {
          i_x_port.write(data_ptr[0]);
        break;
      default:
        std::cerr << "Error! SobelFilter::blocking_transport: address 0x"
                  << std::setfill('0') << std::setw(8) << std::hex << addr
                  << std::dec << " is not valid" << std::endl;
        break;
      }
      break;

    case tlm::TLM_IGNORE_COMMAND:
      payload.set_response_status(tlm::TLM_GENERIC_ERROR_RESPONSE);
      return;
    default:
      payload.set_response_status(tlm::TLM_GENERIC_ERROR_RESPONSE);
      return;
    }
    payload.set_response_status(tlm::TLM_OK_RESPONSE); // Always OK
  }

};

#endif
