#include "CameraAnalyzer.h"
#include "CameraAnalyzerSettings.h"
#include <AnalyzerChannelData.h>


CameraAnalyzer::CameraAnalyzer()
:	Analyzer2(),  
	mSettings( new CameraAnalyzerSettings() ),
	mSimulationInitilized( false )
{
	SetAnalyzerSettings( mSettings.get() );
}

CameraAnalyzer::~CameraAnalyzer()
{
	KillThread();
}

void CameraAnalyzer::ComputeSampleOffsets()
{
	ClockGenerator clock_generator;
	clock_generator.Init( mSettings->mBitRate, mSampleRateHz );

	mSampleOffsets.clear();
	
	U32 num_bits = mSettings->mBitsPerTransfer;

	if( mSettings->mSerialMode != CameraAnalyzerEnums::Normal )
	num_bits++;

	mSampleOffsets.push_back( clock_generator.AdvanceByHalfPeriod( 1.5 ) );  //point to the center of the 1st bit (past the start bit)
	num_bits--;  //we just added the first bit.

	for( U32 i=0; i<num_bits; i++ )
	{
		mSampleOffsets.push_back( clock_generator.AdvanceByHalfPeriod() );
	}

	if( mSettings->mParity != AnalyzerEnums::None )
		mParityBitOffset = clock_generator.AdvanceByHalfPeriod();

	//to check for framing errors, we also want to check
	//1/2 bit after the beginning of the stop bit
	mStartOfStopBitOffset = clock_generator.AdvanceByHalfPeriod( 1.0 );  //i.e. moving from the center of the last data bit (where we left off) to 1/2 period into the stop bit

	//and 1/2 bit before end of the stop bit period
	mEndOfStopBitOffset = clock_generator.AdvanceByHalfPeriod( mSettings->mStopBits - 1.0 );  //if stopbits == 1.0, this will be 0
}


void CameraAnalyzer::SetupResults()
{
	//Unlike the worker thread, this function is called from the GUI thread
	//we need to reset the Results object here because it is exposed for direct access by the GUI, and it can't be deleted from the WorkerThread
	//PCKResults.reset(new CameraAnalyzerResults(this, mSettings.get()));
	//PCKResults->AddChannelBubblesWillAppearOn(mSettings->PCKChannel);
	
	mResults.reset( new CameraAnalyzerResults( this, mSettings.get() ) );
	SetAnalyzerResults( mResults.get() );
	if(mSettings->useVSync) mResults->AddChannelBubblesWillAppearOn(mSettings->VSyncChannel);
	if(mSettings->useHSync) mResults->AddChannelBubblesWillAppearOn( mSettings->HSyncChannel );
	if(mSettings->usePCK) mResults->AddChannelBubblesWillAppearOn(mSettings->PCKChannel);
	mResults->ResetPointer();
}

U64 CameraAnalyzer::AdvanceClock()
{
	U64 PCKLowPosition;
	if (PCKData->GetBitState() == mBitLow) {
		PCKLowPosition = PCKData->GetSampleNumber();
		PCKData->AdvanceToNextEdge();
	}else {
		PCKData->AdvanceToNextEdge();
		PCKLowPosition = PCKData->GetSampleNumber();
		PCKData->AdvanceToNextEdge();
	}
	U64 PCKHighPosition = PCKData->GetSampleNumber();
	VSyncData->AdvanceToAbsPosition(PCKHighPosition);
	HSyncData->AdvanceToAbsPosition(PCKHighPosition);
	D0Data->AdvanceToAbsPosition(PCKHighPosition);
	D1Data->AdvanceToAbsPosition(PCKHighPosition);
	D2Data->AdvanceToAbsPosition(PCKHighPosition);
	D3Data->AdvanceToAbsPosition(PCKHighPosition);
	D4Data->AdvanceToAbsPosition(PCKHighPosition);
	D5Data->AdvanceToAbsPosition(PCKHighPosition);
	D6Data->AdvanceToAbsPosition(PCKHighPosition);
	D7Data->AdvanceToAbsPosition(PCKHighPosition);
	return PCKLowPosition;
}

