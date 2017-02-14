#ifndef CAMERA_ANALYZER_H
#define CAMERA_ANALYZER_H

#include <Analyzer.h>
#include "CameraAnalyzerResults.h"
#include "CameraSimulationDataGenerator.h"

class CameraAnalyzerSettings;
class ANALYZER_EXPORT CameraAnalyzer : public Analyzer2
{
public:
	CameraAnalyzer();
	virtual ~CameraAnalyzer();
	virtual void SetupResults();
	virtual void WorkerThread();

	virtual U32 GenerateSimulationData( U64 newest_sample_requested, U32 sample_rate, SimulationChannelDescriptor** simulation_channels );
	virtual U32 GetMinimumSampleRateHz();

	virtual const char* GetAnalyzerName() const;
	virtual bool NeedsRerun();
	


#pragma warning( push )
#pragma warning( disable : 4251 ) //warning C4251: 'CameraAnalyzer::<...>' : class <...> needs to have dll-interface to be used by clients of class

protected: //functions
	void ComputeSampleOffsets();
	U64 AdvanceClock();
	void AdvanceAll(U64 SampleNumber);

protected: //vars
	std::auto_ptr< CameraAnalyzerSettings > mSettings;
	std::auto_ptr< CameraAnalyzerResults > mResults;
	AnalyzerChannelData* mSerial;

	AnalyzerChannelData* VSyncData;
	AnalyzerChannelData* HSyncData;
	AnalyzerChannelData* PCKData;
	AnalyzerChannelData* D0Data;
	AnalyzerChannelData* D1Data;
	AnalyzerChannelData* D2Data;
	AnalyzerChannelData* D3Data;
	AnalyzerChannelData* D4Data;
	AnalyzerChannelData* D5Data;
	AnalyzerChannelData* D6Data;
	AnalyzerChannelData* D7Data;

	CameraSimulationDataGenerator mSimulationDataGenerator;
	bool mSimulationInitilized;
	
	//Serial analysis vars:
	U32 mSampleRateHz;
	std::vector<U32> mSampleOffsets;
	U32 mParityBitOffset;
	U32 mStartOfStopBitOffset;
	U32 mEndOfStopBitOffset;
	BitState mBitLow;
	BitState mBitHigh;

#pragma warning( pop )
};

extern "C" ANALYZER_EXPORT const char* __cdecl GetAnalyzerName();
extern "C" ANALYZER_EXPORT Analyzer* __cdecl CreateAnalyzer( );
extern "C" ANALYZER_EXPORT void __cdecl DestroyAnalyzer( Analyzer* analyzer );

#endif //CAMERA_ANALYZER_H
