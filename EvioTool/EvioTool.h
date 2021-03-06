//
//  EvioTool.h
//  AprimeAna
//
//  Created by Maurik Holtrop on 5/17/14.
//  Copyright (c) 2014 University of New Hampshire. All rights reserved.
//

// It is entirely possible for this class to NOT inherit from TObject.
// You can still fill and do much of what you want, and even access the classes from
// the ROOT prompt. But since the whole point of these classes is to make EVIO data
// accessible in ROOT, we may as well derive from TObjec and get some of the
// usefull ROOT features, like being able to write these classes to a file.
// Of course, the cost is a litte overhead, but frankly, that is not much to worry
// about here.

#ifndef __EvioTool__
#define __EvioTool__

#include <cstdio>
#include <iostream>
#include <fstream>
#include <map>
#include <vector>
using namespace std;

// %%%%%%%%%%%%
// EVIO headers
// %%%%%%%%%%%%
#ifndef __CINT__
#include "evio.h"
#endif

#include <iostream>
#include "TROOT.h"
#include "TObject.h"
#include "TNamed.h"

#include "Leaf.h"
#include "FADCdata.h"
#include "Bank.h"

#define TOP_BANK_TYPE 0x10   // Standard container type.

enum DebugType_t {
  Debug_Info  = 0x01,
  Debug_Info2 = 0x02,   // Write the bank structure 
  Debug_L1    = 0x04,
  Debug_L2    = 0x08 };


class EvioTool: public Bank {
  
public:
  int fDebug;   // Bit wise!!!  0001 = Info on banks. 0x00
  bool fAutoAdd; // If true, add any bank encountered. If false add only banks already known, i.e. from the dictionary.
  int fChop_level; // Chop, or collapse, the top N bank levels.
  int fMax_level;  // Prune, or collapse, the bottom (deepest) N bank levels.
  // Setting fChop_level = 1 will mean the top most bank, usually a container bank tag=1 num=0,
  // is not copied but instead treated as if it wasn't there.
  // Setting fMax_level = fChop_level means you get only one deep banks. All banks of
  // higher level will have their contends inserted in the parent bank instead.
  
  bool fFullErase; // Useful with fAutoAdd=true and no dictionary, erase fully at new event, so bank structure is gone completely. Otherwise the bank structure is kept.

  // private:
  int evio_handle;

  const unsigned int *evio_buf; // Buf ptr to read event into.
  unsigned int evio_buflen;     // length of buffer.
  
public:
  EvioTool();
  int  Open(const char *filename,const char *dictf=NULL);
  void parseDictionary(const char *dictf);
  int NextNoParse(void);
  int Next(void);
  int ParseEvioBuff(const unsigned int *buf);                                  // Top level buffer parser.
  int ParseBank(const unsigned int *buf, int bankType, int depth, Bank *node); // Recursive buffer parser.
  int LeafNodeHandler(const unsigned int *buf,int len, int padding, int contentType,unsigned short tag,unsigned char num,Bank *node);
  Bank *ContainerNodeHandler(const unsigned int *buf, int len, int padding, int contentType,unsigned short tag, unsigned char num, Bank *node, int depth);
  int AddOrFillLeaf_FADCdata(const unsigned int *buf,int len,unsigned short tag,unsigned char num,Bank *node); // Non template special case for FADC.
  template<typename T> int AddOrFillLeaf(const unsigned int *buf,int len,unsigned short tag,unsigned char num,Bank *node){
    // Add or Fill an int leaf in the bank node.
    // If fAutoAdd is false, find the leaf with tag, num and fill it. If not found do nothing.
    // If fAutoAdd is true, if not found, a new leaf is added and filled.
    int loc = node->Find(tag,num);
    if( loc == -1){
      if(fAutoAdd){
        char str[100];
        sprintf(str,"Int-%ud-%ud",tag,num);
        if(fDebug&Debug_L2) cout << "Adding a new Leaf node to node: " << node->GetNum() << " with name: " << str << endl;
        node->Add_Leaf<T>(str,tag,num,"Auto added int leaf");
        loc= node->leafs->GetEntriesFast()-1;
      }else{
        return 0;
      }
    }
    
    if(fDebug&Debug_L2) cout << "Adding data to Leaf at idx = " << loc << " templated for type <" << ((Leaf<T> *)node->leafs->At(loc))->type() << "> \n";
    node->Push_data_array(loc, (T *)buf, len);
    
    return 1;
  };
  

  
  
  void SetDebug(int bits=0x0F){ fDebug = bits; }
//  int AddBank(Bank &b);
  
  void Test(void);
  
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
  ClassDef(EvioTool,1);
#pragma clang diagnostic pop

};

#endif /* defined(__EvioTool__) */
