#include "ExampleAlg.h"
#include "SniperKernel/AlgFactory.h"
#include "SniperKernel/AlgBase.h"
#include "SniperKernel/SniperPtr.h"
#include "SniperKernel/SniperDataPtr.h"
#include "BufferMemMgr/EvtDataPtr.h"
#include "BufferMemMgr/IDataMemMgr.h"
#include "Event/SimHeader.h"
#include "Event/ElecHeader.h"

#include "TH1.h"
#include "TCanvas.h"

DECLARE_ALGORITHM(ExampleAlg);

ExampleAlg::ExampleAlg(const std::string& name) : AlgBase(name){ }

ExampleAlg::~ExampleAlg() { }

bool ExampleAlg::initialize(){
  hChannels = new TH1F("hChannels","Hit Channels",100,0,100);
  return true;
}

bool ExampleAlg::execute() {

  EvtDataPtr<nEXO::ElecHeader> elecHeader(getParent(), "/Event/Elec");
  nEXO::ElecEvent* elecEvent = dynamic_cast<nEXO::ElecEvent*>(elecHeader->event());  //<--segfault

  std::vector<nEXO::ElecChannel>& elecChannels = elecEvent->ElecChannels(); 

  for(std::vector<nEXO::ElecChannel>::iterator it = elecChannels.begin(); it != elecChannels.end(); it++){
    nEXO::ElecChannel& elecChannel = (*it);
    int localId = elecChannel.ChannelLocalId();
    hChannels->Fill(localId);
  }

  return true;
} 


bool ExampleAlg::finalize(){
  
  TCanvas* canvas = new TCanvas("canvas","",800,600);
  hChannels->Draw();
  canvas->SaveAs("Channels.png");
  
  return true;  
}
