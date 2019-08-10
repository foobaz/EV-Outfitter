/* error codes for DoError() */
#define kCantReadAE 3
#define kBadWindow 14

/* set to true until user decides to quit */
Boolean stillGoing = true;

void InitToolbox(void);
void MenuInit(void);
void AEInit(void);
void DoMenu( long usersChoice );
void DoError( short theErr );
void OpenGame(void);

void main()
{
	char key;
	OSErr err;
	short part;
	long choice;
	WindowPtr wind;
	EventRecord event;
	Point pnt = { -1, -1 };
	
	MaxApplZone();
	MoreMasters();
	InitToolbox();
	
	err = Gestalt( 'sysv', &choice );
	
	if( err || choice < 0x0700 )
	{
			/* Bring app to front under MultiFinder */
		for( choice = 3; choice < 0; choice-- )
			EventAvail( everyEvent, &event );

			/* Display 'sick bastard' alert */
		StopAlert( 128, nil );
		ExitToShell();
	}
	
	MenuInit();
	AEInit();
	while(stillGoing)
	{
		WaitNextEvent( everyEvent, &event, 0x7FFFFFFF, nil );
		switch( event.what )
		{
			case mouseDown:
				part = FindWindow( event.where, &wind );
				if( part == inMenuBar )
				{
					choice = MenuSelect(event.where);
					if( choice == 0x20001 )	// openÉ
						OpenGame();
					else
						DoMenu(choice);
				}
				break;
			case keyDown:
				key = event.message & charCodeMask;
				choice = MenuKey(key);
				if( choice == 0x20001 )	// openÉ
					OpenGame();
				else
					DoMenu(choice);
				break;
			case kHighLevelEvent:
				err = AEProcessAppleEvent(&event);
				break;
			case diskEvt:
				if( HiWord(event.message) )
					DIBadMount( pnt, event.message );
				break;
		}
	}
}