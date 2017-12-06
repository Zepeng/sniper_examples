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
	EvtDataPtr<nEXO::SimHeader> headersim("/Event/Sim");
	nEXO::SimEvent* simevent = dynamic_cast<nEXO::SimEvent*>(headersim->event());
    //std::cout << simevent->EventNumber() << std::endl;
    
    //read out digitized waveform
	EvtDataPtr<nEXO::ElecHeader> header("/Event/Elec");

	if (header.invalid()) {
		LogError << "can't find the digitization header." << std::endl;
		return false;
	}
	nEXO::ElecEvent* event = dynamic_cast<nEXO::ElecEvent*>(header->event());

    //Detector configuration used in digitization simulation.
	PadSize = event->PadSize();
	TileSize = event->TileSize();
    //Total energy
	TotE = event->TotalEventEnergy();
	event->SamplingFrequency(SamplingFrequency);
	event->Noise(Noise);
	event_type = event->Type();
	//set noise cut threshold
    if(Noise<10){
		NoiseCut=600;
		WaveformCut=1500;//Use twice the equivalent noise charge as a threshold cut.
		EquNoise=300;
	}
	else{
		NoiseCut = 600;
		WaveformCut = 1500;
		EquNoise=300;
	}

	PadNumOnTile = ((int)(TileSize+0.1))/((int)(PadSize+0.1));
	sample_time = 1.0/SamplingFrequency;

	double charge_platform=0;
	double dx_max=0, dy_max=0;
	double total_delta_time=0;
	double total_2delta_time=0;
	int ipeakmax_event=0;
	int fNumChannels=0;
	int fXNumChannels=0;
	int fYNumChannels=0;
	double TotColTime = 0;
	double TotColNum = 0;
	double TotCharge = 0;
	// get the channels
	std::vector<nEXO::ElecChannel>& channels = event->ElecChannels();
	std::vector<Double_t>	ChargeWF_Time;
	std::vector<Double_t>	ChargeWF;
	std::vector<Double_t>	ChargeWF_smooth;
	std::vector<Double_t>	TotalAmpl_smooth;
	std::vector<Double_t>	TotalTime_smooth;
    std::vector<Double_t>   mcchannelcharge;
    std::vector<Double_t>   reconchannelcharge;
    std::vector<Double_t>   oversamplingamp;

	TotalAmpl_smooth.clear();
	TotalTime_smooth.clear();
	int numxchannel=0, numychannel=0;
	double xposition[10000], yptile[10000], yposition[10000], xptile[10000];
	double xchanneltime[10000], xchannelcharge[10000], ychanneltime[10000], ychannelcharge[10000];
	double XTile[10000], YTile[10000];
	double drift_time=0, total_charge=0;
	int tilenum=0;
	for (std::vector<nEXO::ElecChannel>::iterator it = channels.begin();it != channels.end(); ++it) {
		ChargeWF.clear();
		ChargeWF_Time.clear();
		ChargeWF_smooth.clear();
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
            if(iter < 100)std::cout << ampl_current[iter] << std::endl;
            ChargeWF_Time.push_back(time[iter]);
        }
        const std::vector<Double_t> & os_amp = channel.OverSamplingAmp();
        oversamplingamp.clear();
        for(int iter = 1; iter < os_amp.size(); iter++)
        {
            oversamplingamp.push_back(os_amp[iter] - os_amp[iter-1]);
        }
        //Apply software harder Bessel Filter
        std::vector<double> before_filter;
        std::vector<double> after_filter;
        for(int iter = 0; iter < 200; iter++)
            before_filter.push_back(currentwf[0]);
        for(int iter = 0; iter < currentwf.size(); iter++)
            before_filter.push_back(currentwf[iter]);
        for(int iter = 0; iter < 200; iter++)
            before_filter.push_back(currentwf[currentwf.size()-1]);
        BesselFilter(before_filter, after_filter);
		std::vector<double>::const_iterator ampfirst = after_filter.begin() + 200;
		std::vector<double>::const_iterator amplast = after_filter.begin() + after_filter.size() - 200;
		std::vector<double> cut_waveform(ampfirst, amplast);
        for(int iter = 0; iter < cut_waveform.size(); iter++)
        {
            //if(iter == 0) ChargeWF.push_back(cut_waveform[iter]);
            //else ChargeWF.push_back(ChargeWF[iter-1] + cut_waveform[iter]);
            std::cout << cut_waveform[iter] << std::endl;
        }
        //const double channeltime = channel.ChannelFirstTime();
        int WFLen = ChargeWF.size();
        //std::cout << "WFLen" << WFLen << std::endl;
		if(WFLen<=3){
            //std::cout << WFLen << std::endl;
			continue;
		}
		drift_time = drift_time + ChargeWF[WFLen-1]*time[WFLen-1];// Charge-weighted drift time.

        WFLen = ChargeWF.size();
		double Ampl_max=0;
		for(Int_t iWFLen=0; iWFLen<WFLen; iWFLen++){
			if(Ampl_max<ChargeWF[iWFLen]){
				Ampl_max = ChargeWF[iWFLen];
			}
		}
        //if(ChargeWF[WFLen-1]>900)FreqChargeRecon(currentwf, SamplingFrequency);
	    //if(ChargeWF[WFLen-1]>900)FreqChargeRecon(oversamplingamp, SamplingFrequency);

        //smooth the waveform(5-point-smooth)
		for(Int_t iWFLen=0; iWFLen<WFLen; iWFLen++){
			if(iWFLen<WFLen-4){
				ChargeWF_smooth.push_back((ChargeWF[iWFLen]+ChargeWF[iWFLen+1]+ChargeWF[iWFLen+2]+ChargeWF[iWFLen+3]+ChargeWF[iWFLen+4])/5);
			}
			else if(iWFLen==WFLen-4){
				ChargeWF_smooth.push_back((ChargeWF[iWFLen]+ChargeWF[iWFLen+1]+ChargeWF[iWFLen+2]+ChargeWF[iWFLen+3])/4);
			}
			else if(iWFLen==WFLen-3){
				ChargeWF_smooth.push_back((ChargeWF[iWFLen]+ChargeWF[iWFLen+1]+ChargeWF[iWFLen+2])/3);
			}
			else if(iWFLen==WFLen-2){
				ChargeWF_smooth.push_back((ChargeWF[iWFLen]+ChargeWF[iWFLen+1])/2);
			}
			else if(iWFLen==WFLen-1){
				ChargeWF_smooth.push_back(ChargeWF[iWFLen]);
			}
			if(TotalAmpl_smooth.size()<WFLen){
				TotalAmpl_smooth.push_back(ChargeWF_smooth[iWFLen]);
                TotalTime_smooth.push_back(ChargeWF_Time[iWFLen]);
			}
			else if(TotalAmpl_smooth.size()==WFLen)
				TotalAmpl_smooth[iWFLen] = TotalAmpl_smooth[iWFLen] + ChargeWF_smooth[iWFLen];
		}
		WFLen=TotalTime_smooth.size();
		//cut false waveform(waveform without data)
		if(WFLen<10)	continue;
        
        //Calcualte rising time
        //double channel_risingtime = ChannelRiseTime(ChargeWF_smooth, SamplingFrequency);
        //std::cout << "channel rising time: " << channel_risingtime << std::endl;
		
		//calculate collected charge for every channel as an average of bins in the last 10 microsecond
		Double_t collect_charge=0;
		Int_t numbins = (int)(20/sample_time+0.01);
		for(Int_t iWFLen=1; iWFLen <= numbins; iWFLen++){
			collect_charge += ChargeWF[WFLen-iWFLen];
		}
		collect_charge /=  double(numbins);
		if(collect_charge<0) collect_charge=0;

		//get the waveform of collected charge>0
		if(collect_charge<=NoiseCut)	continue;
        //std::cout << "ChannelCharge" << " " <<  collect_charge- channel.ChannelNTE() << std::endl;
		mcchannelcharge.push_back(channel.ChannelNTE());
        reconchannelcharge.push_back(collect_charge);
        total_charge += collect_charge;
        //FreqChargeRecon(ChargeWF, SamplingFrequency);
		
		//get the variable Peak Number
		double trough = 9999999999;
		double ini_trough_value[200];
		int ini_trough_time_num[200];
		int ini_ipeak=0;
		double ini_peak_value[200];
		int ini_peak_time_num[200];
		ini_trough_value[0] = ChargeWF_smooth[0];
		ini_trough_time_num[0] = 1;
		for(Int_t iWFLen=1;iWFLen<7;iWFLen++){
			if(ini_trough_value[0]>=ChargeWF_smooth[iWFLen]){
				ini_trough_value[0] = ChargeWF_smooth[iWFLen];
				ini_trough_time_num[0] = iWFLen+1;
			}
		}
		ini_peak_value[0]=0;
		double aa[4],bb[4];
		int slope_change=1;
		for(Int_t iWFLen=7; iWFLen<WFLen-7; iWFLen++){
			bb[0] = ChargeWF_smooth[iWFLen] - (ChargeWF_smooth[iWFLen-1]+ChargeWF_smooth[iWFLen-2]+ChargeWF_smooth[iWFLen-3]+ChargeWF_smooth[iWFLen-4])/4;
			bb[1] = ChargeWF_smooth[iWFLen-1]-(ChargeWF_smooth[iWFLen-2]+ChargeWF_smooth[iWFLen-3]+ChargeWF_smooth[iWFLen-4]+ChargeWF_smooth[iWFLen-5])/4;
			bb[2] = ChargeWF_smooth[iWFLen-2]-(ChargeWF_smooth[iWFLen-3]+ChargeWF_smooth[iWFLen-4]+ChargeWF_smooth[iWFLen-5]+ChargeWF_smooth[iWFLen-6])/4;
			bb[3] = ChargeWF_smooth[iWFLen-3]-(ChargeWF_smooth[iWFLen-4]+ChargeWF_smooth[iWFLen-5]+ChargeWF_smooth[iWFLen-6]+ChargeWF_smooth[iWFLen-7])/4;
			aa[0] = ChargeWF_smooth[iWFLen] - (ChargeWF_smooth[iWFLen+1]+ChargeWF_smooth[iWFLen+2]+ChargeWF_smooth[iWFLen+3]+ChargeWF_smooth[iWFLen+4])/4;
			aa[1] = ChargeWF_smooth[iWFLen+1]-(ChargeWF_smooth[iWFLen+2]+ChargeWF_smooth[iWFLen+3]+ChargeWF_smooth[iWFLen+4]+ChargeWF_smooth[iWFLen+5])/4;
			aa[2] = ChargeWF_smooth[iWFLen+2]-(ChargeWF_smooth[iWFLen+3]+ChargeWF_smooth[iWFLen+4]+ChargeWF_smooth[iWFLen+5]+ChargeWF_smooth[iWFLen+6])/4;
			aa[3] = ChargeWF_smooth[iWFLen+3]-(ChargeWF_smooth[iWFLen+4]+ChargeWF_smooth[iWFLen+5]+ChargeWF_smooth[iWFLen+6]+ChargeWF_smooth[iWFLen+7])/4;
			if(ChargeWF_smooth[iWFLen]<=ChargeWF_smooth[iWFLen-1]+0.5*NoiseCut/5	&&
					ChargeWF_smooth[iWFLen]<=ChargeWF_smooth[iWFLen-2]+0.5*NoiseCut/5	&&
					ChargeWF_smooth[iWFLen]<=ChargeWF_smooth[iWFLen+1]				&&
					ChargeWF_smooth[iWFLen]<=ChargeWF_smooth[iWFLen+2]				&&
					bb[0]<=0.5*NoiseCut/5	&&	bb[1]<=0.5*NoiseCut/5	&&	bb[2]<=0.5*NoiseCut/5	&&	bb[3]<=0.5*NoiseCut/5		&&
					aa[0]<0				&&	aa[1]<0				&&	aa[2]<0				&&	aa[3]<0){
				if(slope_change==-1){
					ini_trough_value[ini_ipeak]=ChargeWF_smooth[iWFLen];
					ini_trough_time_num[ini_ipeak] = iWFLen+1;
				}
				else if(slope_change==1	&&	ChargeWF_smooth[iWFLen]<ini_trough_value[ini_ipeak]){
					ini_trough_value[ini_ipeak]=ChargeWF_smooth[iWFLen];
					ini_trough_time_num[ini_ipeak] = iWFLen+1;
				}
				slope_change=1;
			}
			else if(ChargeWF_smooth[iWFLen]>=ChargeWF_smooth[iWFLen-1]					&&
					ChargeWF_smooth[iWFLen]>=ChargeWF_smooth[iWFLen-2]					&&
					ChargeWF_smooth[iWFLen]>=ChargeWF_smooth[iWFLen+1]-0.5*NoiseCut/5	&&
					ChargeWF_smooth[iWFLen]>=ChargeWF_smooth[iWFLen+2]-0.5*NoiseCut/5	&&
					aa[0]>=-0.5*NoiseCut/5	&&	aa[1]>=-0.5*NoiseCut/5	&&	aa[2]>=-0.5*NoiseCut/5	&&	aa[3]>=-0.5*NoiseCut/5		&&
					bb[0]>0				&&	bb[1]>0				&&	bb[2]>0				&&	bb[3]>0){
				if(slope_change==1){
					ini_peak_value[ini_ipeak] = ChargeWF_smooth[iWFLen];
					ini_peak_time_num[ini_ipeak] = iWFLen+1;
					ini_ipeak++;
				}
				else if(slope_change==-1	&&	ChargeWF_smooth[iWFLen]>ini_peak_value[ini_ipeak-1]){
					ini_peak_value[ini_ipeak-1] = ChargeWF_smooth[iWFLen];
					ini_peak_time_num[ini_ipeak-1] = iWFLen+1;
				}
				slope_change=-1;
			}
		}
		int ipeak=0;
		double peak_value_test;
		int peak_time_test_num;
		int trough_time_test_num;
		double trough_value_1test;
		double trough_value_2test;
		int trough_time_2test_num;
		if(ini_ipeak>=1){
			peak_value_test = ini_peak_value[0];
			peak_time_test_num = ini_peak_time_num[0]-1;
			trough_time_test_num = ini_trough_time_num[0]-1;
			trough_value_1test = ini_trough_value[0];
		}
		if(ini_ipeak<2){
			trough_value_2test = 0;
		}
		else{
			trough_value_2test = ini_trough_value[1];
			trough_time_2test_num = ini_trough_time_num[1]-1;
		}
		double peak_value[100], peak_time[100];
		double trough_value[100], trough_time[100];
		int peak_time_num[100],trough_time_num[100];
		int peak_test=0, trough_test=0;
		double a1=0,a2=0,a3=0,a4=0;
		double interval_time=4.9;
		int interval=(int)(interval_time/sample_time)+1;
		int a_num=0, l_num=interval;
		double slope_test1=0,slope_test2=0;
		int change_flag=0;
		peak_value[0] = -100;
		peak_time[0] = -100;
		peak_time_num[0] = -100;
		trough_value[0] = -100;
		trough_time[0] = -100;
		trough_time_num[0] = -100;
		if(ini_ipeak>=2){
			for(Int_t j_peak=0;j_peak<ini_ipeak;j_peak++){
				if(peak_value_test-trough_value_1test<=WaveformCut||ChargeWF_Time[peak_time_test_num]-ChargeWF_Time[trough_time_test_num]<interval_time){
					if(j_peak==ini_ipeak-1) continue;
					change_flag=0;
					if(trough_value_1test>ini_trough_value[j_peak+1]+NoiseCut/5*1){
						change_flag=1;
						if((peak_test>=2||trough_test>=2)	&&
								(collect_charge-(trough_value_1test+ChargeWF_smooth[trough_time_test_num-1]+ChargeWF_smooth[trough_time_test_num+1])/3)<2*NoiseCut/5	&&
								(collect_charge-(ini_trough_value[j_peak+1]+ChargeWF_smooth[ini_trough_time_num[j_peak+1]-2]+ChargeWF_smooth[ini_trough_time_num[j_peak+1]])/3)<2*NoiseCut/5	&&
								(collect_charge-(peak_value_test+ChargeWF_smooth[peak_time_test_num-1]+ChargeWF_smooth[peak_time_test_num+1])/3)>-2*NoiseCut/5	&&
								(collect_charge-(ini_peak_value[j_peak+1]+ChargeWF_smooth[ini_peak_time_num[j_peak+1]-2]+ChargeWF_smooth[ini_peak_time_num[j_peak+1]])/3)>-2*NoiseCut/5	&&
								(collect_charge-(ini_trough_value[j_peak+1]+ChargeWF_smooth[ini_trough_time_num[j_peak+1]-2]+ChargeWF_smooth[ini_trough_time_num[j_peak+1]])/3)>-2*NoiseCut/5	&&
								(collect_charge-(trough_value_1test+ChargeWF_smooth[trough_time_test_num-1]+ChargeWF_smooth[trough_time_test_num+1])/3)>-2*NoiseCut/5	&&
								(collect_charge-(ini_peak_value[j_peak+1]+ChargeWF_smooth[ini_peak_time_num[j_peak+1]-2]+ChargeWF_smooth[ini_peak_time_num[j_peak+1]])/3)<2*NoiseCut/5	&&
								(collect_charge-(peak_value_test+ChargeWF_smooth[peak_time_test_num-1]+ChargeWF_smooth[peak_time_test_num+1])/3)<2*NoiseCut/5)	
							continue;
						trough_value_1test = ini_trough_value[j_peak+1];
						trough_time_test_num = ini_trough_time_num[j_peak+1]-1;
						peak_value_test = ini_peak_value[j_peak+1];
						peak_time_test_num = ini_peak_time_num[j_peak+1]-1;
						trough_test=0;
						peak_test=0;
					}
					else{
						trough_test++;
					}
					if(peak_value_test<ini_peak_value[j_peak+1]-NoiseCut/5*1||ChargeWF_Time[peak_time_test_num]-ChargeWF_Time[trough_time_test_num]<interval_time){
						change_flag=1;
						if((peak_test>=2||trough_test>=2)	&&
								(collect_charge-(trough_value_1test+ChargeWF_smooth[trough_time_test_num-1]+ChargeWF_smooth[trough_time_test_num+1])/3)<2*NoiseCut/5	&&
								(collect_charge-(ini_trough_value[j_peak+1]+ChargeWF_smooth[ini_trough_time_num[j_peak+1]-2]+ChargeWF_smooth[ini_trough_time_num[j_peak+1]])/3)<2*NoiseCut/5	&&
								(collect_charge-(peak_value_test+ChargeWF_smooth[peak_time_test_num-1]+ChargeWF_smooth[peak_time_test_num+1])/3)>-2*NoiseCut/5	&&
								(collect_charge-(ini_peak_value[j_peak+1]+ChargeWF_smooth[ini_peak_time_num[j_peak+1]-2]+ChargeWF_smooth[ini_peak_time_num[j_peak+1]])/3)>-2*NoiseCut/5	&&
								(collect_charge-(ini_trough_value[j_peak+1]+ChargeWF_smooth[ini_trough_time_num[j_peak+1]-2]+ChargeWF_smooth[ini_trough_time_num[j_peak+1]])/3)>-2*NoiseCut/5	&&
								(collect_charge-(peak_value_test+ChargeWF_smooth[peak_time_test_num-1]+ChargeWF_smooth[peak_time_test_num+1])/3)<2*NoiseCut/5)
							continue;
						peak_value_test = ini_peak_value[j_peak+1];
						peak_time_test_num = ini_peak_time_num[j_peak+1]-1;
						peak_test=0;
						trough_test=0;
					}
					else{
						peak_test++;
					}
					if(j_peak>=ini_ipeak-2){
						trough_value_2test=-collect_charge;
						trough_time_2test_num=WFLen-1;
					}
					else{
						trough_value_2test = ini_trough_value[j_peak+2];
						trough_time_2test_num = ini_trough_time_num[j_peak+2]-1;
					}
					continue;
				}
				else if(j_peak==ini_ipeak-1){
					peak_value[ipeak] = peak_value_test;
					peak_time[ipeak] = ChargeWF_Time[peak_time_test_num];
					peak_time_num[ipeak] = peak_time_test_num;
					trough_value[ipeak] = trough_value_1test;
					trough_time[ipeak] = ChargeWF_Time[trough_time_test_num];
					trough_time_num[ipeak] = trough_time_test_num;
					trough_value[ipeak+1] = -collect_charge;
					trough_time_num[ipeak+1] = WFLen-1;
					trough_time[ipeak+1] = ChargeWF_Time[WFLen-1];
				}
				else if(peak_value_test-trough_value_2test<=WaveformCut){
					change_flag=0;
					if(trough_value_1test>ini_trough_value[j_peak+1]+NoiseCut/5*1){
						change_flag=1;
						if((peak_test>=2||trough_test>=2)	&&
								(collect_charge-(trough_value_1test+ChargeWF_smooth[trough_time_test_num-1]+ChargeWF_smooth[trough_time_test_num+1])/3)<2*NoiseCut/5	&&
								(collect_charge-(ini_trough_value[j_peak+1]+ChargeWF_smooth[ini_trough_time_num[j_peak+1]-2]+ChargeWF_smooth[ini_trough_time_num[j_peak+1]])/3)<2*NoiseCut/5	&&
								(collect_charge-(peak_value_test+ChargeWF_smooth[peak_time_test_num-1]+ChargeWF_smooth[peak_time_test_num+1])/3)>-2*NoiseCut/5	&&
								(collect_charge-(ini_peak_value[j_peak+1]+ChargeWF_smooth[ini_peak_time_num[j_peak+1]-2]+ChargeWF_smooth[ini_peak_time_num[j_peak+1]])/3)>-2*NoiseCut/5	&&
								(collect_charge-(ini_trough_value[j_peak+1]+ChargeWF_smooth[ini_trough_time_num[j_peak+1]-2]+ChargeWF_smooth[ini_trough_time_num[j_peak+1]])/3)>-2*NoiseCut/5	&&
								(collect_charge-(peak_value_test+ChargeWF_smooth[peak_time_test_num-1]+ChargeWF_smooth[peak_time_test_num+1])/3)<2*NoiseCut/5)
							continue;
						trough_value_1test = ini_trough_value[j_peak+1];
						trough_time_test_num = ini_trough_time_num[j_peak+1]-1;
						peak_value_test = ini_peak_value[j_peak+1];
						peak_time_test_num = ini_peak_time_num[j_peak+1]-1;
						trough_test=0;
						peak_test=0;
					}
					else{
						trough=0;
					}
					if(peak_value_test<ini_peak_value[j_peak+1]-NoiseCut/5*1){
						change_flag=1;
						if((peak_test>=2||trough_test>=2)	&&
								(collect_charge-(trough_value_1test+ChargeWF_smooth[trough_time_test_num-1]+ChargeWF_smooth[trough_time_test_num+1])/3)<2*NoiseCut/5	&&
								(collect_charge-(ini_trough_value[j_peak+1]+ChargeWF_smooth[ini_trough_time_num[j_peak+1]-2]+ChargeWF_smooth[ini_trough_time_num[j_peak+1]])/3)<2*NoiseCut/5	&&
								(collect_charge-(peak_value_test+ChargeWF_smooth[peak_time_test_num-1]+ChargeWF_smooth[peak_time_test_num+1])/3)>-2*NoiseCut/5	&&
								(collect_charge-(ini_peak_value[j_peak+1]+ChargeWF_smooth[ini_peak_time_num[j_peak+1]-2]+ChargeWF_smooth[ini_peak_time_num[j_peak+1]])/3)>-2*NoiseCut/5	&&
								(collect_charge-(ini_trough_value[j_peak+1]+ChargeWF_smooth[ini_trough_time_num[j_peak+1]-2]+ChargeWF_smooth[ini_trough_time_num[j_peak+1]])/3)>-2*NoiseCut/5	&&
								(collect_charge-(peak_value_test+ChargeWF_smooth[peak_time_test_num-1]+ChargeWF_smooth[peak_time_test_num+1])/3)<2*NoiseCut/5)
							continue;
						peak_value_test = ini_peak_value[j_peak+1];
						peak_time_test_num = ini_peak_time_num[j_peak+1]-1;
						peak_test=0;
						trough_test=0;
					}
					else{
						peak_test++;
					}
					if(!change_flag&&!peak_test&&!trough_test){
						l_num=interval;
						if(WFLen-1-peak_time_test_num<interval)   l_num = WFLen-1-peak_time_test_num;
						a1=0;a2=0;a3=0;a4=0;
						a_num=0;
						for(Int_t l=0;l<l_num;l++){
							a1 = a1 + ChargeWF_Time[peak_time_test_num+l]*ChargeWF_smooth[peak_time_test_num+l];
							a2 = a2 + ChargeWF_Time[peak_time_test_num+l];
							a3 = a3 + ChargeWF_smooth[peak_time_test_num+l];
							a4 = a4 + ChargeWF_Time[peak_time_test_num+l]*ChargeWF_Time[peak_time_test_num+l];
							a_num++;
						}
						slope_test2=0;
						if(a_num>1){
							slope_test2 = (a1-a2*a3/a_num)/(a4-a2*a2/a_num);
						}
						if(collect_charge-trough_value_2test<WaveformCut&&slope_test2<0.5*NoiseCut/5&&ChargeWF_Time[peak_time_test_num]-ChargeWF_Time[trough_time_test_num]>interval_time){
							peak_value[ipeak] = peak_value_test;
							peak_time[ipeak] = ChargeWF_Time[peak_time_test_num];
							peak_time_num[ipeak] = peak_time_test_num;
							trough_value[ipeak] = trough_value_1test;
							trough_time[ipeak] = ChargeWF_Time[trough_time_test_num];
							trough_time_num[ipeak] = trough_time_test_num;

							trough_value[ipeak+1] = trough_value_2test;
							trough_time[ipeak+1] = ChargeWF_Time[trough_time_2test_num];
							trough_time_num[ipeak+1] = trough_time_2test_num;

							ipeak++;
							peak_test=0;
							trough_test=0;

							peak_value_test = ini_peak_value[j_peak+1];
							trough_value_1test = ini_trough_value[j_peak+1];
							peak_time_test_num = ini_peak_time_num[j_peak+1]-1;
							trough_time_test_num = ini_trough_time_num[j_peak+1]-1;

							if(j_peak<ini_ipeak-2){
								trough_value_2test = ini_trough_value[j_peak+2];
								trough_time_2test_num = ini_trough_time_num[j_peak+2]-1;
							}
							else{
								trough_value_2test = -collect_charge;
								trough_time_2test_num = WFLen-1;
							}
							continue;
						}
					}
					if(j_peak<ini_ipeak-2){
						trough_value_2test = ini_trough_value[j_peak+2];
						trough_time_2test_num = ini_trough_time_num[j_peak+2]-1;
					}
					else{
						trough_value_2test = -collect_charge;
						trough_time_2test_num = WFLen-1;
					}
					continue;
				}
				peak_value[ipeak] = peak_value_test;
				peak_time[ipeak] = ChargeWF_Time[peak_time_test_num];
				peak_time_num[ipeak] = peak_time_test_num;
				trough_value[ipeak] = trough_value_1test;
				trough_time[ipeak] = ChargeWF_Time[trough_time_test_num];
				trough_time_num[ipeak] = trough_time_test_num;

				trough_value[ipeak+1] = trough_value_2test;
				trough_time[ipeak+1] = ChargeWF_Time[trough_time_2test_num];
				trough_time_num[ipeak+1] = trough_time_2test_num;

				ipeak++;
				peak_test=0;
				trough_test=0;

				peak_value_test = ini_peak_value[j_peak+1];
				trough_value_1test = ini_trough_value[j_peak+1];
				peak_time_test_num = ini_peak_time_num[j_peak+1]-1;
				trough_time_test_num = ini_trough_time_num[j_peak+1]-1;
				if(j_peak<ini_ipeak-2){
					trough_value_2test = ini_trough_value[j_peak+2];
					trough_time_2test_num = ini_trough_time_num[j_peak+2]-1;
				}
				else{
					trough_value_2test = -collect_charge;
					trough_time_2test_num = WFLen-1;
				}
				continue;
			}
		}
		else if(ini_ipeak==1&&ini_peak_value[0]-ini_trough_value[0]>WaveformCut){
			ipeak=ini_ipeak;
			peak_time[0] = ChargeWF_Time[ini_peak_time_num[0]-1];
			peak_value[0] = ini_peak_value[0];
			peak_time_num[0] = ini_peak_time_num[0]-1;
			trough_value[0] = ini_trough_value[0];
			trough_time_num[0] = ini_trough_time_num[0]-1;
			trough_time[0] = ChargeWF_Time[ini_trough_time_num[0]-1];
			trough_value[1] = -collect_charge;
			trough_time_num[1] = WFLen-1;
			trough_time[1] = ChargeWF_Time[WFLen-1];
		}
		else if(ini_ipeak==0){
			ipeak=0;
			peak_time[0] = 0;
			peak_value[0] = 0;
			peak_time_num[0] = 0;
			trough_value[0] = 0;
			trough_time_num[0] = 0;
			trough_time[0] = 0;
			trough_value[1] = -collect_charge;
			trough_time_num[1] = WFLen-1;
			trough_time[1] = ChargeWF_Time[WFLen-1];
		}
		if(ipeak>0){
			if(collect_charge-trough_value[ipeak-1]>3*NoiseCut/5){
				int k1=trough_time_num[ipeak-1], k2=trough_time_num[ipeak];
				for(Int_t iWFLen=k1; iWFLen<WFLen; iWFLen++){
					if(collect_charge<ChargeWF_smooth[iWFLen]+Ampl_max*0.05){
						iWFLen++;
						if(iWFLen>k2){
							if(ChargeWF_Time[iWFLen]-ChargeWF_Time[trough_time_num[ipeak]]<interval_time){
								peak_value[ipeak-1] = ChargeWF_smooth[iWFLen];
								peak_time_num[ipeak-1] = iWFLen;
								peak_time[ipeak-1] = ChargeWF_Time[iWFLen];
								trough_value[ipeak] = collect_charge;
								trough_time[ipeak] = ChargeWF_Time[WFLen-1];
								trough_time_num[ipeak] = WFLen-1;
							}
							else{
								a1=0;a2=0;a3=0;a4=0;
								a_num=0;
								for(Int_t l=0;l<interval;l++){
									if(iWFLen-l<0) continue;
									a1 = a1 + ChargeWF_Time[iWFLen-l]*ChargeWF_smooth[iWFLen-l];
									a2 = a2 + ChargeWF_Time[iWFLen-l];
									a3 = a3 + ChargeWF_smooth[iWFLen-l];
									a4 = a4 + ChargeWF_Time[iWFLen-l]*ChargeWF_Time[iWFLen-l];
									a_num++;
								}
								slope_test1=0;
								if(a_num>1){
									slope_test1 = (a1-a2*a3/a_num)/(a4-a2*a2/a_num);
								}
								if(slope_test1>0.5*NoiseCut/5){
									peak_value[ipeak] = ChargeWF_smooth[iWFLen];
									peak_time_num[ipeak] = iWFLen;
									peak_time[ipeak] = ChargeWF_Time[iWFLen];
									trough_value[ipeak+1] = collect_charge;
									trough_time[ipeak+1] = ChargeWF_Time[WFLen-1];
									trough_time_num[ipeak+1] = WFLen-1;
									ipeak++;
								}
								else{
									peak_value[ipeak-1] = ChargeWF_smooth[iWFLen];
									peak_time_num[ipeak-1] = iWFLen;
									peak_time[ipeak-1] = ChargeWF_Time[iWFLen];
									trough_value[ipeak] = collect_charge;
									trough_time[ipeak] = ChargeWF_Time[WFLen-1];
									trough_time_num[ipeak] = WFLen-1;
								}
							}
						}
						else{
							peak_value[ipeak-1] = ChargeWF_smooth[iWFLen];
							peak_time_num[ipeak-1] = iWFLen;
							peak_time[ipeak-1] = ChargeWF_Time[iWFLen];
						}
						break;
					}
				}
			}
			for(Int_t i_peak=0;i_peak<ipeak;i_peak++){
				if(collect_charge-trough_value[i_peak]>NoiseCut&&collect_charge<peak_value[i_peak]+0.1){
					int more_peak_number=trough_time_num[i_peak];
					while(ChargeWF_smooth[more_peak_number]<collect_charge&&more_peak_number<peak_time_num[i_peak]){more_peak_number++;}
					if(more_peak_number>=peak_time_num[i_peak])  continue;
					more_peak_number--;
					if(ChargeWF_Time[peak_time_num[i_peak]]-ChargeWF_Time[more_peak_number]>interval_time){
						double slope1, slope2;
						double b1=0,b2=0,b3=0,b4=0;
						double c1=0,c2=0,c3=0,c4=0;
						a_num=0;
						for(Int_t l=0;l<interval;l++){
							if(more_peak_number-l<0)   continue;
							b1 = b1 + ChargeWF_Time[more_peak_number-l]*ChargeWF_smooth[more_peak_number-l];
							b2 = b2 + ChargeWF_Time[more_peak_number-l];
							b3 = b3 + ChargeWF_smooth[more_peak_number-l];
							b4 = b4 + ChargeWF_Time[more_peak_number-l]*ChargeWF_Time[more_peak_number-l];

							c1 = c1 + ChargeWF_Time[more_peak_number+l]*ChargeWF_smooth[more_peak_number+l];
							c2 = c2 + ChargeWF_Time[more_peak_number+l];
							c3 = c3 + ChargeWF_smooth[more_peak_number+l];
							c4 = c4 + ChargeWF_Time[more_peak_number+l]*ChargeWF_Time[more_peak_number+l];
							a_num++;
						}
						if(a_num<2){slope1=-1;slope2=-1;}
						else{
							slope1 = (b1 - b2*b3/a_num)/(b4 - b2*b2/a_num);
							slope2 = (c1 - c2*c3/a_num)/(c4 - c2*c2/a_num);
						}
						if(slope1>0&&slope2>0&&slope2/slope1<0.067){
							trough_value[ipeak+1] = trough_value[ipeak];
							trough_time[ipeak+1] = trough_time[ipeak];
							trough_time_num[ipeak+1] = trough_time_num[ipeak];
							for(Int_t l_peak=ipeak-1; l_peak>i_peak; l_peak--){
								peak_value[l_peak+1] = peak_value[l_peak];
								peak_time[l_peak+1] = peak_time[l_peak];
								peak_time_num[l_peak+1] = peak_time_num[l_peak];
								trough_value[l_peak+1] = trough_value[l_peak];
								trough_time[l_peak+1] = trough_time[l_peak];
								trough_time_num[l_peak+1] = trough_time_num[l_peak];
							}
							peak_value[i_peak+1] = peak_value[i_peak];
							peak_time[i_peak+1] = peak_time[i_peak];
							peak_time_num[i_peak+1] = peak_time_num[i_peak];
							trough_value[i_peak+1] = ChargeWF_smooth[more_peak_number+1];
							trough_time[i_peak+1] = ChargeWF_Time[more_peak_number+1];
							trough_time_num[i_peak+1] = more_peak_number+1;

							peak_value[i_peak] = ChargeWF_smooth[more_peak_number];
							peak_time[i_peak] = ChargeWF_Time[more_peak_number];
							peak_time_num[i_peak] = more_peak_number;
							ipeak++;
						}
					}
				}
			}
		}
		double collection_time = -100;
		int ls_peak = ipeak-1;
		double Coltime = -100;
		if(collect_charge>NoiseCut){
			if(ipeak>0){
				double a_former,b_former, a_after;//b_after;
				double mx_former[8],my_former[8], mx_after[8],my_after[8];
				for(Int_t i_peak=0; i_peak<ipeak; i_peak++){
					if(collect_charge<ChargeWF_smooth[peak_time_num[i_peak]]+Ampl_max*0.05+NoiseCut/5&&collect_charge<ChargeWF_smooth[trough_time_num[i_peak+1]]+NoiseCut/5+Ampl_max*0.05){
						ls_peak=i_peak;
						break;
					}
				}
				mx_former[0] = ChargeWF_Time[peak_time_num[ls_peak]];
				my_former[0] = ChargeWF_smooth[peak_time_num[ls_peak]];
				mx_former[1] = ChargeWF_Time[peak_time_num[ls_peak]-1];
				my_former[1] = ChargeWF_smooth[peak_time_num[ls_peak]-1];
				mx_former[2] = ChargeWF_Time[peak_time_num[ls_peak]-3];
				my_former[2] = ChargeWF_smooth[peak_time_num[ls_peak]-3];
				mx_former[3] = ChargeWF_Time[peak_time_num[ls_peak]-5];
				my_former[3] = ChargeWF_smooth[peak_time_num[ls_peak]-5];
				mx_former[4] = ChargeWF_Time[peak_time_num[ls_peak]-7];
				my_former[4] = ChargeWF_smooth[peak_time_num[ls_peak]-7];
				mx_former[5] = ChargeWF_Time[peak_time_num[ls_peak]-9];
				my_former[5] = ChargeWF_smooth[peak_time_num[ls_peak]-9];
				mx_after[0] = ChargeWF_Time[peak_time_num[ls_peak]];
				my_after[0] = ChargeWF_smooth[peak_time_num[ls_peak]];
				mx_after[1] = ChargeWF_Time[peak_time_num[ls_peak]+1];
				my_after[1] = ChargeWF_smooth[peak_time_num[ls_peak]+1];
				mx_after[2] = ChargeWF_Time[peak_time_num[ls_peak]+3];
				my_after[2] = ChargeWF_smooth[peak_time_num[ls_peak]+3];
				mx_after[3] = ChargeWF_Time[peak_time_num[ls_peak]+5];
				my_after[3] = ChargeWF_smooth[peak_time_num[ls_peak]+5];

				a_former = ((my_former[1]+my_former[2]+my_former[3]+my_former[4]+my_former[5])/5.0 - my_former[0])/((mx_former[1]+mx_former[2]+mx_former[3]+mx_former[4]+mx_former[5])/5.0 - mx_former[0]);
				b_former = my_former[0] - a_former*mx_former[0];
				a_after = ((my_after[1]+my_after[2]+my_after[3]+my_after[4]+my_after[5])/5.0 - my_after[0])/((mx_after[1]+mx_after[2]+mx_after[3]+mx_after[4]+mx_after[5])/5.0 - mx_after[0]);
				//b_after = my_after[0] - a_after*mx_after[0];

				for(Int_t i_peak=0;i_peak<ipeak;i_peak++){
					if(trough_value[i_peak+1]>-0.0001){
						double collect_charge_test2 = (ChargeWF_smooth[trough_time_num[i_peak+1]-2]+ChargeWF_smooth[trough_time_num[i_peak+1]-1]+ChargeWF_smooth[trough_time_num[i_peak+1]]+ChargeWF_smooth[trough_time_num[i_peak+1]+1]+ChargeWF_smooth[trough_time_num[i_peak+1]+2])/5;
						if(collect_charge_test2-collect_charge>-(Ampl_max*0.05+2*NoiseCut/5)&&collect_charge<ChargeWF_smooth[peak_time_num[i_peak]]){
							Coltime = ChargeWF_Time[trough_time_num[i_peak+1]];
							break;
						}
					}
				}

				if(a_former<0.5*NoiseCut/5||a_after>-0.5*NoiseCut/5){
					if(Coltime<0||(Coltime>0&&Coltime>ChargeWF_Time[peak_time_num[ls_peak]]))	Coltime = ChargeWF_Time[peak_time_num[ls_peak]];
				}
				else if(collect_charge>ChargeWF_smooth[peak_time_num[ls_peak]]){
					if(Coltime<(collect_charge - b_former)/a_former)	Coltime = (collect_charge - b_former)/a_former;
				}
				else{
					if(fabs(a_former/a_after)>10){
						if(Coltime<0||(Coltime>0&&Coltime>ChargeWF_Time[peak_time_num[ls_peak]]))	Coltime = ChargeWF_Time[peak_time_num[ls_peak]];
					}
					else{
						if(Coltime<0||(Coltime>0&&Coltime>(collect_charge - b_former)/a_former))	Coltime = (collect_charge - b_former)/a_former;
					}
				}
			}
			collection_time=Coltime;
			TotColTime = (TotColTime*TotCharge+collection_time*collect_charge) / (TotCharge+collect_charge);
			TotColNum++;
			TotCharge = TotCharge + collect_charge;
			if(ipeakmax_event<ipeak&&ipeak>0)   ipeakmax_event = ipeak;
			
			
			XTile[tilenum] = channel.xTile();
			YTile[tilenum] = channel.yTile();
			tilenum++;
			if(channel.ChannelLocalId()<PadNumOnTile){
				xposition[numxchannel] = channel.XPosition();
				yptile[numxchannel] = channel.YPosition();
				xchanneltime[numxchannel] = collection_time;
				xchannelcharge[numxchannel] = collect_charge;
				numxchannel++;
			}
			else if(channel.ChannelLocalId()>=PadNumOnTile){
				yposition[numychannel] = channel.YPosition();
				xptile[numychannel] = channel.XPosition();
				ychanneltime[numychannel] = collection_time;
				ychannelcharge[numychannel] = collect_charge;
				numychannel++;
			}
		}
	}
	
	//fill false event(no channel collected charge)
	if(numxchannel + numychannel < 1){
        SniperDataPtr<nEXO::NavBuffer>	navBuf("/Event");
		nEXO::EvtNavigator* nav = navBuf->curEvt();
		nEXO::PidTmvaHeader* tmva_header = new nEXO::PidTmvaHeader();
		nav->addHeader(tmva_header);
		nEXO::PidTmvaEvent* tmva_event = new nEXO::PidTmvaEvent();
		
		tmva_header->setEvent(tmva_event);
		tmva_header->set_type(event_type);
		tmva_header->set_edge_distance(0);
		tmva_header->set_r_distance(0);
		tmva_header->set_z_distance(0);
		tmva_header->set_dx_max(0);
		tmva_header->set_dy_max(0);
		tmva_header->set_charge(0);
		tmva_header->set_drift_time(0);
		tmva_header->set_channel_num(0);
		tmva_header->set_Xchannel_num(0);
		tmva_header->set_Ychannel_num(0);
		tmva_header->set_peak_num(0);
		tmva_header->set_rising_time(0);
		tmva_header->set_2rising_time(0);
		tmva_header->set_mean_time(-100);
		tmva_header->set_TotalEnergy(TotE);
		tmva_header->set_value(-999);
		tmva_header->set_value_err(-999);
		tmva_header->set_signal(-999);
		tmva_header->set_cutvalue(-999);
		tmva_header->set_outside(true);
		tmva_header->set_mcchannelcharge(mcchannelcharge);
        tmva_header->set_reconchannelcharge(reconchannelcharge);
        return true;
	}
	
    fNumChannels = numxchannel+numychannel; 
	fXNumChannels = numxchannel;
	fYNumChannels = numychannel;
	
    //calculate standoff variables
	double z = 0;
	int Znum=0;
	double dZ_min = 9999, dR_min = 9999, Edge_Distance = 9999;
	double r;
	while(charge_platform*0.99>TotalAmpl_smooth[Znum]){
		Znum++;
	}
	z = 1.8958*TotalTime_smooth[Znum]; //Volt = 380V/cm
	dZ_min = (z<fabs(1300-z))?z:fabs(1300-z);
	for(int i=0; i<tilenum; i++){
		r = fabs(650 - sqrt(XTile[i]*XTile[i]+YTile[i]*YTile[i]));
		if(dR_min>r)
			dR_min=r;
	}
	Edge_Distance = (dZ_min<dR_min)?dZ_min:dR_min;

	total_delta_time = EventRiseTime(TotalAmpl_smooth, SamplingFrequency);//total_peak_time - total_trough_time;
	drift_time = drift_time/total_charge;
	
    //Define variables to store the information of hit pads (position and charge) into vector
    std::vector<double> fxposition;
    std::vector<double> fxcharge;
    std::vector<double> fxtime;
    std::vector<double> fyposition;
    std::vector<double> fycharge;
    std::vector<double> fytime;
	for(Int_t i=0;i<numxchannel-1;i++){
        for(Int_t j=i;j<numxchannel;j++){
			if(dx_max<fabs(xposition[i]-xposition[j])+PadSize)
				dx_max=fabs(xposition[i]-xposition[j])+PadSize;
		}
	}
	for(Int_t i=0;i<numychannel-1;i++){
        for(Int_t j=i;j<numychannel;j++){
			if(dy_max<fabs(yposition[i]-yposition[j])+PadSize)
				dy_max=fabs(yposition[i]-yposition[j])+PadSize;
		}
	}
    //store the information of hit pads into vector
    for(Int_t i = 0; i < numxchannel; i++)
    {
        fxposition.push_back(xposition[i]);
        fxcharge.push_back(xchannelcharge[i]);
        fxtime.push_back(xchanneltime[i]);
    }
    for(Int_t i = 0; i < numychannel; i++)
    {
        fyposition.push_back(yposition[i]);
        fycharge.push_back(ychannelcharge[i]);
        fytime.push_back(ychanneltime[i]);
    }
	
    SniperDataPtr<nEXO::NavBuffer>	navBuf("/Event");
	nEXO::EvtNavigator* nav = navBuf->curEvt();
	nEXO::PidTmvaHeader* tmva_header = new nEXO::PidTmvaHeader();
	nav->addHeader(tmva_header);
	nEXO::PidTmvaEvent* tmva_event = new nEXO::PidTmvaEvent();
	
	tmva_header->setEvent(tmva_event);
	tmva_header->set_type(event_type);
	tmva_header->set_edge_distance(Edge_Distance);
	tmva_header->set_r_distance(dR_min);
	tmva_header->set_z_distance(dZ_min);
	tmva_header->set_dx_max(dx_max);
	tmva_header->set_dy_max(dy_max);
	tmva_header->set_drift_time(drift_time);
    tmva_header->set_charge(TotCharge);
	tmva_header->set_channel_num(fNumChannels);
	tmva_header->set_Xchannel_num(fXNumChannels);
	tmva_header->set_Ychannel_num(fYNumChannels);
	tmva_header->set_peak_num(ipeakmax_event);
	tmva_header->set_rising_time(total_delta_time);
	tmva_header->set_2rising_time(total_2delta_time);
	tmva_header->set_mean_time(TotColTime);
	tmva_header->set_TotalEnergy(TotE);
	tmva_header->set_outside(false);
	tmva_header->set_value(-999);
	tmva_header->set_value_err(-999);
	tmva_header->set_signal(-999);
	tmva_header->set_cutvalue(-999);
    tmva_header->set_xposition(fxposition);
    tmva_header->set_xcharge(fxcharge);
    tmva_header->set_xtime(fxtime);
    tmva_header->set_yposition(fyposition);
    tmva_header->set_ycharge(fycharge);
    tmva_header->set_ytime(fytime);
	tmva_header->set_mcchannelcharge(mcchannelcharge);
    tmva_header->set_reconchannelcharge(reconchannelcharge);
	for(int i=0; i<numxchannel; i++){
		if(xposition[i]<-200||xposition[i]>200)	tmva_header->set_outside(true);
	}
	for(int i=0; i<numychannel; i++){
		if(yposition[i]<-200||yposition[i]>200)	tmva_header->set_outside(true);
	}

	return true;
}

