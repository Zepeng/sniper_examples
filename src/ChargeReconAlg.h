#ifndef ChargeReconAlg_h
#define ChargeReconAlg_h

#include "SniperKernel/AlgBase.h"
#include <string>

class ChargeReconAlg: public AlgBase 
{
	public:
		ChargeReconAlg(const std::string& name);
		~ChargeReconAlg();

		bool initialize();
		bool execute();
		bool finalize();

		double NoiseCut;
		double WaveformCut;
		std::string event_type;
	
    private:
		double Diffusion;
		double Field;
		double SamplingFrequency;
		double Noise;
		double TotE;

};

#endif
