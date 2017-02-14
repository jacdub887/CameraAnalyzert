#include "CameraSimulationDataGenerator.h"
#include "CameraAnalyzerSettings.h"

CameraSimulationDataGenerator::CameraSimulationDataGenerator()
{
}

CameraSimulationDataGenerator::~CameraSimulationDataGenerator()
{
}

void CameraSimulationDataGenerator::Initialize( U32 simulation_sample_rate, CameraAnalyzerSettings* settings )
{
	mSimulationSampleRateHz = simulation_sample_rate;
	mSettings = settings;

	mBitLow = BIT_LOW;
	mBitHigh = BIT_HIGH;
	mSettings->PCKHz = 4000000;
	mSettings->width = 320;
	mSettings->height = 240;
	mSettings->frames = 4;
	mSettings->dataChannels = 8;

	VSyncSimulationData = mSimulationGroup.Add(mSettings->VSyncChannel, simulation_sample_rate, BIT_LOW);
	HSyncSimulationData = mSimulationGroup.Add(mSettings->HSyncChannel, simulation_sample_rate, BIT_LOW);
	PCKSimulationData = mSimulationGroup.Add(mSettings->PCKChannel, simulation_sample_rate, BIT_HIGH);
	D0SimulationData = mSimulationGroup.Add(mSettings->D0Channel, simulation_sample_rate, BIT_LOW);
	D1SimulationData = mSimulationGroup.Add(mSettings->D1Channel, simulation_sample_rate, BIT_LOW);
	D2SimulationData = mSimulationGroup.Add(mSettings->D2Channel, simulation_sample_rate, BIT_LOW);
	D3SimulationData = mSimulationGroup.Add(mSettings->D3Channel, simulation_sample_rate, BIT_LOW);
	D4SimulationData = mSimulationGroup.Add(mSettings->D4Channel, simulation_sample_rate, BIT_LOW);
	D5SimulationData = mSimulationGroup.Add(mSettings->D5Channel, simulation_sample_rate, BIT_LOW);
	D6SimulationData = mSimulationGroup.Add(mSettings->D6Channel, simulation_sample_rate, BIT_LOW);
	D7SimulationData = mSimulationGroup.Add(mSettings->D7Channel, simulation_sample_rate, BIT_LOW);

	mClockGenerator.Init(mSettings->PCKHz, simulation_sample_rate);
	//mClockGenerator.Init( mSettings->mBitRate, simulation_sample_rate );
	
	//HSyncSimulationData->Advance( mClockGenerator.AdvanceByTimeS( 0.001 ) );  //insert 10 bit-periods of idle
	//VSyncSimulationData->Advance(mClockGenerator.AdvanceByTimeS(0.001));
	mSimulationGroup.AdvanceAll(mClockGenerator.AdvanceByTimeS(0.0001));

	mValue = 0;

	mMpModeAddressMask = 0;
	mMpModeDataMask = 0;
	mNumBitsMask = 0;

	U32 num_bits = mSettings->mBitsPerTransfer;
	for( U32 i = 0; i < num_bits; i++ )
	{
		mNumBitsMask <<= 1;
		mNumBitsMask |= 0x1;
	}

	if( mSettings->mSerialMode == CameraAnalyzerEnums::MpModeMsbOneMeansAddress )
		mMpModeAddressMask = 0x1 << ( mSettings->mBitsPerTransfer );

	if( mSettings->mSerialMode == CameraAnalyzerEnums::MpModeMsbZeroMeansAddress )
		mMpModeDataMask = 0x1 << ( mSettings->mBitsPerTransfer );
}