double ChargeReconAlg::ChannelRiseTime(std::vector<double>& fWaveform, double frequency)
{
    //algorithm to calculate the rising time of each electronics channel.
    //The logic is to (1)first search for a peak in the waveform, then compare with the collected
    //charge in the channel to examine whether the channel has induced signal or collected charge
    //(2) search for 40% of peak position and 90% of peak position, then use the time difference
    //as a rising time.
    //Peak charge search
    double peak_charge = 0;
    std::vector<double>::iterator peak_loc;
    for(std::vector<double>::iterator iter = fWaveform.begin(); iter < fWaveform.end(); iter++)
    {
        if(*iter>peak_charge) 
        {
            peak_charge = *iter;
            peak_loc = iter;
        }
    }
    std::cout << peak_charge << " " << std::endl;
    if(peak_charge < EquNoise*2) return 0;
    //search for 40% and 90% of peak height positions.
    std::vector<double>::iterator rise_lower;
    std::vector<double>::iterator rise_upper;
    for(std::vector<double>::iterator iter = peak_loc; iter > fWaveform.begin()+1; iter--)
    {
        //std::cout << *iter << " " ;
        if(*iter >= 0.4*peak_charge && *(iter+1) > 0.4*peak_charge &&*(iter-1) <= 0.4*peak_charge)
        {
            rise_lower = iter;
            break;
        }
        if(*iter >= 0.9*peak_charge && *(iter-1) < 0.9*peak_charge )
            rise_upper = iter;
    }
    //calculate rise time as the lapse between upper edge and lower edge.
    return (rise_upper - rise_lower)/frequency;
}

