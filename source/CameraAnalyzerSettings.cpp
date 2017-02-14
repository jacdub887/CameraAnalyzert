#include "CameraAnalyzerSettings.h"

#include <AnalyzerHelpers.h>
#include <sstream>

#pragma warning(disable: 4800) //warning C4800: 'U32' : forcing value to bool 'true' or 'false' (performance warning)
#pragma warning(disable: 4996) //warning C4996: 'sprintf': This function or variable may be unsafe. Consider using sprintf_s instead.

CameraAnalyzerSettings::CameraAnalyzerSettings()
	: VSyncChannel(UNDEFINED_CHANNEL),
	HSyncChannel(UNDEFINED_CHANNEL),
	PCKChannel(UNDEFINED_CHANNEL),
	D0Channel(UNDEFINED_CHANNEL),
	D1Channel(UNDEFINED_CHANNEL),
	D2Channel(UNDEFINED_CHANNEL),
	D3Channel(UNDEFINED_CHANNEL),
	D4Channel(UNDEFINED_CHANNEL),
	D5Channel(UNDEFINED_CHANNEL),
	D6Channel(UNDEFINED_CHANNEL),
	D7Channel(UNDEFINED_CHANNEL),
	useVSync(true),
	useHSync(true),
	usePCK(true),
	framesCount(-1),
	
	mBitRate( 9600 ),
	mBitsPerTransfer( 8 ),
	mStopBits( 1.0 ),
	mParity( AnalyzerEnums::None ),
	mShiftOrder( AnalyzerEnums::LsbFirst ),
	mInverted( false ),
	mUseAutobaud( false ),
	mSerialMode( CameraAnalyzerEnums::Normal )
{
	VSyncInterface.reset(new AnalyzerSettingInterfaceChannel());
	VSyncInterface->SetTitleAndTooltip("VSync", "VSync Height");
	VSyncInterface->SetChannel(VSyncChannel);

	HSyncInterface.reset( new AnalyzerSettingInterfaceChannel() );
	HSyncInterface->SetTitleAndTooltip( "HSync", "HSync Width" );
	HSyncInterface->SetChannel( HSyncChannel );
	
	PCKInterface.reset(new AnalyzerSettingInterfaceChannel());
	PCKInterface->SetTitleAndTooltip("PCK", "PIXEL CLOCK");
	PCKInterface->SetChannel(PCKChannel);
	
	D0Interface.reset(new AnalyzerSettingInterfaceChannel());
	D0Interface->SetTitleAndTooltip("D0", "DATA 0");
	D0Interface->SetChannel(D0Channel);

	D1Interface.reset(new AnalyzerSettingInterfaceChannel());
	D1Interface->SetTitleAndTooltip("D1", "DATA 1");
	D1Interface->SetChannel(D1Channel);

	D2Interface.reset(new AnalyzerSettingInterfaceChannel());
	D2Interface->SetTitleAndTooltip("D2", "DATA 2");
	D2Interface->SetChannel(D2Channel);

	D3Interface.reset(new AnalyzerSettingInterfaceChannel());
	D3Interface->SetTitleAndTooltip("D3", "DATA 3");
	D3Interface->SetChannel(D3Channel);

	D4Interface.reset(new AnalyzerSettingInterfaceChannel());
	D4Interface->SetTitleAndTooltip("D4", "DATA 4");
	D4Interface->SetChannel(D4Channel);
	
	D5Interface.reset(new AnalyzerSettingInterfaceChannel());
	D5Interface->SetTitleAndTooltip("D5", "DATA 5");
	D5Interface->SetChannel(D5Channel);

	D6Interface.reset(new AnalyzerSettingInterfaceChannel());
	D6Interface->SetTitleAndTooltip("D6", "DATA 6");
	D6Interface->SetChannel(D6Channel);

	D7Interface.reset(new AnalyzerSettingInterfaceChannel());
	D7Interface->SetTitleAndTooltip("D7", "DATA 7");
	D7Interface->SetChannel(D7Channel);

	useVSyncInterface.reset(new AnalyzerSettingInterfaceBool());
	useVSyncInterface->SetTitleAndTooltip("", "");
	useVSyncInterface->SetCheckBoxText("Display VSync");
	useVSyncInterface->SetValue(useVSync);

	useHSyncInterface.reset(new AnalyzerSettingInterfaceBool());
	useHSyncInterface->SetTitleAndTooltip("", "");
	useHSyncInterface->SetCheckBoxText("Display HSync");
	useHSyncInterface->SetValue(useHSync);

	usePCKInterface.reset(new AnalyzerSettingInterfaceBool());
	usePCKInterface->SetTitleAndTooltip("", "");
	usePCKInterface->SetCheckBoxText("Display PCK Data");
	usePCKInterface->SetValue(usePCK);

	framesCountInterface.reset(new AnalyzerSettingInterfaceNumberList());
	framesCountInterface->SetTitleAndTooltip("number of camera frames to be processed", "bollocks");
	framesCountInterface->AddNumber(-1, "Unlimited", "");
	framesCountInterface->AddNumber(1, "1", "lolo");
	framesCountInterface->AddNumber(2, "2", "");
	framesCountInterface->AddNumber(3, "3", "");
	framesCountInterface->AddNumber(4, "4", "");
	framesCountInterface->AddNumber(5, "5", "");
	framesCountInterface->AddNumber(6, "6", "");
	framesCountInterface->AddNumber(7, "7", "");
	framesCountInterface->AddNumber(8, "8", "");
	framesCountInterface->SetNumber(-1);
	
	/**
	mBitRateInterface.reset( new AnalyzerSettingInterfaceInteger() );
	mBitRateInterface->SetTitleAndTooltip( "Bit Rate (Bits/S)",  "Specify the bit rate in bits per second." );
	mBitRateInterface->SetMax( 6000000 );
	mBitRateInterface->SetMin( 1 );
	mBitRateInterface->SetInteger( mBitRate );

	mUseAutobaudInterface.reset( new AnalyzerSettingInterfaceBool() );
	mUseAutobaudInterface->SetTitleAndTooltip( "", "With Autobaud turned on, the analyzer will run as usual, with the current bit rate.  At the same time, it will also keep track of the shortest pulse it detects.  After analyzing all the data, if the bit rate implied by this shortest pulse is different by more than 10% from the specified bit rate, the bit rate will be changed and the analysis run again." );
	mUseAutobaudInterface->SetCheckBoxText( "Use Autobaud" );
	mUseAutobaudInterface->SetValue( mUseAutobaud );

	mBitsPerTransferInterface.reset( new AnalyzerSettingInterfaceNumberList() );
	mBitsPerTransferInterface->SetTitleAndTooltip( "", "Select the number of bits per frame" ); 
	for( U32 i = 1; i <= 64; i++ )
	{
		std::stringstream ss; 

		if( i == 1 )
			ss << "1 Bit per Transfer";
		else
			if( i == 8 )
				ss << "8 Bits per Transfer (Standard)";
			else
				ss << i << " Bits per Transfer";

		mBitsPerTransferInterface->AddNumber( i, ss.str().c_str(), "" );
	}
	mBitsPerTransferInterface->SetNumber( mBitsPerTransfer );


	mStopBitsInterface.reset( new AnalyzerSettingInterfaceNumberList() );
	mStopBitsInterface->SetTitleAndTooltip( "", "Specify the number of stop bits." );
	mStopBitsInterface->AddNumber( 1.0, "1 Stop Bit (Standard)", "" );
	mStopBitsInterface->AddNumber( 1.5, "1.5 Stop Bits", "" );
	mStopBitsInterface->AddNumber( 2.0, "2 Stop Bits", "" );
	mStopBitsInterface->SetNumber( mStopBits ); 


	mParityInterface.reset( new AnalyzerSettingInterfaceNumberList() );
	mParityInterface->SetTitleAndTooltip( "", "Specify None, Even, or Odd Parity." );
	mParityInterface->AddNumber( AnalyzerEnums::None, "No Parity Bit (Standard)", "" );
	mParityInterface->AddNumber( AnalyzerEnums::Even, "Even Parity Bit", "" );
	mParityInterface->AddNumber( AnalyzerEnums::Odd, "Odd Parity Bit", "" ); 
	mParityInterface->SetNumber( mParity );


	mShiftOrderInterface.reset( new AnalyzerSettingInterfaceNumberList() );
	mShiftOrderInterface->SetTitleAndTooltip( "", "Select if the most significant bit or least significant bit is transmitted first" );
	mShiftOrderInterface->AddNumber( AnalyzerEnums::LsbFirst, "Least Significant Bit Sent First (Standard)", "" );
	mShiftOrderInterface->AddNumber( AnalyzerEnums::MsbFirst, "Most Significant Bit Sent First", "" );
	mShiftOrderInterface->SetNumber( mShiftOrder );


	mInvertedInterface.reset( new AnalyzerSettingInterfaceNumberList() );
	mInvertedInterface->SetTitleAndTooltip( "", "Specify if the serial signal is inverted" );
	mInvertedInterface->AddNumber( false, "Non Inverted (Standard)", "" );
	mInvertedInterface->AddNumber( true, "Inverted", "" );

	mInvertedInterface->SetNumber( mInverted );enum Mode { Normal, MpModeRightZeroMeansAddress, MpModeRightOneMeansAddress, MpModeLeftZeroMeansAddress, MpModeLeftOneMeansAddress };

	mSerialModeInterface.reset( new AnalyzerSettingInterfaceNumberList() );
	mSerialModeInterface->SetTitleAndTooltip( "Special Mode", "Specify if this is normal, or MP serial (aka multi-drop, MP, multi-processor, 9-bit serial)" );
	mSerialModeInterface->AddNumber( CameraAnalyzerEnums::Normal, "None", "" );
	mSerialModeInterface->AddNumber( CameraAnalyzerEnums::MpModeMsbZeroMeansAddress, "MP Mode: Address indicated by MSB=0", "(aka MP, multi-processor, 9-bit serial)" );
	mSerialModeInterface->AddNumber( CameraAnalyzerEnums::MpModeMsbOneMeansAddress, "MDB Mode: Address indicated by MSB=1", "(aka multi-drop, 9-bit serial)" );
	mSerialModeInterface->SetNumber( mSerialMode );
	*/

	AddInterface( VSyncInterface.get());
	AddInterface( HSyncInterface.get() );
	AddInterface(PCKInterface.get());
	AddInterface(D0Interface.get());
	AddInterface(D1Interface.get());
	AddInterface(D2Interface.get());
	AddInterface(D3Interface.get());
	AddInterface(D4Interface.get());
	AddInterface(D5Interface.get());
	AddInterface(D6Interface.get());
	AddInterface(D7Interface.get());
	AddInterface(useVSyncInterface.get());
	AddInterface(useHSyncInterface.get());
	AddInterface(usePCKInterface.get());
	AddInterface(framesCountInterface.get());
	
	/**
	AddInterface( mBitRateInterface.get() );
	AddInterface( mUseAutobaudInterface.get() );
	AddInterface( mBitsPerTransferInterface.get() );
	AddInterface( mStopBitsInterface.get() );
	AddInterface( mParityInterface.get() );
	AddInterface( mShiftOrderInterface.get() );
	AddInterface( mInvertedInterface.get() );
	AddInterface( mSerialModeInterface.get() );
	*/

	//AddExportOption( 0, "Export as text/csv file", "text (*.txt);;csv (*.csv)" );
	AddExportOption( 0, "Export as HxD binary file" );
	AddExportExtension( 0, "frame", "hdd" );
	//AddExportExtension( 0, "csv", "csv" );

	ClearChannels();
	AddChannel( VSyncChannel, "VSync", false);
	AddChannel( HSyncChannel, "HSync", false );
	AddChannel(PCKChannel, "PCK", false);
	AddChannel(D0Channel, "D0", false);
	AddChannel(D1Channel, "D1", false);
	AddChannel(D2Channel, "D2", false);
	AddChannel(D3Channel, "D3", false);
	AddChannel(D4Channel, "D4", false);
	AddChannel(D5Channel, "D5", false);
	AddChannel(D6Channel, "D6", false);
	AddChannel(D7Channel, "D7", false);
	
}

