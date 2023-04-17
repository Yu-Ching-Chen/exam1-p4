#ifndef _STIM_H_
#define _STIM_H_

#include <iomanip>
using std::setw;
#include "filter_def.h"
#include "filter.h"
#include "ready_valid_port.h"

using namespace sc_dt;
using namespace sc_core;


const float x_input_signal={0.500,   0.525,   0.549,   0.574,   0.598,   0.622,   0.646,   0.670,
                            0.693,   0.715,   0.737,   0.759,   0.780,   0.800,   0.819,   0.838,
                            0.856,   0.873,   0.889,   0.904,   0.918,   0.931,   0.943,   0.954,
                            0.964,   0.972,   0.980,   0.986,   0.991,   0.995,   0.998,   1.000,
                            1.000,   0.999,   0.997,   0.994,   0.989,   0.983,   0.976,   0.968,
                            0.959,   0.949,   0.937,   0.925,   0.911,   0.896,   0.881,   0.864,
                            0.847,   0.829,   0.810,   0.790,   0.769,   0.748,   0.726,   0.704,
                            0.681,   0.658,   0.634,   0.610,   0.586,   0.562,   0.537,   0.512,
                            0.488,   0.463,   0.438,   0.414,   0.390,   0.366,   0.342,   0.319,
                            0.296,   0.274,   0.252,   0.231,   0.210,   0.190,   0.171,   0.153,
                            0.136,   0.119,   0.104,   0.089,   0.075,   0.063,   0.051,   0.041,
                            0.032,   0.024,   0.017,   0.011,   0.006,   0.003,   0.001,   0.000,
                            0.000,   0.002,   0.005,   0.009,   0.014,   0.020,   0.028,   0.036,
                            0.046,   0.057,   0.069,   0.082,   0.096,   0.111,   0.127,   0.144,
                            0.162,   0.181,   0.200,   0.220,   0.241,   0.263,   0.285,   0.307,
                            0.330,   0.354,   0.378,   0.402,   0.426,   0.451,   0.475,   0.500};

SC_MODULE(stim) {

  Initiator initiator;

  sc_in_clk i_clk;
  sc_out<bool> o_rst;

  word data;


  void stim_gen() {
    //initilize handshaking signals
    //o_a.vld.write(false); 
    //o_b.vld.write(false); 
    //i_sum.rdy.write(false);
    for (int n = 0; n < 64; n++) {
        data.uc[0] = x_input_signal[n*M-1];
        mask[0] = 0xff;
        initiator.write_to_socket(FILTER_R_ADDR, mask, data.uc, 4);
        wait(1 * CLOCK_PERIOD, SC_NS);

        data.uc[0] = x_input_signal[n*M];
        mask[0] = 0xff;
        initiator.write_to_socket(FILTER_R_ADDR, mask, data.uc, 4);
        wait(1 * CLOCK_PERIOD, SC_NS);

        data.uc[0] = x_input_signal[n*M+1];
        mask[0] = 0xff;
        initiator.write_to_socket(FILTER_R_ADDR, mask, data.uc, 4);
        wait(1 * CLOCK_PERIOD, SC_NS);
    }
  }

  void sink() {

    bool done=false;
    int output_num=0;
    word data;

    cout << setw(8) << "time" << setw(8) << "y" << endl;

    while(true) {
      while(!done){
        initiator.read_from_socket(FILTER_CHECK_ADDR, mask, data.uc, 4);
        output_num = data.sint;
        if(output_num>0) done=true;
      }
      initiator.read_from_socket(FILTER_RESULT_ADDR, mask, data.uc, 4);
      initiator.write_to_socket(RAM_MM_BASE, mask, data_uc,
                            output_raw_data_offset);
      cout << setw(8) << sc_time_stamp();
      cout << setw(8) << data.uc << endl;
    }
  }


  SC_CTOR(stim) {

    SC_THREAD(stim_gen);
    sensitive << i_clk.pos();
    dont_initialize();

    SC_THREAD(sink);
    sensitive << i_clk.pos(); 
  }
};

#endif
