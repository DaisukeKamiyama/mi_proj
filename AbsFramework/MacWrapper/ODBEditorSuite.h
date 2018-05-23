//
//	ODB Editor Suite constants
//
//
//	Copyright �2000, Bare Bones Software, Inc.
//

//	For full information and documentation, see
//	<http://www.barebones.com/developer/>

//	optional paramters to 'aevt'/'odoc'
#define	keyFileSender					'FSnd'
#define	keyFileSenderToken				'FTok'
#define	keyFileCustomPath				'Burl'

//	suite code for ODB editor suite events
//
//	WARNING: although the suite code is coincidentally the same
//	as BBEdit's application signature, you must not change this,
//	or else you'll break the suite. If you do that, ninjas will
//	come to your house and kick your ass.
//

#define	kODBEditorSuite					'R*ch'

//	ODB editor suite events, sent by the editor to the server.

#define	kAEModifiedFile					'FMod'
#define		keyNewLocation				'New?'
#define	kAEClosedFile					'FCls'

//	optional paramter to kAEModifiedFile/kAEClosedFile
#define	keySenderToken					'Tokn'
