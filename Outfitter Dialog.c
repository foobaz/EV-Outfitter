/* Error codes for DoError() */
#define kMenuMissing 1
#define kDITLMissing 2
#define kTooBigNumber 13

/* number of items in base dialog */
#define itemsInShell 8
#define firstItem 9

/* these vars are used in many editing functions */
Str255 nameOfShip;
short page;
DialogPtr dialog;
extern Boolean stillGoing;
Boolean dialUp = false;

void Edit( FSSpec* a_file );
void Undo(void);
Boolean OpenResources( FSSpec* file );
void SwitchMenusOn(void);
void SwitchMenusOff(void);
void GotoPage(void);
void SavePage(void);
void SetupNew(void);
void MaxOut(void);
Boolean SaveAs(void);
Boolean Save( FSSpec* gameFile );
void SetTitle( Str63 pilotsName );
void Forklift( Rect* buttonRect );
/*void Violate(void);*/
void GetRidOfFileResources(void);
void DoMenu( long usersChoice );
void DoError( short theErr );

void Edit( FSSpec* gameFile )
{
	char key;
	long mItem;
	OSErr err;
	Point mouseLoc;
	WindowPtr aWin;
	EventRecord event;
	Rect pictRect;
	Handle pageCtl, iText;
	short newPage, iHit;
	
			/* read in file’s resources */
	if( OpenResources(gameFile) )
		return;
	
		/* Enable the now-useful menu items */
	SwitchMenusOn();
	
		/* Put dialog up with 1st (welcome) page */
	dialog = GetNewDialog( 196, nil, (WindowPtr) -1L );
	SetTitle(gameFile->name);
	ShowWindow(dialog);
	SelectWindow(dialog);
	SetPort(dialog);
	if( CountDITL(dialog) != itemsInShell )
		DoError(kDITLMissing);
	pageCtl = Get1Resource( 'DITL', 197 );
	AppendDITL( dialog, pageCtl, overlayDITL );
	UpdtDialog( dialog, dialog->visRgn );
	
			/* Get needed item info */
	GetDItem( dialog, 4, &iHit, &pageCtl, &pictRect );			// handle of page menu
	GetDItem( dialog, 7, &iHit, (Handle*)&mItem, &pictRect );	// rect for big button
	
	ValidRgn( dialog->visRgn );
	dialUp = true;
	page = 197;
	
		/* Carry out the user’s every desire until he saves or cancels */
	do
	{
		iHit = 0;
		do
		{
			//WaitNextEvent( everyEvent & ~highLevelEventMask, &event, 30, nil );
			if( EventAvail( highLevelEventMask, &event ) )
			{	// check for apple event, but leave in queue for main() to get
				if( *((long*)&event.where) == 'odoc' )
					ParamText( "\popen that new document", nil, nil, nil );
				else if( *((long*)&event.where) == 'quit' )
					ParamText( "\pquit", nil, nil, nil );
				else
					continue;
				goto confirm;
			}
			else
			{
				IsDialogEvent( &event );
				DialogSelect( &event, &aWin, &iHit );
			}
		}while( !WaitNextEvent( everyEvent & ~highLevelEventMask, &event, 16, nil ) );
		switch(event.what)
		{
			case mouseDown:
				switch( FindWindow( event.where, &aWin) )
				{
					case inContent:
						mouseLoc = event.where;
						GlobalToLocal(&mouseLoc);
						if( PtInRect( mouseLoc, &pictRect ) )
							Forklift(&pictRect);
						else
							if( IsDialogEvent( &event ) )
								DialogSelect( &event, &aWin, &iHit );
							break;
					case inMenuBar:
						mItem = MenuSelect(event.where);
						if( mItem >> 16 )
							DoMenu(mItem);
						if( !stillGoing )
						{
							ParamText( "\pquit", nil, nil, nil );
							goto confirm;
						}
						break;
					case inDrag:
						DragWindow( dialog, event.where, &qd.screenBits.bounds );
					case inSysWindow:
						SystemClick( &event, aWin );
						break;
				}
				break;
			case keyDown:
				key = event.message & charCodeMask;
				mItem = MenuKey(key);
				if( mItem >> 16 )
				{
					DoMenu(mItem);
					if( !stillGoing )
					{
						ParamText( "\pquit", nil, nil, nil );
						goto confirm;
					}
				}
				else if( key == '-' || key == '[' || key == 0x0B )
					iHit = 5;		// prev. page
				else if( key == '=' || key == '+' || key == ']' || key == 0x0C )
					iHit = 6;		// next page
				else if( key == 0x01 )
				{
					SavePage();
					page = 197;
					GotoPage();
				}
				else if( key < ':' && key > '/' || key < ' ' )
				{
					if( IsDialogEvent( &event ) )
						DialogSelect( &event, &aWin, &iHit );
				}
				break;
			case autoKey:
				break;
			case diskEvt:
				GetMouse(&mouseLoc);
				if( HiWord(event.message) )
					DIBadMount( mouseLoc, event.message );
				break;
			default:
				IsDialogEvent( &event );
				DialogSelect( &event, &aWin, &iHit );
				break;
		} // end event switch

	/* Handle a button / simulated button push */
		switch(iHit)
		{
		/* Bottom (save / cancel) buttons */
			case 1:
				dialUp = SaveAs();
				break;
			case 3:
				dialUp = Save(gameFile);
				break;
			case 2:
				dialUp = false;
				break;
				
		/* Top (page-change) buttons */
			case 4:	// Popup menu
				newPage = GetCtlValue( (ControlHandle)pageCtl);
				newPage += 196;
				if( page != newPage && newPage > 196 && newPage < 204 )
				{
					SavePage();
					page = newPage;
					GotoPage();
				}
				InvalRect( &( *(ControlHandle)pageCtl )->contrlRect );
				break;
			case 5:	// prev. page
				SavePage();
				page--;
				if( page == 196 )
					page = 203;
				GotoPage();
				SetCtlValue( (ControlHandle) pageCtl, page - 196 );
				InvalRect( &( *(ControlHandle)pageCtl )->contrlRect );
				break;
			case 6:	// next page
				SavePage();
				page++;
				if( page == 204 )
					page = 197;
				GotoPage();
				SetCtlValue( (ControlHandle) pageCtl, page - 196 );
				InvalRect( &( *(ControlHandle)pageCtl )->contrlRect );
				break;
		} // end item switch
		continue;
confirm:
		iHit = CautionAlert( 130, nil );
		switch( iHit )
		{
			case 1:	// Save As…
				dialUp = SaveAs();
				break;
			case 2:	// Don’t save
				dialUp = false;
				break;
			case 3:	// Save
				dialUp = Save(gameFile);
				break;
			case 4:	// Cancel
				stillGoing = true;
				if( event.what == kHighLevelEvent )
				GetNextEvent( highLevelEventMask, &event );
				break;
		}
		if( dialUp )
			stillGoing = true;
	} while( dialUp );

	GetRidOfFileResources();

	SwitchMenusOff();

	DisposDialog(dialog);
}

