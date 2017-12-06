#ifndef create_simevent_h
#define create_simevent_h

#include "SniperKernel/AlgBase.h"
#include "TROOT.h"
#include "TTree.h"
#include "TH1.h"
#include "TH2.h"
#include <vector>
#include "TVector3.h"
#include "TObject.h"
#include "TBranch.h"


class create_simevent: public AlgBase 
{
	public:
		create_simevent(const std::string& name);
		~create_simevent();

		bool initialize();
		bool execute();
		bool finalize();
//		void GetGen();


	private:
		std::string InputMCName;
		std::string InputMCDir;
		int EVENT;
		int TotalEvent;
		TFile *f;
		TTree *t1;
		int fEventNumber;
		double fGenX;
		double fGenY;
		double fGenZ;
		double fTotalEventEnergy;
		int fInitNOP;
		int fNTE;
		std::vector<float> *fpTEEnergy;
		std::vector<float> *fpTEX;
		std::vector<float> *fpTEY;
		std::vector<float> *fpTEZ;
		std::vector<double> fTEEnergy;
		std::vector<double> fTEX;
		std::vector<double> fTEY;
		std::vector<double> fTEZ;
		
};

#endif