U32 CameraSimulationDataGenerator::GenerateSimulationData( U64 largest_sample_requested, U32 sample_rate, SimulationChannelDescriptor** simulation_channels )
{
	U64 adjusted_largest_sample_requested = AnalyzerHelpers::AdjustSimulationTargetSample( largest_sample_requested, sample_rate, mSimulationSampleRateHz );

	U32 data[] = { 0x1,0x2,0x4,0x8,0x10,0x20,0x40,0x80 };
	U32 data_counter = 0;
	//BitExtractor bit_extractor(1, AnalyzerEnums::ShiftOrder::LsbFirst, 8);
	
	for (U32 i = 0; i < mSettings->frames; i++)
	{
		ClockPulse(80);
		VSyncSimulationData->TransitionIfNeeded(mBitHigh);  //start frame
		ClockPulse(20);
		VSyncSimulationData->Transition();
		ClockPulse(100);
		for (U32 i = 0; i < mSettings->height; i++)  //hsync line 240
		{
			HSyncSimulationData->TransitionIfNeeded(mBitHigh);
			for (U32 y = 0; y < mSettings->width; y++)  //width data in single line 320
			{
				BitExtractor bit_extractor(data[data_counter++], AnalyzerEnums::ShiftOrder::LsbFirst, 8);
				if (data_counter == 8) data_counter = 0;
				for (U32 z = 3; z < 11; z++)  //1 byte/clock period
				{
					mSimulationGroup.GetArray()[z].TransitionIfNeeded(bit_extractor.GetNextBit());
				}
				ClockPulse(1);
			}
			HSyncSimulationData->TransitionIfNeeded(mBitLow);
			D0SimulationData->TransitionIfNeeded(mBitLow);
			D1SimulationData->TransitionIfNeeded(mBitLow);
			D2SimulationData->TransitionIfNeeded(mBitLow);
			D3SimulationData->TransitionIfNeeded(mBitLow);
			D4SimulationData->TransitionIfNeeded(mBitLow);
			D5SimulationData->TransitionIfNeeded(mBitLow);
			D6SimulationData->TransitionIfNeeded(mBitLow);
			D7SimulationData->TransitionIfNeeded(mBitLow);
			ClockPulse(50);
		}
	}

	/**
	while(VSyncSimulationData->GetCurrentSampleNumber() < adjusted_largest_sample_requested )
	{
		VSyncSimulationData->Transition();
		VSyncSimulationData->Advance(mClockGenerator.AdvanceByHalfPeriod());
		VSyncSimulationData->Transition();
		VSyncSimulationData->Advance(mClockGenerator.AdvanceByHalfPeriod());
			//CreateSerialByte( mValue++ );
			//mSimulationGroup.AdvanceAll( mClockGenerator.AdvanceByTimeS( 0.001 ) );  //insert 10 bit-periods of idle
	}
	*/


	*simulation_channels = mSimulationGroup.GetArray();
	return mSimulationGroup.GetCount();  // we are retuning the size of the SimulationChannelDescriptor array.  In our case, the "array" is length 1.
}

void CameraSimulationDataGenerator::ClockPulse(U64 value)
{
	for (U32 i = 0; i < value; i++)
	{
		PCKSimulationData->Transition();
		//PCKSimulationData->Advance(mClockGenerator.AdvanceByHalfPeriod());
		mSimulationGroup.AdvanceAll(mClockGenerator.AdvanceByHalfPeriod());
		PCKSimulationData->Transition();
		//PCKSimulationData->Advance(mClockGenerator.AdvanceByHalfPeriod());
		mSimulationGroup.AdvanceAll(mClockGenerator.AdvanceByHalfPeriod());
	}
}
void CameraSimulationDataGenerator::CreateSerialByte( U64 value )
{
	//assume we start high

	//HSyncSimulationData->Transition();  //low-going edge for start bit
	//VSyncSimulationData->Transition();  //low-going edge for start bit
	for (U32 i = 0; i < mSimulationGroup.GetCount(); i++)
	{
		mSimulationGroup.GetArray()[i].Transition();
	}
	mSimulationGroup.AdvanceAll(mClockGenerator.AdvanceByHalfPeriod());  //add start bit time



	U32 num_bits = mSettings->mBitsPerTransfer;


	BitExtractor bit_extractor( value, mSettings->mShiftOrder, num_bits );

	for( U32 i=0; i<num_bits; i++ )
	{
		BitState x = bit_extractor.GetNextBit();
		for (U32 i = 0; i < mSimulationGroup.GetCount(); i++)
		{
			mSimulationGroup.GetArray()[i].TransitionIfNeeded(x);
		}
		mSimulationGroup.AdvanceAll(mClockGenerator.AdvanceByHalfPeriod());
		//HSyncSimulationData->TransitionIfNeeded( x );
		//HSyncSimulationData->Advance( mClockGenerator.AdvanceByHalfPeriod() );
		//VSyncSimulationData->TransitionIfNeeded(x);
		//VSyncSimulationData->Advance(mClockGenerator.AdvanceByHalfPeriod());
	}
	
	
	for (U32 i = 0; i < mSimulationGroup.GetCount(); i++)
	{
		mSimulationGroup.GetArray()[i].TransitionIfNeeded(mBitHigh);
	}
	//HSyncSimulationData->TransitionIfNeeded(mBitHigh);  //stop bit
	//VSyncSimulationData->TransitionIfNeeded(mBitHigh);  //stop bit

	//lets pad the end a bit for the stop bit:
	//HSyncSimulationData->Advance( mClockGenerator.AdvanceByHalfPeriod( mSettings->mStopBits ) );
	//VSyncSimulationData->Advance(mClockGenerator.AdvanceByHalfPeriod(mSettings->mStopBits));
	mSimulationGroup.AdvanceAll(mClockGenerator.AdvanceByHalfPeriod(mSettings->mStopBits));
	

	
}