double ChargeReconAlg::EventRiseTime(std::vector<double>& fWaveform, double frequency)
{
	//calculate total waveform's rising time
	//Calculate an average of charge in event for last 10 microsecond of total waveform.
    int ave_bin_num = 10*(int)frequency;
    double charge_platform = 0;
	for(Int_t i_time=1; i_time <= ave_bin_num; i_time++){
		charge_platform += fWaveform[fWaveform.size()-i_time];
	}
	charge_platform = charge_platform/ave_bin_num;
	//search for lower edge of waveform that pass threshold of 10% of total charge.
    double total_peak_time = 0, total_lowedge_time = 0;
	for(Int_t i_time=fWaveform.size()-1;i_time>=0;i_time--){
		if(charge_platform*0.4>=fWaveform[i_time]){
			total_lowedge_time = i_time; 
			break;
		}
	}
    //search for upper edge of rise that pass threshold of 99% of total charge.
	for(Int_t i_time=0;i_time<fWaveform.size();i_time++){
		if(charge_platform*0.99<=fWaveform[i_time]){
			total_peak_time = i_time;
			break;
		}
	}
 
    return (total_peak_time - total_lowedge_time)/frequency;
}

double ChargeReconAlg::FreqChargeRecon(std::vector<double>& fWaveform, double frequency)
{
	Double_t* in = new Double_t[fWaveform.size()];
	for(int i =0; i < fWaveform.size(); i++)
		in[i] = fWaveform[i];
	//FFT transform time domain to frequency domain and apply filter
	int fftsize = fWaveform.size() ;
	//FFT transformation of real to complex (frequency)
	TVirtualFFT* fftr2c = TVirtualFFT::FFT(1, &fftsize, "R2C M K");
	fftr2c->SetPoints(in);
	fftr2c->Transform();
	Double_t* re_full = new Double_t[fftsize];
	Double_t* im_full = new Double_t[fftsize];
    std::cout << "Freq Domain" << std::endl;
    for(unsigned int i = 0; i < fftsize; i++)
	{
		double re_local, im_local;
		fftr2c->GetPointComplex(i, re_local, im_local);
        std::cout << re_local << " " << im_local << std::endl;
    }
    return 0;
}

