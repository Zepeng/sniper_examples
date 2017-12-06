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

		int PadNumOnTile;
		double sample_time;
		double PadSize;
		double TileSize;
		double NoiseCut;
		double WaveformCut;
		double EquNoise;
		std::string event_type;
        double ChannelRiseTime(std::vector<double>& fWaveform, double samplingfrequency);
        double EventRiseTime(std::vector<double>& fWaveform, double samplingfrequency);
        double FreqChargeRecon(std::vector<double>& fWaveform, double samplingfrequency);
        void BesselFilter(std::vector<double>& UnFiltered, std::vector<double>& Filtered);
	private:
		double Diffusion;
		double Field;
		double SamplingFrequency;
		double Noise;
		double TotE;

};

#endif
