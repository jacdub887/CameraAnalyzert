#include "CameraAnalyzerResults.h"
#include <AnalyzerHelpers.h>
#include "CameraAnalyzer.h"
#include "CameraAnalyzerSettings.h"
#include <iostream>
#include <sstream>
#pragma warning(disable: 4996) //warning C4996: 'sprintf': This function or variable may be unsafe. Consider using sprintf_s instead.

CameraAnalyzerResults::CameraAnalyzerResults( CameraAnalyzer* analyzer, CameraAnalyzerSettings* settings )
:	AnalyzerResults(),
	mSettings( settings ),
	mAnalyzer( analyzer )
{
}

CameraAnalyzerResults::~CameraAnalyzerResults()
{
}

void CameraAnalyzerResults::GenerateBubbleText( U64 frame_index, Channel& channel, DisplayBase display_base )  //unrefereced vars commented out to remove warnings.
{
	//we only need to pay attention to 'channel' if we're making bubbles for more than one channel (as set by AddChannelBubblesWillAppearOn)
	ClearResultStrings();
	Frame frame = GetFrame( frame_index );

	
	if (channel == mSettings->VSyncChannel)
	{
		if (frame.mFlags == 2)
		{
			char vertical[32];
			char horizontal[32];
			char total[32];
			AnalyzerHelpers::GetNumberString(frame.mData1, display_base, 16, horizontal, 32);
			AnalyzerHelpers::GetNumberString(frame.mData2, display_base, 16, vertical, 32);
			AnalyzerHelpers::GetNumberString(frame.mData1 * frame.mData2, display_base, 24, total, 32);
			//AddResultString("Horizontal: ", horizontal," Vertical: ", vertical," Total: ",total);
			AddResultString(horizontal,"x", vertical," T:",total);
			AddResultString(total);
			return;
		}
		if (frame.mFlags == 3)
		{
			char vertical[32];
			char total[32];
			AnalyzerHelpers::GetNumberString(frame.mData2, display_base, 16, vertical, 32);
			AnalyzerHelpers::GetNumberString(frame.mData1, display_base, 24, total, 32);
			AddResultString("Different Data Count: ", vertical, " lines, Total: ", total);
			AddResultString( "L:", vertical, " T:", total);
			return;
		}
	}
	if (channel == mSettings->HSyncChannel)
	{
		if (frame.mFlags == 1)
		{
			char number_str[32];
			AnalyzerHelpers::GetNumberString(frame.mData1, display_base, 16, number_str, 32);
			AddResultString(number_str);
			AddResultString(number_str);
			return;
		}
	}
	if (channel == mSettings->PCKChannel)
	{
		if (frame.mFlags == 0) {
			char number_str[32];
			AnalyzerHelpers::GetNumberString(frame.mData1, display_base, 8, number_str, 32);
			AddResultString(number_str);
			return;
		}
	}

	
	
	
	/*
	bool framing_error = false;
	if( ( frame.mFlags & FRAMING_ERROR_FLAG ) != 0 )
		framing_error = true;

	bool parity_error = false;
	if( ( frame.mFlags & PARITY_ERROR_FLAG ) != 0 )
		parity_error = true;

	U32 bits_per_transfer = mSettings->mBitsPerTransfer;
	if( mSettings->mSerialMode != CameraAnalyzerEnums::Normal )
		bits_per_transfer--;

	char number_str[128];
	AnalyzerHelpers::GetNumberString( frame.mData1, display_base, bits_per_transfer, number_str, 128 );

	char result_str[128];

	//MP mode address case:
	bool mp_mode_address_flag = false;
	if( ( frame.mFlags & MP_MODE_ADDRESS_FLAG ) != 0 )
	{
		mp_mode_address_flag = true;

		AddResultString( "A" );
		AddResultString( "Addr" );

		if( framing_error == false )
		{
			sprintf( result_str, "Addr: %s", number_str );
			AddResultString( result_str );

			sprintf( result_str, "Address: %s", number_str );
			AddResultString( result_str );

		}else
		{
			sprintf( result_str, "Addr: %s (framing error)", number_str );
			AddResultString( result_str );

			sprintf( result_str, "Address: %s (framing error)", number_str );
			AddResultString( result_str );
		}
		return;
	}

	//normal case:
	if( ( parity_error == true ) || ( framing_error == true ) )
	{
		AddResultString( "!" );

		sprintf( result_str, "%s (error)", number_str );
		AddResultString( result_str );

		if( parity_error == true && framing_error == false )
			sprintf( result_str, "%s (parity error)", number_str );
		else
		if( parity_error == false && framing_error == true )
			sprintf( result_str, "%s (framing error)", number_str );
		else
			sprintf( result_str, "%s (framing error & parity error)", number_str );

		AddResultString( result_str );
			
	}else
	{
		
		AddResultString( number_str );
	}
	*/
}

void CameraAnalyzerResults::AddPictureByte(U8 _byte)
{
	if (pictureDataPTR == 16000000) pictureDataPTR--;
	pictureData[pictureDataPTR++] = _byte;
}

void CameraAnalyzerResults::ResetPointer()
{
	pictureDataPTR = 0;
}

