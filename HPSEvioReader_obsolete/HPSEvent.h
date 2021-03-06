//
//  HPSEvent.h
//  HPSEvioReader
//
//  Created by Maurik Holtrop on 5/7/14.
//  Update: 12/2018
//
// The main "event" is set of nested structures that have a either static arrays or vectors in them.
// This design is on purpose, it allows for very fast data access and can be streamed or memory shared fairly easily.
// With this design the decoder can run at over 30kHz for the 2015 data structure on an i7 laptop.
// Using classes, or nested classes, quicky creates headaches like vtables that should not be shared or streamed.
//
// This file and the associated cc file contain some tools for Evio_Event_t.
// The EvioEvent class is a helper class to provided various useful services when dealing with the Evio_Event_t structure
//
#ifndef __HPSEvent__
#define __HPSEvent__

#ifndef __clang__
#define nullptr  NULL
#endif

/* The classes below are exported */
#pragma GCC visibility push(default)

#include <stdio.h>
#include <iostream>
#include <vector>
using namespace std;

#define EVIO_EVENT_HEADER  0xC000  // 49152
#define EVIO_EVENT_TRIGGER 0x002e  //    46
#define EVIO_FADC_1_BANK	 0xE101  // 57601
#define EVIO_FADC_3_BANK	 0xE103  // 57603
#define EVIO_FADC_7_BANK	 0xE102  // 57602
#define EVIO_TI_BANK	  	 0xE10A  // 57610
#define EVIO_SSP_BANK			 0xE10C  // 57612
#define EVIO_CFG_BANK		   0xE10E  // 57514

// HPS TEST RUN
#define EVIO_ECAL_FADC_CRATE_1     37//1   // ECAL Crate 1
#define EVIO_ECAL_FADC_CRATE_2     39// 2   // ECAL Crate 2
#define EVIO_SVT_CRATE_MIN         51   // SVT Crate
#define EVIO_SVT_CRATE_MAX         66   // SVT Crate

#define EVIO_PRESTART 17
#define EVIO_GO       18

// HPS Engineering RUN

static const int ECAL_FADC_MASTER=46;
static const int ECAL_FADC_CRATE1=37;
static const int ECAL_FADC_CRATE2=39;
static const int ECAL_FADC_GTP1=38;
static const int ECAL_FADC_GTP2=40;

#define EVIO_ROC_HPS1			37
#define EVIO_ROC_HPS2			39
#define EVIO_ROC_HPS11		46
#define EVIO_ROC_HPS12		58

// HPS TEST RUN SVT Definitions
#define MAX_NUM_FADC     25
#define MAX_FADC_DATA    50
#define MAX_NUM_SVT_FPGA 12
#define MAX_NUM_SVT_SAMPLES 6
#define MAX_SVT_DATA   1024
//#define NUM_FPGA_TEMPS 7

struct FADC_chan_f13_t{
  unsigned char chan;
  vector<unsigned short> samples;
//  unsigned short samples[50];
};

struct FADC_chan_f15_t{
  int chan;
  vector<short> time;
  vector<int> adc;
};

struct FADC_data_f13_t {
  unsigned char crate;
  unsigned char  slot;
  unsigned int  trig;
  unsigned long long time;       /* 64 bit unsinged int */
  vector<FADC_chan_f13_t> data;
};

struct FADC_data_f15_t {
  int  crate;
  int  slot;
  int  trig;
  unsigned int  time;
  vector<FADC_chan_f15_t> data;
};

// For an explanation on bit packing with CLANG or G++: http://jkz.wtf/bit-field-packing-in-gcc-and-clang
// Key issue - spanning a 32-bit long data field, the upper bit fields need to have type unsigned int.
// Note that the bools are OK.
// The *memory shape* of this structure or class MUST correspond perfectly to the memory layout of the EVIO data.
// That way we can use the much faster memory overlay rather than data copy.
//
struct SVT_header_t {
  unsigned char XXX1    :8;   // bit 0-7   UNKNOWN
  unsigned char feb_id  :8;   //  bit  8-15   = FEB ID
  unsigned int  chan    :7;   //   bit  16-22 = Channel ID
  unsigned int  apv     :3;   //   bit  23-25 = APV number
  unsigned int  hyb_id  :3;    //   bit  26-28 = Hybrid ID
  bool          isTail  :1;   //    bit  29  = Is a Tail (seem to never be the case?)
  bool          isHeader:1;  //   bit  30   = Is a Header (these are useless?)
  bool          XXX2    :1;    //    bit  31  =  UNKNOWN
};

struct SVT_chan_t{
  unsigned short samples[MAX_NUM_SVT_SAMPLES];
  SVT_header_t head;
};

struct EVIO_Event_t{
  // The data from the EVIO file header is stored here:
  unsigned int run_number;
  unsigned int start_time;
  unsigned int file_number;
  
  // The data from the EVIO EVENT header is stored here:
  unsigned int topnode_tag;
  unsigned int event_number;
  unsigned int event_type;
  
  // Trigger Bank information
  
  unsigned int or_bits;
  unsigned int top_bits;
  unsigned int bottom_bits;
  unsigned int pair_bits;
  unsigned int trig_time;
  
  // FADC data encountered:
  
  vector<FADC_data_f13_t> FADC_13;  // Mode 13 - Nsamples.
  vector<FADC_data_f15_t> FADC_15;  // Mode 15 - Integrated.
  vector<SVT_chan_t> SVT_data;

};

// This keeps the root interpreter happy when dealing with these structures.

#ifndef __CINT__
template class std::vector<FADC_chan_f13_t>;
template class std::vector<FADC_chan_f15_t>;
template class std::vector<FADC_data_f13_t>;
template class std::vector<FADC_data_f15_t>;
template class std::vector<SVT_chan_t>;
#endif

void EvioEventClear(EVIO_Event_t *evt);
void EvioEventInit(EVIO_Event_t *evt);
void EvioEventPrint(EVIO_Event_t *evt, int level=0);
void EvioEventPrintSVT(EVIO_Event_t *evt, int level=0);
void EvioEventPrintECAL(EVIO_Event_t *evt, int level=0);

#pragma GCC visibility pop
#endif /* defined(__EvioTool__EvioEvent__) */