CameraAnalyzerSettings::~CameraAnalyzerSettings()
{
}

bool CameraAnalyzerSettings::SetSettingsFromInterfaces()
{/**
	if( AnalyzerEnums::Parity( U32( mParityInterface->GetNumber() ) ) != AnalyzerEnums::None )
		if( CameraAnalyzerEnums::Mode( U32( mSerialModeInterface->GetNumber() ) ) != CameraAnalyzerEnums::Normal )
		{
			SetErrorText( "Sorry, but we don't support using parity at the same time as MP mode." );
			return false;
		}
	*/
	VSyncChannel = VSyncInterface->GetChannel();
	HSyncChannel = HSyncInterface->GetChannel();
	PCKChannel = PCKInterface->GetChannel();
	D0Channel = D0Interface->GetChannel();
	D1Channel = D1Interface->GetChannel();
	D2Channel = D2Interface->GetChannel();
	D3Channel = D3Interface->GetChannel();
	D4Channel = D4Interface->GetChannel();
	D5Channel = D5Interface->GetChannel();
	D6Channel = D6Interface->GetChannel();
	D7Channel = D7Interface->GetChannel();
	useVSync = useVSyncInterface->GetValue();
	useHSync = useHSyncInterface->GetValue();
	usePCK = usePCKInterface->GetValue();
	framesCount = framesCountInterface->GetNumber();
	/**
	mBitRate = mBitRateInterface->GetInteger();
	mBitsPerTransfer = U32( mBitsPerTransferInterface->GetNumber() );
	mStopBits = mStopBitsInterface->GetNumber();
	mParity = AnalyzerEnums::Parity( U32( mParityInterface->GetNumber() ) );
	mShiftOrder =  AnalyzerEnums::ShiftOrder( U32( mShiftOrderInterface->GetNumber() ) );
	mInverted = bool( U32( mInvertedInterface->GetNumber() ) );
	mUseAutobaud = mUseAutobaudInterface->GetValue();
	mSerialMode = CameraAnalyzerEnums::Mode( U32( mSerialModeInterface->GetNumber() ) );
	*/
	ClearChannels();
	AddChannel(VSyncChannel, "VSync", true);
	AddChannel( HSyncChannel, "HSync", true );
	AddChannel(PCKChannel, "PCK", true);
	AddChannel(D0Channel, "D0", true);
	AddChannel(D1Channel, "D1", true);
	AddChannel(D2Channel, "D2", true);
	AddChannel(D3Channel, "D3", true);
	AddChannel(D4Channel, "D4", true);
	AddChannel(D5Channel, "D5", true);
	AddChannel(D6Channel, "D6", true);
	AddChannel(D7Channel, "D7", true);
	

	return true;
}