void GotoPage()
{
	Handle ditl;
	
	ShortenDITL( dialog, CountDITL(dialog) - itemsInShell );
	ditl = GetResource('DITL', page );
	if(!ditl)
		DoError(kDITLMissing);
	AppendDITL( dialog, ditl, overlayDITL );
	ReleaseResource(ditl);
	SetupNew();
}

void Undo()
{
	short howMany, iType, offset;
	Handle curTextItem;
	Rect iRect;
	Str255 baseTen;
	#include "xorCodes+offsets.h"
	extern Handle pilotData;
	
	howMany = ((DialogPeek)dialog)->editField + 1;
	GetDItem( dialog, howMany, &iType, &curTextItem, &iRect );
	if( page == 198 && howMany == 15 )
		howMany = *(long*)&(*pilotData)[0x11BA] ^ 0x4D143169;
	else
	{
		offset = offsets[page - 198][howMany - firstItem];
		howMany = ((short*)(*pilotData))[offset] ^ xorCodes[page - 198][ howMany - firstItem ];
	}
	NumToString( howMany, baseTen );
	SetIText( curTextItem, baseTen );
}

void SetupNew()
{
	long howMany;
	short itemKind, itemAt, offset;
	Handle theItem;
	Str255 baseTen;
	Rect crapRect;
	extern Handle pilotData;
	#include "xorCodes+offsets.h"
	
	page -= 198;
	itemAt = firstItem;
	GetDItem( dialog, itemAt, &itemKind, &theItem, &crapRect );
	while( itemKind & editText )
	{
		offset = offsets[page][itemAt - firstItem];
		howMany = ((short*)(*pilotData))[offset] ^ xorCodes[page][itemAt - firstItem];
		if( howMany > 0 )
		{
			NumToString( howMany, baseTen );
			SetIText( theItem, baseTen );
		}
		else
			SetIText( theItem, "\p" );
		itemAt++;
		GetDItem( dialog, itemAt, &itemKind, &theItem, &crapRect );
	}
	if(!page)
	{
		howMany = ((short*)(*pilotData))[1] ^ 0x12B4;
		howMany -= 127;
		if( howMany < 27 && howMany > 0 )	// normal ship
			SetCtlValue( (ControlHandle)theItem, howMany );
		else if( howMany == 64 )	// escape pod
			SetCtlValue( (ControlHandle)theItem, 28 );
		else	// unknown ship
			SetCtlValue( (ControlHandle)theItem, 27 );
		GetDItem( dialog, ++itemAt, &itemKind, &theItem, &crapRect );
		howMany = *(long*)&(*pilotData)[0x11BA] ^ 0x4D143169;
		if( howMany )
		{
			NumToString( howMany, baseTen );
			SetIText( theItem, baseTen );
		}
		else
			SetIText( theItem, "\p" );
	}
	page += 198;
}