void CameraAnalyzer::AdvanceAll(U64 SampleNumber)
{
	VSyncData->AdvanceToAbsPosition(SampleNumber);
	HSyncData->AdvanceToAbsPosition(SampleNumber);
	PCKData->AdvanceToAbsPosition(SampleNumber);
	D0Data->AdvanceToAbsPosition(SampleNumber);
	D1Data->AdvanceToAbsPosition(SampleNumber);
	D2Data->AdvanceToAbsPosition(SampleNumber);
	D3Data->AdvanceToAbsPosition(SampleNumber);
	D4Data->AdvanceToAbsPosition(SampleNumber);
	D5Data->AdvanceToAbsPosition(SampleNumber);
	D6Data->AdvanceToAbsPosition(SampleNumber);
	D7Data->AdvanceToAbsPosition(SampleNumber);
}
void CameraAnalyzer::WorkerThread()
{
	
	U32 lastLineDataCount = 0, lineDataCount = 0, linesCount = 0, totalCount = 0;
	mBitHigh = BIT_HIGH;
	mBitLow = BIT_LOW;
	
	VSyncData = GetAnalyzerChannelData(mSettings->VSyncChannel);
	HSyncData = GetAnalyzerChannelData(mSettings->HSyncChannel);
	PCKData = GetAnalyzerChannelData(mSettings->PCKChannel);
	D0Data = GetAnalyzerChannelData(mSettings->D0Channel);
	D1Data = GetAnalyzerChannelData(mSettings->D1Channel);
	D2Data = GetAnalyzerChannelData(mSettings->D2Channel);
	D3Data = GetAnalyzerChannelData(mSettings->D3Channel);
	D4Data = GetAnalyzerChannelData(mSettings->D4Channel);
	D5Data = GetAnalyzerChannelData(mSettings->D5Channel);
	D6Data = GetAnalyzerChannelData(mSettings->D6Channel);
	D7Data = GetAnalyzerChannelData(mSettings->D7Channel);
	

	
	int fCount = mSettings->framesCount;
	
	while (true)
	{
		if (fCount-- == 0) break;
		if (VSyncData->GetBitState() == mBitLow) {
			VSyncData->AdvanceToNextEdge(); 
			VSyncData->AdvanceToNextEdge();
		}else VSyncData->AdvanceToNextEdge();

		AdvanceAll(VSyncData->GetSampleNumber());
		Frame VSyncFrame;
		VSyncFrame.mStartingSampleInclusive = VSyncData->GetSampleNumber();
		
		while (VSyncData->GetBitState() == mBitLow)
		{
			U64 lowEdge = AdvanceClock();
			Frame HSyncFrame;
			HSyncFrame.mStartingSampleInclusive = lowEdge;
			if (HSyncData->GetBitState() == mBitLow) continue;
			while (HSyncData->GetBitState() == mBitHigh)
			{
				Frame PCKFrame;
				U64 data = 0;
				DataBuilder data_builder;
				data_builder.Reset(&data, AnalyzerEnums::LsbFirst, mSettings->dataChannels);
				data_builder.AddBit(D0Data->GetBitState());
				data_builder.AddBit(D1Data->GetBitState());
				data_builder.AddBit(D2Data->GetBitState());
				data_builder.AddBit(D3Data->GetBitState());
				data_builder.AddBit(D4Data->GetBitState());
				data_builder.AddBit(D5Data->GetBitState());
				data_builder.AddBit(D6Data->GetBitState());
				data_builder.AddBit(D7Data->GetBitState());

				lineDataCount++;
				totalCount++;
				PCKFrame.mData1 = data;
				PCKFrame.mFlags = 0;
				PCKFrame.mStartingSampleInclusive = lowEdge;
				lowEdge = AdvanceClock();
				PCKFrame.mEndingSampleInclusive = lowEdge;
				if (mSettings->usePCK) {
					mResults->AddFrame(PCKFrame);
					mResults->CommitResults();
				}
				//PCKResults->AddFrame(PCKFrame);
				//PCKResults->CommitResults();
				mResults->AddPictureByte(PCKFrame.mData1);
				ReportProgress(PCKFrame.mEndingSampleInclusive);
				CheckIfThreadShouldExit();
			}
			HSyncFrame.mEndingSampleInclusive = lowEdge;
			HSyncFrame.mData1 = lineDataCount;
			HSyncFrame.mFlags = 1;
			lastLineDataCount = lineDataCount;
			lineDataCount = 0;
			linesCount++;
			if (mSettings->useHSync) {
				mResults->AddFrame(HSyncFrame);
				mResults->CommitResults();
			}
			ReportProgress(HSyncFrame.mEndingSampleInclusive);
		}
		VSyncFrame.mEndingSampleInclusive = VSyncData->GetSampleNumber();
		VSyncFrame.mFlags = 2;
		if (linesCount == 0) linesCount = 1;
		if ((totalCount / linesCount) != lastLineDataCount)
		{
			VSyncFrame.mFlags = 3;
			VSyncFrame.mData1 = totalCount;
		}
		else {
			VSyncFrame.mData1 = lastLineDataCount;
		}
		VSyncFrame.mData2 = linesCount;
		linesCount = 0;
		totalCount = 0;
		if (mSettings->useVSync) {
			mResults->AddFrame(VSyncFrame);
			mResults->CommitPacketAndStartNewPacket();
			mResults->CommitResults();
		}
	}
	
	
	/**
	return;
	Frame VSyncFrame;
	VSyncFrame.mData1 = 125;
	VSyncFrame.mFlags = 0;

	if (VSyncData->GetBitState() == mBitLow) VSyncData->AdvanceToNextEdge();
	//mResults->AddMarker(VSyncData->GetSampleNumber() , AnalyzerResults::Dot, mSettings->VSyncChannel);
	VSyncFrame.mStartingSampleInclusive = VSyncData->GetSampleNumber();
	VSyncData->AdvanceToNextEdge();
	VSyncData->AdvanceToNextEdge();
	VSyncData->AdvanceToNextEdge();
	//mResults->AddMarker(VSyncData->GetSampleNumber(), AnalyzerResults::Dot, mSettings->VSyncChannel);
	VSyncFrame.mEndingSampleInclusive = VSyncData->GetSampleNumber();
	

	
	
	mResults->AddFrame(VSyncFrame);
	mResults->CommitResults();
	//ReportProgress(VSyncFrame.mEndingSampleInclusive);

	//ReportProgress(VSyncFrame.mEndingSampleInclusive);
	//mResults->CommitPacketAndStartNewPacket();
	//mResults->CommitResults();
	//mResults->AddChannelBubblesWillAppearOn(mSettings->HSyncChannel);
	
	
	
	mSampleRateHz = GetSampleRate();
	ComputeSampleOffsets();
	U32 num_bits = mSettings->mBitsPerTransfer;

	
	mSerial = GetAnalyzerChannelData( mSettings->HSyncChannel );
	mSerial->TrackMinimumPulseWidth();
	
	if( mSerial->GetBitState() == mBitLow )
		mSerial->AdvanceToNextEdge();

	for( ; ; )
	{
		//we're starting high.  (we'll assume that we're not in the middle of a byte. 

		mSerial->AdvanceToNextEdge();

		//we're now at the beginning of the start bit.  We can start collecting the data.
		U64 frame_starting_sample = mSerial->GetSampleNumber();

		U64 data = 0;
		bool parity_error = false;
		bool framing_error = false;
		bool mp_is_address = false;
		
		DataBuilder data_builder;
		data_builder.Reset( &data, mSettings->mShiftOrder, num_bits );
		U64 marker_location = frame_starting_sample;

		for( U32 i=0; i<num_bits; i++ )
		{
			mSerial->Advance( mSampleOffsets[i] );
			data_builder.AddBit( mSerial->GetBitState() );

			marker_location += mSampleOffsets[i];
			mResults->AddMarker( marker_location, AnalyzerResults::Dot, mSettings->HSyncChannel );
		}

			
		parity_error = false;

		//now we must dermine if there is a framing error.
		framing_error = false;

		mSerial->Advance( mStartOfStopBitOffset );

		if( mSerial->GetBitState() != mBitHigh )
		{
			framing_error = true;
		}else
		{
			U32 num_edges = mSerial->Advance( mEndOfStopBitOffset );
			if( num_edges != 0 )
				framing_error = true;
		}

		if( framing_error == true )
		{
			marker_location += mStartOfStopBitOffset;
			mResults->AddMarker( marker_location, AnalyzerResults::ErrorX, mSettings->HSyncChannel );

			if( mEndOfStopBitOffset != 0 )
			{
				marker_location += mEndOfStopBitOffset;
				mResults->AddMarker( marker_location, AnalyzerResults::ErrorX, mSettings->HSyncChannel );
			}
		}

		//ok now record the value!
		//note that we're not using the mData2 or mType fields for anything, so we won't bother to set them.
		Frame frame;
		frame.mStartingSampleInclusive = frame_starting_sample;
		frame.mEndingSampleInclusive = mSerial->GetSampleNumber();
		frame.mData1 = data;
		frame.mFlags = 0;
		if( parity_error == true )
		frame.mFlags |= PARITY_ERROR_FLAG | DISPLAY_AS_ERROR_FLAG;

		if( framing_error == true )
			frame.mFlags |= FRAMING_ERROR_FLAG | DISPLAY_AS_ERROR_FLAG;

		if( mp_is_address == true )
			frame.mFlags |= MP_MODE_ADDRESS_FLAG;

		if( mp_is_address == true )
			mResults->CommitPacketAndStartNewPacket();

		mResults->AddFrame( frame );

		mResults->CommitResults();

		ReportProgress( frame.mEndingSampleInclusive );
		CheckIfThreadShouldExit();

	}
	*/
}