void CameraAnalyzerSettings::UpdateInterfacesFromSettings()
{
	VSyncInterface->SetChannel(VSyncChannel);
	HSyncInterface->SetChannel( HSyncChannel );
	PCKInterface->SetChannel(PCKChannel);
	D0Interface->SetChannel(D0Channel);
	D1Interface->SetChannel(D1Channel);
	D2Interface->SetChannel(D2Channel);
	D3Interface->SetChannel(D3Channel);
	D4Interface->SetChannel(D4Channel);
	D5Interface->SetChannel(D5Channel);
	D6Interface->SetChannel(D6Channel);
	D7Interface->SetChannel(D7Channel);
	useVSyncInterface->SetValue(useVSync);
	useHSyncInterface->SetValue(useHSync);
	usePCKInterface->SetValue(usePCK);
	framesCountInterface->SetNumber(framesCount);
	/**
	mBitRateInterface->SetInteger( mBitRate );
	mBitsPerTransferInterface->SetNumber( mBitsPerTransfer );
	mStopBitsInterface->SetNumber( mStopBits );
	mParityInterface->SetNumber( mParity );
	mShiftOrderInterface->SetNumber( mShiftOrder );
	mInvertedInterface->SetNumber( mInverted );
	mUseAutobaudInterface->SetValue( mUseAutobaud );
	mSerialModeInterface->SetNumber( mSerialMode );
	*/
}