void SavePage()
{
	long howMany;
	short itemKind, itemAt;
	Handle textItem;
	Str255 text;
	Rect crapRect;
	Boolean tooBig = false;
	extern Handle pilotData;
	#include "xorCodes+offsets.h"
	
	page -= 198;
	itemAt = firstItem;
	GetDItem( dialog, itemAt, &itemKind, &textItem, &crapRect );
	while( itemKind & editText )
	{
		GetIText( textItem, text );
		if( text[0] )
			StringToNum( text, &howMany );
		else
			howMany = 0;
		if( howMany > 32767 )
		{
			tooBig = true;
			howMany = 32767;
		}
		howMany ^= xorCodes[page][itemAt - firstItem];
		((short*)(*pilotData))[ offsets[page][itemAt - firstItem] ] = howMany;
		itemAt++;
		GetDItem( dialog, itemAt, &itemKind, &textItem, &crapRect );
	}
	if( page == 0 )
	{
		howMany = GetCtlValue( (ControlHandle) textItem );
		if( howMany == 28 )
			howMany = 64;
		else if( howMany == 27 )	// after calculations, howMany is 0 to bypass the…
			howMany = 4661;	// …write-to-file. I didn’t create ship and won’t mess with it.
		howMany += 127;	// get ship resource ID form menu item…
		howMany ^= 0x12B4;	// and xor with magical ship code
		if(howMany)	// the bypass mechanism
			((short*)(*pilotData))[1] = howMany;
		itemAt++;
		GetDItem( dialog, itemAt, &itemKind, &textItem, &crapRect );
		GetIText( textItem, text );
		if( text[0] )
			StringToNum( text, &howMany );
		else
			howMany = 0;
		if( *(long*)&text[0] > '\n214' )
		{
			tooBig = true;
			howMany = 2147483647;
		}
		else if( *(long*)&text[0] == '\n214' && *(long*)&text[4] > '7483' )
		{
			tooBig = true;
			howMany = 2147483647;
		}
		else if( *(long*)&text[0] == '\n214' && *(long*)&text[4] == '7483' && *(long*)&text[4] > '7483' )
		{
			tooBig = true;
			howMany = 2147483647;
		}
		else if( *(long*)&text[0] == '\n214' && *(long*)&text[4] == '7483' && *(long*)&text[4] == '7483' &&  *(long*)&text[8] >= '648\0' )
		{
			tooBig = true;
			howMany = 2147483647;
		}
		howMany ^= 0x4D143169;
		*(long*)&(*pilotData)[0x11BA] = howMany;
	}
	if(tooBig)
		DoError(kTooBigNumber);
	page += 198;
}

void SetTitle( Str63 pilotName )
{
	Str255 dlogTitle;
	
	BlockMoveData( pilotName, dlogTitle, pilotName[0] + 1);
	if( dlogTitle[0] + nameOfShip[0] < 254 )
	{
		dlogTitle[0]++;
		dlogTitle[ dlogTitle[0] ] = '-';
		BlockMoveData( &(nameOfShip[1]), &( dlogTitle[ dlogTitle[0] + 1 ] ), nameOfShip[0] );
		dlogTitle[0] += nameOfShip[0];
	}
	SetWTitle( dialog, dlogTitle );
}