void ChargeReconAlg::BesselFilter(std::vector<double>& UnFiltered, std::vector<double>& Filtered)
{
	Double_t* in = new Double_t[UnFiltered.size()];
	for(int i =0; i < UnFiltered.size(); i++)
		in[i] = UnFiltered[i];
	//Bessel Filter s = w/w_0
	//H = 1/(0.61*s+0.9264)/((0.61*s)*(0.61*s) + 0.61*1.703*s + 0.9211)/((0.61*s)*(0.61*s) + 0.61*1.181*s + 1.172);
	double tau = 1/(2*TMath::Pi()*80.);
	TF1* bessel = new TF1("bessel", "1/(0.61*x+0.9264)/((0.61*x)^2 + 0.61*1.703*x + 0.9211)/((0.61*x)^2 + 0.61*1.181*x + 1.172)", 0, 500);
	//FFT transform time domain to frequency domain and apply filter
	int fftsize = UnFiltered.size() ;
	//FFT transformation of real to complex (frequency)
	TVirtualFFT* fftr2c = TVirtualFFT::FFT(1, &fftsize, "R2C M K");
	fftr2c->SetPoints(in);
	fftr2c->Transform();
	double binWidth = 2000./double(fftsize);//Factor to scale bin number to real frequency
	Double_t* re_full = new Double_t[fftsize];
	Double_t* im_full = new Double_t[fftsize];
	for(unsigned int i = 0; i < fftsize; i++)
	{
		double re_local, im_local;
		fftr2c->GetPointComplex(i, re_local, im_local);
		double s = 2*TMath::Pi()*tau*i*binWidth;// w/w_0
		//Apply the Bessel filter
		re_full[i] = re_local*bessel->Eval(s);
		im_full[i] = im_local*bessel->Eval(s);
	}
	TVirtualFFT* fftc2r = TVirtualFFT::FFT(1, &fftsize, "C2R M K");
	fftc2r->SetPointsComplex(re_full, im_full);
	fftc2r->Transform();
	//Fill the filtered waveform with FFT backward transformation.
	Filtered.clear();
	for(unsigned int i = 0; i < fftsize; i++)
		Filtered.push_back(fftc2r->GetPointReal(i, false)/double(fftsize));//scale the waveform amplitude by 1/fftsize after two FFT transformation.
	delete fftr2c;
	delete fftc2r;
	delete bessel;
	delete in;
	delete re_full;
	delete im_full;
}

bool ChargeReconAlg::finalize()
{
	return true;
}
