#ifndef CAMERA_SIMULATION_DATA_GENERATOR
#define CAMERA_SIMULATION_DATA_GENERATOR

#include <AnalyzerHelpers.h>

class CameraAnalyzerSettings;

class CameraSimulationDataGenerator
{
public:
	CameraSimulationDataGenerator();
	~CameraSimulationDataGenerator();

	void Initialize( U32 simulation_sample_rate, CameraAnalyzerSettings* settings );
	U32 GenerateSimulationData( U64 newest_sample_requested, U32 sample_rate, SimulationChannelDescriptor** simulation_channelss );

protected:
	CameraAnalyzerSettings* mSettings;
	U32 mSimulationSampleRateHz;
	BitState mBitLow;
	BitState mBitHigh;
	U64 mValue;

	U64 mMpModeAddressMask;
	U64 mMpModeDataMask;
	U64 mNumBitsMask;

	

protected: //Serial specific

	void CreateSerialByte( U64 value );
	void ClockPulse(U64 value);
	ClockGenerator mClockGenerator;
	SimulationChannelDescriptorGroup mSimulationGroup;
	SimulationChannelDescriptor* VSyncSimulationData;
	SimulationChannelDescriptor* HSyncSimulationData;  //if we had more than one channel to simulate, they would need to be in an array
	SimulationChannelDescriptor* PCKSimulationData;
	SimulationChannelDescriptor* D0SimulationData;
	SimulationChannelDescriptor* D1SimulationData;
	SimulationChannelDescriptor* D2SimulationData;
	SimulationChannelDescriptor* D3SimulationData;
	SimulationChannelDescriptor* D4SimulationData;
	SimulationChannelDescriptor* D5SimulationData;
	SimulationChannelDescriptor* D6SimulationData;
	SimulationChannelDescriptor* D7SimulationData;
};
#endif //UNIO_SIMULATION_DATA_GENERATOR