#ifndef ExampleAlg_h
#define ExampleAlg_h

#include "SniperKernel/AlgBase.h"
#include "TH1.h"

class ExampleAlg: public AlgBase{

  public:

  ExampleAlg(const std::string& name);
  ~ExampleAlg();

  bool initialize();
  bool execute();
  bool finalize();

  private:
  
  TH1F *sCharge;
  TH1F *nCharge;
};

#endif
