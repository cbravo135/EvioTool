//
//  Leaf.h
//  AprimeAna
//
//  Created by Maurik Holtrop on 5/19/14.
//  Copyright (c) 2014 University of New Hampshire. All rights reserved.
//

#ifndef __Leaf__
#define __Leaf__

#include <iostream>
#include <vector>
using namespace std;

#include "TROOT.h"
#include "TObject.h"
#include "TNamed.h"

enum LeafDataTypes_t { Leaf_None, Leaf_Bank, Leaf_Int, Leaf_Uint32, Leaf_Float, Leaf_Double, Leaf_String ,Leaf_FADC, Leaf_END};

class Leaf_base: public TNamed {
  //// This is a base class for the templated leaf class. This makes it a lot easier
  //// to use the Leaf class, since you don't always need to know the type of Leaf.
public:
  int tag;
  int num;
  
public:
  Leaf_base(): tag(0),num(0){;};
  Leaf_base(int itag, int inum): tag(itag),num(inum){;};
  virtual void Clear(Option_t *){;};
  virtual size_t Get_size(void){return(0);};
  virtual void Print(Option_t *op) const {;};
  ClassDef(Leaf_base,1);
};

template <class T> class Leaf : public Leaf_base {
  //// This class stores the actual data of the EVIO "vectors"
public:
  //  string name;
  //  string description;
  
  vector<T> data;
  
public:
  Leaf(){;};
  Leaf(Leaf const&cp):Leaf_base(cp.tag,cp.num), data(cp.data){ SetName(cp.GetName()); SetTitle(cp.GetTitle());};
  Leaf(Leaf *cp): Leaf_base(cp->tag,cp->num),data(cp->data){ SetName(cp->GetName()); SetTitle(cp->GetTitle());};
  Leaf(string n,int ta, int nu,string desc): Leaf_base(ta,nu) {SetName(n.c_str()); SetTitle(desc.c_str());}
  
  void Clear(Option_t *){
    // Clear out the contents of the leaf, but do not delete the leaf.
    data.clear();
  }
  
  void Push_data_vector(vector<T> &vec){
    // Put the data from vector at the end of the leaf.
    data.insert(data.end(),vec.begin(),vec.end());
  }

  void Push_data_array(const T* dat, const int len){
    // Put the data from vector at the end of the leaf.
    data.insert(data.end(),dat,dat+len);
  }

  void Swap_data_vector(vector<T> &vec){
    // Put the data from vector at the end of the leaf.
    data.swap(vec);
  }
  
  void Push_back(T dat){
    data.push_back(dat);
  }
  vector<T> Get_data_vector(void){
    // Get the data vector.
    return data;
  }
  T Get_data(const int indx){
    // Get the data at indx. Throws exception if out of range.
    return(data.at(indx));
  }
  size_t size(void){
    // Get the size in the data vector.
    return(data.size());
  }
  static int type(void){
    // Return the type stored in this leaf. Static so can be used as
    // Leaf<int>::Get_type() etc.
    return(Leaf_None);
  };
  
  void Print(Option_t *op) const{
    // Print your self, and num of the contents.
    // To be compatible with TObject::Print(), which is needed for virtual use,
    // the num and level are encoded in the opt= "N10L3" is num=10, level=3
    string s;
    int level;
    int n;
    if(strlen(op)>4){
      sscanf(op,"N%dL%d",&n,&level);
      for(int i=0;i<level;i++) s.append("    ");
      cout << s << ClassName() << ":\t" << GetName() << "\t tag = " << tag << " num = " << num << endl;
      cout << s << "Data: ";
      for(int i=0;i<n && i< (int)data.size();i++){
        cout << " " << data[i];
      }
    }else{
      cout << "  -?- " << ClassName() << ":\t" << GetName() << "\t tag = " << tag << " num = " << num << endl;
    }
  };
  
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winconsistent-missing-override"
  ClassDef(Leaf,1);
#pragma clang diagnostic pop

};

#endif /* defined(__AprimeAna__Leaf__) */