bool CameraAnalyzer::NeedsRerun()
{
	return false;
	if( mSettings->mUseAutobaud == false )
		return false;

	//ok, lets see if we should change the bit rate, base on mShortestActivePulse

	U64 shortest_pulse = mSerial->GetMinimumPulseWidthSoFar();

	if( shortest_pulse == 0 )
		AnalyzerHelpers::Assert( "Alg problem, shortest_pulse was 0" );

	U32 computed_bit_rate = U32( double( mSampleRateHz ) / double( shortest_pulse ) );

	if( computed_bit_rate > mSampleRateHz )
		AnalyzerHelpers::Assert( "Alg problem, computed_bit_rate is higer than sample rate" );  //just checking the obvious...

	if( computed_bit_rate > (mSampleRateHz / 4) )
		return false; //the baud rate is too fast.
	if( computed_bit_rate == 0 )
	{
		//bad result, this is not good data, don't bother to re-run.
		return false;
	}

	U32 specified_bit_rate = mSettings->mBitRate;

	double error = double( AnalyzerHelpers::Diff32( computed_bit_rate, specified_bit_rate ) ) / double( specified_bit_rate );

	if( error > 0.1 )
	{
		mSettings->mBitRate = computed_bit_rate;
		mSettings->UpdateInterfacesFromSettings();
		return true;
	}else
	{
		return false;
	}
}

U32 CameraAnalyzer::GenerateSimulationData( U64 minimum_sample_index, U32 device_sample_rate, SimulationChannelDescriptor** simulation_channels )
{
	if( mSimulationInitilized == false )
	{
		mSimulationDataGenerator.Initialize( GetSimulationSampleRate(), mSettings.get() );
		mSimulationInitilized = true;
	}

	return mSimulationDataGenerator.GenerateSimulationData(minimum_sample_index, device_sample_rate, simulation_channels);
}

U32 CameraAnalyzer::GetMinimumSampleRateHz()
{
	
	return mSettings->mBitRate * 4;
}

const char* CameraAnalyzer::GetAnalyzerName() const
{
	return "CameraAnalyzer";
}

const char* GetAnalyzerName()
{
	return "CameraAnalyzer";
}

Analyzer* CreateAnalyzer()
{
	return new CameraAnalyzer();
}

void DestroyAnalyzer( Analyzer* analyzer )
{
	delete analyzer;
}
