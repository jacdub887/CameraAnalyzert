#ifndef CAMERA_ANALYZER_SETTINGS
#define CAMERA_ANALYZER_SETTINGS

#include <AnalyzerSettings.h>
#include <AnalyzerTypes.h>

namespace CameraAnalyzerEnums
{
	enum Mode { Normal, MpModeMsbZeroMeansAddress, MpModeMsbOneMeansAddress };
};

class CameraAnalyzerSettings : public AnalyzerSettings
{
public:
	CameraAnalyzerSettings();
	virtual ~CameraAnalyzerSettings();

	virtual bool SetSettingsFromInterfaces();
	void UpdateInterfacesFromSettings();
	virtual void LoadSettings( const char* settings );
	virtual const char* SaveSettings();

	
	Channel VSyncChannel;
	Channel HSyncChannel;
	Channel PCKChannel;
	Channel D0Channel;
	Channel D1Channel;
	Channel D2Channel;
	Channel D3Channel;
	Channel D4Channel;
	Channel D5Channel;
	Channel D6Channel;
	Channel D7Channel;
	U32 PCKHz;
	U32 width, height, frames, dataChannels;
	bool useVSync, useHSync, usePCK;
	int framesCount;
	
	
	U32 mBitRate;
	U32 mBitsPerTransfer;
	AnalyzerEnums::ShiftOrder mShiftOrder;
	double mStopBits;
	AnalyzerEnums::Parity mParity;
	bool mInverted;
	bool mUseAutobaud;
	CameraAnalyzerEnums::Mode mSerialMode;

protected:
	std::auto_ptr< AnalyzerSettingInterfaceChannel >	VSyncInterface;
	std::auto_ptr< AnalyzerSettingInterfaceChannel >	HSyncInterface;
	std::auto_ptr< AnalyzerSettingInterfaceChannel >	PCKInterface;
	std::auto_ptr< AnalyzerSettingInterfaceChannel >	D0Interface;
	std::auto_ptr< AnalyzerSettingInterfaceChannel >	D1Interface;
	std::auto_ptr< AnalyzerSettingInterfaceChannel >	D2Interface;
	std::auto_ptr< AnalyzerSettingInterfaceChannel >	D3Interface;
	std::auto_ptr< AnalyzerSettingInterfaceChannel >	D4Interface;
	std::auto_ptr< AnalyzerSettingInterfaceChannel >	D5Interface;
	std::auto_ptr< AnalyzerSettingInterfaceChannel >	D6Interface;
	std::auto_ptr< AnalyzerSettingInterfaceChannel >	D7Interface;
	std::auto_ptr< AnalyzerSettingInterfaceBool >	useVSyncInterface;
	std::auto_ptr< AnalyzerSettingInterfaceBool >	useHSyncInterface;
	std::auto_ptr< AnalyzerSettingInterfaceBool >	usePCKInterface;
	std::auto_ptr< AnalyzerSettingInterfaceNumberList > framesCountInterface;
	
	/**
	std::auto_ptr< AnalyzerSettingInterfaceInteger >	mBitRateInterface;
	std::auto_ptr< AnalyzerSettingInterfaceNumberList > mBitsPerTransferInterface;
	std::auto_ptr< AnalyzerSettingInterfaceNumberList >	mShiftOrderInterface;
	std::auto_ptr< AnalyzerSettingInterfaceNumberList >	mStopBitsInterface;
	std::auto_ptr< AnalyzerSettingInterfaceNumberList >	mParityInterface;
	std::auto_ptr< AnalyzerSettingInterfaceNumberList >	mInvertedInterface;
	std::auto_ptr< AnalyzerSettingInterfaceBool >	mUseAutobaudInterface;
	std::auto_ptr< AnalyzerSettingInterfaceNumberList >	mSerialModeInterface;
	*/
};

#endif //CAMERA_ANALYZER_SETTINGS
