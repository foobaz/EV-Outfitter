/* Error codes for DoError() */
#define kDITLMissing 2
#define kCantOpenRF 7
#define kNotEVFile 8
#define kBadSize 9
#define kCantWrite 10
#define kCantDelete 14

/* right size for game file resources */
#define kPilotSize 9646
#define kShipSize 8958

Handle pilotData, shipData;
extern Str255 nameOfShip;

Boolean OpenResources( FSSpec* file );
void OpenGame(void);
Boolean SaveAs(void);
Boolean Save( FSSpec* gameFile );
void SavePage(void);
void Violate(void);
void GetRidOfFileResources(void);
void Edit( FSSpec* a_file );
void DoError( short theErr );

Boolean OpenResources( FSSpec* file )
{
	short gameRefNum, myRefNum, ID;
	Size size;
	ResType type;
	
	myRefNum = CurResFile();
	gameRefNum = FSpOpenResFile( file, fsRdPerm );
	if( gameRefNum == -1 )
	{
		DoError(kCantOpenRF);
		return true;
	}
	UseResFile(gameRefNum);
	pilotData = Get1Resource( 'MpïL', 128 );
	if( pilotData == nil )
	{
		DoError(kNotEVFile);
		CloseResFile(gameRefNum);
		return true;
	}
	shipData = Get1Resource( 'MpïL', 129 );
	if( shipData == nil )
	{
		DoError(kNotEVFile);
		return true;
	}
	GetResInfo( shipData, &ID, &type, nameOfShip );
	DetachResource(pilotData);
	DetachResource(shipData);
	CloseResFile(gameRefNum);
	size = GetHandleSize(pilotData);
	if( size != kPilotSize )
		DoError(kBadSize);
	UseResFile(myRefNum);
	return false;
}

Boolean SaveAs()
{
	StandardFileReply new;
	FSSpec* gameFile;
	short gameRefNum;
	OSErr err;
	
	SavePage();
	StandardPutFile( "\pNew pilot/file name:", "\pAmanda Huginkis", &new );
	if( !new.sfGood )
		return true;
	gameFile = &new.sfFile;
	if( new.sfReplacing )
	{
		if( FSpDelete(gameFile) )
		{
			DoError(kCantDelete);
			return;
		}
	}
	gameFile = &new.sfFile;
	FSpCreateResFile( gameFile, 'Mërc', 'MpïL', 0L );
	gameRefNum = FSpOpenResFile( gameFile, fsRdWrPerm );
	if( gameRefNum == -1 )
	{
		DoError(kCantWrite);
		err = FSpDelete(gameFile);
		return true;
	}
	
	AddResource( pilotData, 'MpïL', 128 ,"\pPilot Data" );
	if( ResError() )
	{
		DoError(kCantWrite);
		err = FSpDelete(gameFile);
		return true;
	}
	DetachResource(pilotData);

	AddResource( shipData, 'MpïL', 129 ,nameOfShip );
	if( ResError() )
	{
		DoError(kCantWrite);
		err = FSpDelete(gameFile);
		return true;
	}
	
	DetachResource(shipData);
	
	CloseResFile(gameRefNum);
	if( ResError() ){
		DoError(kCantWrite);
		err = FSpDelete(gameFile);
		return true;
	}
	
	return false;
}

Boolean Save( FSSpec* gameFile )
{
	short gameRefNum;
	Handle oldPilot;
	
	SavePage();
	gameRefNum = FSpOpenResFile( gameFile, fsRdWrPerm );
	if( gameRefNum == -1 )
	{
		DoError(kCantWrite);
		return true;
	}
	
	oldPilot = Get1Resource( 'MpïL', 128 );
	if( oldPilot == nil )
	{
		DoError(kNotEVFile);
		CloseResFile(gameRefNum);
		return true;
	}
	BlockMove( *pilotData, *oldPilot, kPilotSize );
	ChangedResource(oldPilot);
	CloseResFile(gameRefNum);
	if( ResError() )
	{
		DoError(kCantWrite);
		return true;
	}
	
	return false;
}

void OpenGame()
{
	FSSpec file;
	short button;
	SFTypeList type;
	StandardFileReply reply;
	
	HiliteMenu(0);
	type[0] = 'MpïL';
	StandardGetFile( nil, 1, type, &reply );
	if( !reply.sfGood )
		return;
	Edit( &reply.sfFile );
}
/*
void Violate()
{
	StandardFileReply reply;
	SFTypeList type;
	Handle forklift;
	short dataRefNum, rID;
	ResType rType;
	Str255 rName;

	type[0] = 'MDät';
	StandardGetFile( nil, 1, type, &reply );
	dataRefNum = FSpOpenResFile( &reply.sfFile, fsRdWrPerm );
	if( !reply.sfGood )
		return;
	if( dataRefNum == -1 )
	{
		DoError(kEVOpen);
		return;
	}
	forklift = Get1Resource( 'wëap', 191 );
	if( forklift == nil )
	{
		DoError(kPreMessed);
		return;
	}
	GetResInfo( forklift, &rID, &rType, rName );
	SetResInfo( forklift, 163, rName );
	ReleaseResource(forklift);
	CloseResFile( dataRefNum );
}
*/
void GetRidOfFileResources(void)
{
	DisposHandle(pilotData);
	DisposHandle(shipData);
}