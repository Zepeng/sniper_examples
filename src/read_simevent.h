#ifndef ReadSimEvent_h
#define ReadSimEvent_h

#include "SniperKernel/AlgBase.h"
#include <string>

class ReadSimEvent: public AlgBase 
{
	public:
		ReadSimEvent(const std::string& name);
		~ReadSimEvent();

		bool initialize();
		bool execute();
		bool finalize();

	private:
		double Diffusion;
		double Field;
		double SamplingFrequency;
		double Noise;
		double TotE;

};

#endif
