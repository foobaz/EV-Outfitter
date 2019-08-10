/* DoError() error codes */
#define kMenuMissing 1
#define kDITLMissing 2
#define kCantReadAE 3
#define kCorrupAE 4
#define kBadFileList 5
#define kBadFileDesc 6
#define kCantSendAE 11
#define kTooManyFiles 12

void InitToolbox(void);
void MenuInit(void);
void DoError( short theErr );
void SwitchMenusOn(void);
void SwitchMenusOff(void);
void AEInit(void);
pascal OSErr ManualOpen( AppleEvent theAppleEvent, AppleEvent reply, long refCon );
pascal OSErr CoolOpen( AppleEvent theAppleEvent, AppleEvent reply, long refCon );
pascal OSErr AEQuit( AppleEvent theAppleEvent, AppleEvent reply, long refCon );
OSErr GotRequiredParams( AppleEvent* appleEventPtr );
void Edit( FSSpec* a_file );
void DoError( short theErr );
void OpenGame(void);

/*void About()
{
	GWorldPtr textWin;
	WindowPtr skewed;
	TEHandle theStory;
	Handle textRsrc;
	Rect srcRect = { 0, 0, 256, 196 }, destRect = {0,0,0,0};
		
	if( NewGWorld( &textWin, 1, &srcRect, nil, nil, 0 ) )
		return;
	theStory = TEStylNew( &textWin->portRect, &textWin->portRect );
	textRsrc = GetResource( 'TEXT', 128 );
	TESetText( *textRsrc, GetHandleSize(textRsrc), theStory );
	TESetJust( teCenter, theStory );
	EraseRect(&textWin->portRect);
	InvalRect(&textWin->portRect);
	TEUpdate( &textWin->portRect, theStory );
	ValidRect(&textWin->portRect);
	skewed = NewCWindow( nil, &srcRect, "\p", false, plainDBox, (WindowPtr)-1, false, 0 );
	if(!skewed)
		return;
	for( srcRect.bottom = 1; !Button() & (srcRect.bottom < 256); srcRect.bottom++ )
	{
		for( rowAt = 0; rowAt < srcRect.bottom; ++rowAt )
		{
			CopyBits( &textWin.portBits, &skewed.portBits, &srcRect, &destRect, tMode, maskRgn );
		}
	}
}
*/
void InitToolbox()
{
	InitGraf((Ptr) &qd.thePort);
	InitFonts();
	InitWindows();
	InitMenus();
	FlushEvents(everyEvent,0);
	TEInit();
	InitDialogs(0L);
	InitCursor();
}

void MenuInit()
{
	Handle mBar;
	
	mBar = GetNewMBar(128);
	if( mBar == nil )
		DoError( kMenuMissing );
	SetMenuBar(mBar);
	DisposHandle(mBar);
	AddResMenu( GetMHandle(1), 'DRVR' );
	DrawMenuBar();
}

void DoError( short theErr )
{
	short button;
	Str255 theMessage;
	
	SetCursor(&qd.arrow);
	GetIndString( theMessage, 128, theErr );
	ParamText( theMessage, nil, nil, nil );
	button = StopAlert( 129, nil );
	if( theErr < 3 )		// A missing resource error
		ExitToShell();
}

void SwitchMenusOn(void)
{
	MenuHandle menu;
	
	menu = GetMHandle(2);
	DisableItem( menu, 1);
	EnableItem( menu, 2);
	EnableItem( menu, 3);
	EnableItem( menu, 4);
	EnableItem( menu, 5);
	menu = GetMHandle(3);
	EnableItem( menu, 1);
	EnableItem( menu, 3);
	EnableItem( menu, 4);
	EnableItem( menu, 5);
	EnableItem( menu, 6);
	EnableItem( menu, 7);
	EnableItem( menu, 8);
	DrawMenuBar();
}

void SwitchMenusOff(void)
{
	MenuHandle menu;
	
	menu = GetMHandle(2);
	EnableItem( menu, 1);
	DisableItem( menu, 2);
	DisableItem( menu, 3);
	DisableItem( menu, 4);
	DisableItem( menu, 5);
	menu = GetMHandle(3);
	DisableItem( menu, 1);
	DisableItem( menu, 3);
	DisableItem( menu, 4);
	DisableItem( menu, 5);
	DisableItem( menu, 6);
	DisableItem( menu, 7);
	DisableItem( menu, 8);
	DrawMenuBar();
}

void AEInit()
{
	AEEventHandlerUPP handler;
	OSErr err;
	
	handler = NewAEEventHandlerProc( ManualOpen );
	err = AEInstallEventHandler('aevt', 'oapp', handler, 0L, false);
	handler = NewAEEventHandlerProc( CoolOpen );
	err = AEInstallEventHandler('aevt', 'odoc', handler, 0L, false);
	handler = NewAEEventHandlerProc( AEQuit );
	err = AEInstallEventHandler('aevt', 'quit', handler, 0L, false);
}

pascal OSErr ManualOpen( AppleEvent theEvent, AppleEvent answer, long long_ago )
{
	PostEvent( keyDown, (long)0x0000006F );
}

pascal OSErr AEQuit( AppleEvent theEvent, AppleEvent reply, long john_silver )
{
	extern Boolean stillGoing;

	stillGoing = false;
}

pascal OSErr CoolOpen( AppleEvent theEvent, AppleEvent reply, long howMany )
{
	AEDescList specs;
	FSSpec file;
	DescType type;
	Size size;
	long fileAt;
	AEKeyword wyrd;
	OSErr err;
	
	err = AEGetParamDesc( &theEvent, keyDirectObject, typeAEList, &specs );
		/*   error  */	if( err != noErr ){
		/* handling */		DoError( kCantReadAE );
		/*   crap   */		return( err );}
	err = GotRequiredParams( &theEvent );
		/* more  */	if( err != noErr ){
		/* error */		err = AEDisposeDesc(&specs);
		/*  crap */		DoError( kCorrupAE );
							return( err );}

	err = AECountItems( &specs, &howMany );
		/*  yet */	if( err != noErr ){
		/*	more */		DoError( kBadFileList );
		/* crap */		return( err );}
	
	if( howMany != 1 )
		DoError(kTooManyFiles);
	
	err = AEGetNthPtr( &specs, 1, typeFSS, &wyrd, &type,
					  (Ptr)&file, sizeof( FSSpec ), &size );
		/*   you   */	if( err != noErr ){
		/* guessed */		DoError( kBadFileDesc );
		/* it-crap */		return( err );}
	Edit( &file );
	return AEDisposeDesc(&specs);
}

OSErr GotRequiredParams( AppleEvent *evtPtr )
{
	DescType returnedType;
	Size realSize;
	OSErr err;
	
	err = AEGetAttributePtr( evtPtr, keyMissedKeywordAttr, typeWildCard,
								&returnedType, nil, 0, &realSize );
	if( err == errAEDescNotFound )
		return( noErr );
	else if( err == noErr )
		return( errAEEventNotHandled );
	else return( err );
}