void CameraAnalyzerSettings::LoadSettings( const char* settings )
{
	SimpleArchive text_archive;
	text_archive.SetString( settings );

	const char* name_string;	//the first thing in the archive is the name of the protocol analyzer that the data belongs to.
	text_archive >> &name_string;
	if( strcmp( name_string, "SaleaeAsyncCameraAnalyzer" ) != 0 )
		AnalyzerHelpers::Assert( "SaleaeAsyncCameraAnalyzer: Provided with a settings string that doesn't belong to us;" );

	text_archive >> HSyncChannel;
	text_archive >> VSyncChannel;
	text_archive >> PCKChannel;
	text_archive >> D0Channel;
	text_archive >> D1Channel;
	text_archive >> D2Channel;
	text_archive >> D3Channel;
	text_archive >> D4Channel;
	text_archive >> D5Channel;
	text_archive >> D6Channel;
	text_archive >> D7Channel;
	text_archive >> useVSync;
	text_archive >> useHSync;
	text_archive >> usePCK;
	text_archive >> framesCount;
	
	text_archive >> mBitRate;
	text_archive >> mBitsPerTransfer;
	text_archive >> mStopBits;
	text_archive >> *(U32*)&mParity;
	text_archive >> *(U32*)&mShiftOrder;
	text_archive >> mInverted;

	//check to make sure loading it actual works befor assigning the result -- do this when adding settings to an anylzer which has been previously released.
	bool use_autobaud;
	if( text_archive >> use_autobaud )
		mUseAutobaud = use_autobaud;

	CameraAnalyzerEnums::Mode mode;
	if( text_archive >> *(U32*)&mode )
		mSerialMode = mode;

	ClearChannels();
	AddChannel( VSyncChannel, "VSync", true);
	AddChannel( HSyncChannel, "HSync", true );
	AddChannel(PCKChannel, "PCK", true);
	AddChannel(D0Channel, "D0", true);
	AddChannel(D1Channel, "D1", true);
	AddChannel(D2Channel, "D2", true);
	AddChannel(D3Channel, "D3", true);
	AddChannel(D4Channel, "D4", true);
	AddChannel(D5Channel, "D5", true);
	AddChannel(D6Channel, "D6", true);
	AddChannel(D7Channel, "D7", true);
	

	UpdateInterfacesFromSettings();
}

const char* CameraAnalyzerSettings::SaveSettings()
{
	SimpleArchive text_archive;

	text_archive << "SaleaeAsyncCameraAnalyzer";
	text_archive << HSyncChannel;
	text_archive << VSyncChannel;
	text_archive << PCKChannel;
	text_archive << D0Channel;
	text_archive << D1Channel;
	text_archive << D2Channel;
	text_archive << D3Channel;
	text_archive << D4Channel;
	text_archive << D5Channel;
	text_archive << D6Channel;
	text_archive << D7Channel;
	text_archive << useVSync;
	text_archive << useHSync;
	text_archive << usePCK;
	text_archive << framesCount;
	
	text_archive << mBitRate;
	text_archive << mBitsPerTransfer;
	text_archive << mStopBits;
	text_archive << mParity;
	text_archive << mShiftOrder;
	text_archive << mInverted;

	text_archive << mUseAutobaud;

	text_archive << mSerialMode;

	return SetReturnString( text_archive.GetString() );
}
