#ifndef FILTER_DEF_H_
#define FILTER_DEF_H_

#include <systemc>
using namespace sc_dt;
using namespace sc_core;

const int FILTER_R_ADDR = 0x00000000;
const int FILTER_RESULT_ADDR = 0x00000004;
const int FILTER_CHECK_ADDR = 0x00000008;

union word {
  int sint;
  unsigned int uint;
  unsigned char uc[4];
};

sc_ufixed_fast<5, 0> filter_coef[] = {1/5, 1/3, 1/6};
const int M = 2; // down sampling rate
#endif
