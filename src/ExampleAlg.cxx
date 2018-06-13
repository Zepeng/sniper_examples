//Modified the example from nexo-offline/Analysis/ExampleAlg. --061318 zpli
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
#include "TStyle.h"
#include "TLegend.h"
#include "TCanvas.h"

DECLARE_ALGORITHM(ExampleAlg);

ExampleAlg::ExampleAlg(const std::string& name) : AlgBase(name){ }

ExampleAlg::~ExampleAlg() { }

bool ExampleAlg::initialize(){
  sCharge = new TH1F("sCharge","Channel Charge",100,0,10000);
  nCharge = new TH1F("nCharge","Noise Channel Charge", 100, 0, 10000);
  return true;
}

bool ExampleAlg::execute() {

  EvtDataPtr<nEXO::ElecHeader> elecHeader(getParent(), "/Event/Elec");
  nEXO::ElecEvent* elecEvent = dynamic_cast<nEXO::ElecEvent*>(elecHeader->event());  //<--segfault

  std::vector<nEXO::ElecChannel>& elecChannels = elecEvent->ElecChannels(); 

  for(std::vector<nEXO::ElecChannel>::iterator it = elecChannels.begin(); it != elecChannels.end(); it++){
    nEXO::ElecChannel& elecChannel = (*it);
    if(elecChannel.ChannelNoiseTag() == 0)
        sCharge->Fill(elecChannel.ChannelCharge());
    else
        nCharge->Fill(elecChannel.ChannelCharge());
  }

  return true;
} 


bool ExampleAlg::finalize(){
  
  TCanvas* canvas = new TCanvas("canvas","",800,600);
  gStyle->SetOptStat(0);
  TLegend* leg = new TLegend(0.7, 0.7, 0.85, 0.85);
  nCharge->Draw();
  nCharge->SetXTitle("# of e");
  nCharge->SetYTitle("# of Channels");
  nCharge->SetLineColor(kRed);
  leg->AddEntry(nCharge, "Noise Channel", "l");
  leg->AddEntry(sCharge, "Hit Channel", "l");
  sCharge->Draw("same");
  sCharge->SetLineColor(kBlue);
  leg->Draw();
  canvas->SaveAs("ChannelCharge.png");
  
  return true;  
}
