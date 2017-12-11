#ifndef ReadSimEvent_h
#define ReadSimEvent_h

#include "SniperKernel/AlgBase.h"
#include <string>
#include "TH1.h"

class ReadSimEvent: public AlgBase 
{
	public:
		ReadSimEvent(const std::string& name);
		~ReadSimEvent();

		bool initialize();
		bool execute();
		bool finalize();

	private:
        //declare a 1D histogram
        TH1* h_te;
};

#endif
