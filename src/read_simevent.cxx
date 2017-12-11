#include "read_simevent.h"
#include "SniperKernel/AlgFactory.h"
#include "SniperKernel/AlgBase.h"
#include "SniperKernel/SniperPtr.h"
#include "SniperKernel/SniperDataPtr.h"
#include "BufferMemMgr/EvtDataPtr.h"
#include "Event/SimHeader.h"
#include "Event/EventObject.h"
#include "BufferMemMgr/IDataMemMgr.h"

#include "TH1.h"
#include "TFile.h"
#include "TCanvas.h"

DECLARE_ALGORITHM(ReadSimEvent);

	ReadSimEvent::ReadSimEvent(const std::string& name)
: AlgBase(name)
{
	//declProp("Noise",     Noise=0.);
	//declProp("SamplingFrequency",	SamplingFrequency=2.);

}

ReadSimEvent::~ReadSimEvent()
{

}

bool ReadSimEvent::initialize()
{
    //Define the histogram delared in header file
    h_te = new TH1F("h_te", "h_te", 100, 500000, 150000);

	return true;
}

bool ReadSimEvent::execute()
{
	EvtDataPtr<nEXO::SimHeader> headersim("/Event/Sim");
	nEXO::SimEvent* simevent = dynamic_cast<nEXO::SimEvent*>(headersim->event());
    //use the class's functions to read variables
    std::cout << simevent->EventNumber() << std::endl;
    //read a variable and fill it in a histogram
    h_te->Fill(simevent->NTE());
	return true;
}



bool ReadSimEvent::finalize()
{
    //Draw the histogram and save it to pdf.
    TCanvas* c1 = new TCanvas("c1", "c1", 800, 600);
    h_te->Draw();
    c1->Print("numte.pdf", "pdf");
	return true;
}
