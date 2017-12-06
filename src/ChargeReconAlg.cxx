#include "ChargeReconAlg.h"
#include "SniperKernel/AlgFactory.h"
#include "SniperKernel/AlgBase.h"
#include "SniperKernel/SniperPtr.h"
#include "SniperKernel/SniperDataPtr.h"
#include "BufferMemMgr/EvtDataPtr.h"
#include "Event/SimHeader.h"
#include "Event/ElecHeader.h"
#include "Event/EventObject.h"
#include "BufferMemMgr/IDataMemMgr.h"
#include "Event/PidTmvaHeader.h"
#include "TMath.h"
#include "TRandom.h"
#include "TVirtualFFT.h"
#include "TF1.h"

DECLARE_ALGORITHM(ChargeReconAlg);

	ChargeReconAlg::ChargeReconAlg(const std::string& name)
: AlgBase(name)
{
	declProp("Noise",     Noise=0.);
	declProp("SamplingFrequency",	SamplingFrequency=2.);

}

ChargeReconAlg::~ChargeReconAlg()
{

}

bool ChargeReconAlg::initialize()
{

	return true;
}

bool ChargeReconAlg::execute()
{
    //read out digitized waveform
	EvtDataPtr<nEXO::ElecHeader> header("/Event/Elec");
	if (header.invalid()) {
		LogError << "can't find the digitization header." << std::endl;
		return false;
	}
	nEXO::ElecEvent* event = dynamic_cast<nEXO::ElecEvent*>(header->event());

	//set noise cut threshold
    if(Noise<10){
		NoiseCut=600;
		WaveformCut=1500;//Use twice the equivalent noise charge as a threshold cut.
	}
	else{
		NoiseCut = 600;
		WaveformCut = 1500;
	}

	// get the channels
	std::vector<nEXO::ElecChannel>& channels = event->ElecChannels();
	std::vector<Double_t>	ChargeWF_Time;
	std::vector<Double_t>	ChargeWF;

	double total_charge=0;
    int WFLen = 0;
	for (std::vector<nEXO::ElecChannel>::iterator it = channels.begin();it != channels.end(); ++it) {
		ChargeWF.clear();
		ChargeWF_Time.clear();
		nEXO::ElecChannel& channel = (*it);
		// get the waveform
		const std::vector<Double_t>& ampl_current = channel.WFAmplitude();//the waveform produced in digitization is current, needs to convert to charge waveform.
		const std::vector<Double_t>& time = channel.WFTime();
        std::vector<Double_t> currentwf;
        //Integrate current wavefor to produce charge waveform.
        for(int iter = 0; iter < ampl_current.size(); iter++)
        {
            currentwf.push_back(ampl_current[iter]);
            if(iter == 0) ChargeWF.push_back(ampl_current[iter]);
            else ChargeWF.push_back(ChargeWF[iter-1] + ampl_current[iter]);
            ChargeWF_Time.push_back(time[iter]);
        }
        WFLen = ChargeWF.size();
		if(WFLen<=3){
            std::cout << WFLen << std::endl;
			continue;
		}
		
        //calculate collected charge for every channel as an average of bins in the last 10 microsecond
		Double_t collect_charge=0;
		Int_t numbins = 20; 
		for(Int_t iWFLen=1; iWFLen <= numbins; iWFLen++){
			collect_charge += ChargeWF[WFLen-iWFLen];
		}
		collect_charge /=  double(numbins);
        std::cout << collect_charge << std::endl;
        if(collect_charge<0) collect_charge=0;

		//get the waveform of collected charge>0
		if(collect_charge<=NoiseCut)	continue;
        total_charge += collect_charge;
    }
    return true;
}
bool ChargeReconAlg::finalize()
{
	return true;
}