void MaxOut(void)
{
	const char	maxMoney[10] = "2147483647",
				maxNorm[5] = "32767",
				maxCapacitors[3] = "851",
				maxBoosters[3] = "127";
	short crap;
	Handle changedItem;
	Rect updtRect;
	EventRecord updtEvent;
	DialogPtr theDlg;
	#define curItem ((DialogPeek)dialog)->editField
	#define curText ((DialogPeek)dialog)->textH
	
	if( page == 197 )
		return;
	if( page == 198 && curItem == 14 )	// money sp. case
		TESetText( &maxMoney, 10, curText );
	else if( page == 199 && curItem == 10 )
		TESetText( &maxCapacitors, 3, curText );
	else if( page == 199 && curItem == 11 )
		TESetText( &maxBoosters, 3, curText );
	else
		TESetText( &maxNorm, 5, curText );
	GetDItem( dialog, curItem + 1, &crap, &changedItem, &updtRect );
	InvalRect(&updtRect);
	if( WaitNextEvent( updateMask, &updtEvent, 0, nil ) )
	{
		IsDialogEvent( &updtEvent );
		DialogSelect( &updtEvent, &theDlg, &crap );
	}
}

void Forklift( Rect* pictRect )
{
	short iHit, iType;
	DialogPtr modal;
	Handle textItem;
	long forklifts;
	Str15 text;
	Rect iRect;
	Point mouseLoc;
	Boolean BtnUp = 0;
	CIconHandle up, down;
	extern Handle pilotData;
	
	up = GetCIcon(128);
	if(!up)
		DoError(kDITLMissing);
	DetachResource( (Handle)up );
	HLock( (Handle)up );
	down = GetCIcon(129);
	if(!up)
		DoError(kDITLMissing);
	DetachResource( (Handle)down );
	HLock( (Handle)down );
	while( StillDown() )
	{
		GetMouse(&mouseLoc);
		if( PtInRect( mouseLoc, pictRect ) )
		{
			if(!BtnUp)
			{
				PlotCIcon( pictRect, down );
				BtnUp = 1;
			}
		}
		else
		{
			if(BtnUp)
			{
				PlotCIcon( pictRect, up );
				BtnUp = 0;
			}
		}
	}
	if( !PtInRect( mouseLoc, pictRect ) )	// user wussed out
	{
		PlotCIcon( pictRect, up );
		HUnlock( (Handle)up );
		DisposHandle( (Handle)up );
		HUnlock( (Handle)down );
		DisposHandle( (Handle)down );
		return;
	}

	modal = GetNewDialog( 195, nil, (WindowPtr) -1L );
	GetDItem( modal, 4, &iType, &textItem, &iRect );
	forklifts = ((short*)(*pilotData))[0x0880] ^ 0x5E2E;
	if(forklifts)
	{
		NumToString( forklifts, text );
		SetIText( textItem, text );
	}
	do
	{
		ModalDialog( nil, &iHit );
/*		if( iHit == 3 )*/
/*			Violate();*/		//prep ev data
	} while( iHit > 2 );
	if( iHit == 1 )
	{
		GetIText( textItem, text );
		if( text[0] )
			StringToNum( text, &forklifts );
		else
			forklifts = 0;
		forklifts ^= 0x5E2E;
		((short*)(*pilotData))[0x0880] = forklifts;
	}
	DisposDialog(modal);
	PlotCIcon( pictRect, up );
	HUnlock( (Handle)up );
	DisposHandle( (Handle)up );
	HUnlock( (Handle)down );
	DisposHandle( (Handle)down );
}

void DoMenu( long choice )
{
	Str255 name;
	
	switch(choice)
	{
		case 0:
			return;
		case 0x10001:	// about…
about:
			if( NoteAlert( 131, nil ) == 3 )
				goto help;
			break;
		case 0x10002:	// help…
help:
			if( NoteAlert( 132, nil ) == 3 )
				goto about;
			break;
		case 0x20002:	// cancel…
			dialUp = false;
			break;
		case 0x20004:	// save as…
			dialUp = SaveAs();
			break;
		case 0x20005:	// revert page
			SetupNew();
			break;
		case 0x20007:	// quit
			stillGoing = false;
			break;
		case 0x30001:	// undo
			Undo();
			break;
		case 0x30003:	// cut
			DlgCut( dialog );
			break;
		case 0x30004:	// copy
			DlgCopy( dialog );
			break;
		case 0x30005:	// paste
			DlgPaste( dialog );
			break;
		case 0x30006:	// clear
			DlgDelete( dialog );
			break;
		case 0x30007:	// select all
			TESetSelect( 0, 32767, ((DialogPeek) dialog)->textH );
			break;
		case 0x30008:	// maximum
			MaxOut();
			break;
		default:
			if( choice < 0x20000 )
			{
				GetItem( GetMHandle(1), choice >> 16, name );
				OpenDeskAcc(name);
				SetPort(dialog);
			}
			break;
	}
	HiliteMenu(0);
}