void CameraAnalyzerResults::GenerateExportFile( const char* file, DisplayBase display_base, U32 /*export_type_user_id*/ )
{
	//export_type_user_id is only important if we have more than one export type.
	std::stringstream ss;
	
	U64 trigger_sample = mAnalyzer->GetTriggerSample();
	U32 sample_rate = mAnalyzer->GetSampleRate();
	U32 num_frames = pictureDataPTR;
	
	if (pictureDataPTR == 16000000)
	{
		void* f = AnalyzerHelpers::StartFile(file);
		ss << "DATA BUFFER OVERFLOW 16MB USED";
		AnalyzerHelpers::AppendToFile((U8*)ss.str().c_str(), ss.str().length(), f);
		AnalyzerHelpers::EndFile(f);
	}
	else {
		AnalyzerHelpers::SaveFile(file, pictureData, pictureDataPTR, true);
	}
	UpdateExportProgressAndCheckForCancel(num_frames, num_frames);
	

	/**
	return;
	if( mSettings->mSerialMode == CameraAnalyzerEnums::Normal )
	{
		//Normal case -- not MP mode.
		ss << "Time [s],Value,Parity Error,Framing Error" << std::endl;

		for( U32 i=0; i < num_frames; i++ )
		{
			Frame frame = GetFrame( i );
			
			//static void GetTimeString( U64 sample, U64 trigger_sample, U32 sample_rate_hz, char* result_string, U32 result_string_max_length );
			char time_str[128];
			AnalyzerHelpers::GetTimeString( frame.mStartingSampleInclusive, trigger_sample, sample_rate, time_str, 128 );

			char number_str[128];
			AnalyzerHelpers::GetNumberString( frame.mData1, display_base, mSettings->mBitsPerTransfer, number_str, 128 );

			ss << time_str << "," << number_str;

			if( ( frame.mFlags & PARITY_ERROR_FLAG ) != 0 )
				ss << ",Error,";
			else
				ss << ",,";

			if( ( frame.mFlags & FRAMING_ERROR_FLAG ) != 0 )
				ss << "Error";


			ss << std::endl;

			AnalyzerHelpers::AppendToFile( (U8*)ss.str().c_str(), ss.str().length(), f );
			ss.str( std::string() );

			if( UpdateExportProgressAndCheckForCancel( i, num_frames ) == true )
			{
				AnalyzerHelpers::EndFile( f );
				return;
			}
		}
	}else
	{
		//MP mode.
		ss << "Time [s],Packet ID,Address,Data,Framing Error" << std::endl;
		U64 address = 0;

		for( U32 i=0; i < num_frames; i++ )
		{
			Frame frame = GetFrame( i );

			if( ( frame.mFlags & MP_MODE_ADDRESS_FLAG ) != 0 )
			{
				address = frame.mData1;
				continue;
			}

			U64 packet_id = GetPacketContainingFrameSequential( i ); 

			//static void GetTimeString( U64 sample, U64 trigger_sample, U32 sample_rate_hz, char* result_string, U32 result_string_max_length );
			char time_str[128];
			AnalyzerHelpers::GetTimeString( frame.mStartingSampleInclusive, trigger_sample, sample_rate, time_str, 128 );

			char address_str[128];
			AnalyzerHelpers::GetNumberString( address, display_base, mSettings->mBitsPerTransfer - 1, address_str, 128 );

			char number_str[128];
			AnalyzerHelpers::GetNumberString( frame.mData1, display_base, mSettings->mBitsPerTransfer - 1, number_str, 128 );
			if( packet_id == INVALID_RESULT_INDEX )
				ss << time_str << "," << "" << "," << address_str << "," << number_str << ",";
			else
				ss << time_str << "," << packet_id << "," << address_str << "," << number_str << ",";

			if( ( frame.mFlags & FRAMING_ERROR_FLAG ) != 0 )
				ss << "Error";

			ss << std::endl;

			AnalyzerHelpers::AppendToFile( (U8*)ss.str().c_str(), ss.str().length(), f );
			ss.str( std::string() );


			if( UpdateExportProgressAndCheckForCancel( i, num_frames ) == true )
			{
				AnalyzerHelpers::EndFile( f );
				return;
			}


		}
	}

	UpdateExportProgressAndCheckForCancel( num_frames, num_frames );
	AnalyzerHelpers::EndFile( f );
	*/
}

void CameraAnalyzerResults::GenerateFrameTabularText( U64 frame_index, DisplayBase display_base )
{
#ifdef PLATFORMx64
	ClearTabularText();
#endif // DEBUG

	
	return;
	Frame frame = GetFrame( frame_index );
	ClearResultStrings();

	bool framing_error = false;
	if( ( frame.mFlags & FRAMING_ERROR_FLAG ) != 0 )
		framing_error = true;

	bool parity_error = false;
	if( ( frame.mFlags & PARITY_ERROR_FLAG ) != 0 )
		parity_error = true;

	char number_str[128];
	AnalyzerHelpers::GetNumberString( frame.mData1, display_base, mSettings->mBitsPerTransfer, number_str, 128 );

	char result_str[128];

	if( parity_error == false && framing_error == false )
	{
		AddResultString( number_str );
	}else
	if( parity_error == true && framing_error == false )
	{
		sprintf( result_str, "%s (parity error)", number_str );
		AddResultString( result_str );
	}else
	if( parity_error == false && framing_error == true )
	{
		sprintf( result_str, "%s (framing error)", number_str );
		AddResultString( result_str );
	}else
	{
		sprintf( result_str, "%s (framing error & parity error)", number_str );
		AddResultString( result_str );
	}
}

void CameraAnalyzerResults::GeneratePacketTabularText( U64 /*packet_id*/, DisplayBase /*display_base*/ )  //unrefereced vars commented out to remove warnings.
{
	ClearResultStrings();
	AddResultString( "not supported" );
}

void CameraAnalyzerResults::GenerateTransactionTabularText( U64 /*transaction_id*/, DisplayBase /*display_base*/ )  //unrefereced vars commented out to remove warnings.
{
	ClearResultStrings();
	AddResultString( "not supported" );
}


