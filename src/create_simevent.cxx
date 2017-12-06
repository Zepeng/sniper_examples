#include "create_simevent.h"
#include "SniperKernel/AlgFactory.h"
#include "SniperKernel/SniperPtr.h"
#include "SniperKernel/SniperDataPtr.h"
#include "BufferMemMgr/EvtDataPtr.h"
#include "EvtNavigator/NavBuffer.h"
#include "Event/SimHeader.h"
#include "BufferMemMgr/IDataMemMgr.h"

#include "RootWriter/RootWriter.h"
#include "TFile.h"
#include "TTree.h"

DECLARE_ALGORITHM(MC_change);

	MC_change::MC_change(const std::string& name)
: AlgBase(name)
{
	declProp("InputMCName", InputMCName="test.root");
	declProp("InputMCDir", InputMCDir="");

}

MC_change::~MC_change()
{

}

	bool
MC_change::initialize()
{
	std::string filename = InputMCDir + "/" + InputMCName;
	f = new TFile(filename.c_str());
	t1 = (TTree*)f->Get("nEXOevents");
	t1->SetBranchAddress("EventNumber", &fEventNumber);
	t1->SetBranchAddress("GenX", &fGenX);
	t1->SetBranchAddress("GenY", &fGenY);
	t1->SetBranchAddress("GenZ", &fGenZ);
	t1->SetBranchAddress("TotalEventEnergy", &fTotalEventEnergy);
	t1->SetBranchAddress("InitNumOP", &fInitNOP);
	t1->SetBranchAddress("NumTE", &fNTE);
	t1->SetBranchAddress("TEX", &fpTEX);
	t1->SetBranchAddress("TEY", &fpTEY);
	t1->SetBranchAddress("TEZ", &fpTEZ);
	TotalEvent = t1->GetEntries();
	EVENT=0;
	return true;
}

	bool
MC_change::execute()
{
	fTEEnergy.clear();
	fTEX.clear();
	fTEY.clear();
	fTEZ.clear();
	t1->GetEntry(EVENT);
	EVENT++;
	for(int i=0; i<fNTE; i++){
		fTEEnergy.push_back( 9.47e-7 );
		fTEX.push_back( (double)(*fpTEX)[i] );
		fTEY.push_back( (double)(*fpTEY)[i] );
		fTEZ.push_back( (double)(*fpTEZ)[i] );
	}
	// 1. create an event navigator
	nEXO::EvtNavigator* nav = new nEXO::EvtNavigator();
	TTimeStamp ts;
	nav->setTimeStamp(ts);

	// 2. put into data buffer
	SniperPtr<IDataMemMgr> mMgr("BufferMemMgr");
	mMgr->adopt(nav, "/Event");

	// 3. create SimHeader
	nEXO::SimHeader* header = new nEXO::SimHeader();
	header->setEventID(fEventNumber);
	nav->addHeader(header);


	// 4. create SimEvent
	nEXO::SimEvent* event = new nEXO::SimEvent();
	// * fill all data
	// **
	
	event->EventNumber(fEventNumber);
	event->GenX(fGenX);
	event->GenY(fGenY);
	event->GenZ(fGenZ);
	event->TotalEventEnergy(fTotalEventEnergy);
	event->InitNOP(fInitNOP);
	event->NTE(fNTE);
	event->TEEnergy(fTEEnergy);
	event->TEX(fTEX);
	event->TEY(fTEY);
	event->TEZ(fTEZ);

	header->setEvent(event);

	return true;
}

	bool
MC_change::finalize()
{
	return true;
}
