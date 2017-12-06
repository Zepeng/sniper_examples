#include "read_simevent.h"
#include "SniperKernel/AlgFactory.h"
#include "SniperKernel/AlgBase.h"
#include "SniperKernel/SniperPtr.h"
#include "SniperKernel/SniperDataPtr.h"
#include "BufferMemMgr/EvtDataPtr.h"
#include "Event/SimHeader.h"
#include "Event/EventObject.h"
#include "BufferMemMgr/IDataMemMgr.h"

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

	return true;
}

bool ReadSimEvent::execute()
{
	EvtDataPtr<nEXO::SimHeader> headersim("/Event/Sim");
	nEXO::SimEvent* simevent = dynamic_cast<nEXO::SimEvent*>(headersim->event());
    std::cout << simevent->EventNumber() << std::endl;

	return true;
}



bool ReadSimEvent::finalize()
{
	return true;
}
