#ifndef CAMERA_ANALYZER_RESULTS
#define CAMERA_ANALYZER_RESULTS

#include <AnalyzerResults.h>

#define FRAMING_ERROR_FLAG ( 1 << 0 )
#define PARITY_ERROR_FLAG ( 1 << 1 )
#define MP_MODE_ADDRESS_FLAG ( 1 << 2 )

class CameraAnalyzer;
class CameraAnalyzerSettings;

class CameraAnalyzerResults : public AnalyzerResults
{
public:
	CameraAnalyzerResults( CameraAnalyzer* analyzer, CameraAnalyzerSettings* settings );
	virtual ~CameraAnalyzerResults();

	virtual void GenerateBubbleText( U64 frame_index, Channel& channel, DisplayBase display_base );
	virtual void GenerateExportFile( const char* file, DisplayBase display_base, U32 export_type_user_id );

	virtual void GenerateFrameTabularText(U64 frame_index, DisplayBase display_base );
	virtual void GeneratePacketTabularText( U64 packet_id, DisplayBase display_base );
	virtual void GenerateTransactionTabularText( U64 transaction_id, DisplayBase display_base );

	void AddPictureByte(U8);
	void ResetPointer();
	
protected: //functions

protected:  //vars
	CameraAnalyzerSettings* mSettings;
	CameraAnalyzer* mAnalyzer;
	U8 pictureData [16000000];
	U32 pictureDataPTR;
};

#endif //CAMERA_ANALYZER_RESULTS
