// ===================================================================
//
// Description		
//		Contains the implementation of CreateRoomRequest
//
// Revision history
//		Date			Description
//		30-Nov-2012		First version
//
// ===================================================================
#include "CreateRoomRequest.h"

namespace Sfs2X {
namespace Requests {

boost::shared_ptr<string> CreateRoomRequest::KEY_ROOM (new string("r"));
boost::shared_ptr<string> CreateRoomRequest::KEY_NAME (new string("n"));
boost::shared_ptr<string> CreateRoomRequest::KEY_PASSWORD (new string("p"));
boost::shared_ptr<string> CreateRoomRequest::KEY_GROUP_ID (new string("g"));
boost::shared_ptr<string> CreateRoomRequest::KEY_ISGAME (new string("ig"));
boost::shared_ptr<string> CreateRoomRequest::KEY_MAXUSERS (new string("mu"));
boost::shared_ptr<string> CreateRoomRequest::KEY_MAXSPECTATORS (new string("ms"));
boost::shared_ptr<string> CreateRoomRequest::KEY_MAXVARS (new string("mv"));
boost::shared_ptr<string> CreateRoomRequest::KEY_ROOMVARS (new string("rv"));
boost::shared_ptr<string> CreateRoomRequest::KEY_PERMISSIONS (new string("pm"));
boost::shared_ptr<string> CreateRoomRequest::KEY_EVENTS (new string("ev"));
boost::shared_ptr<string> CreateRoomRequest::KEY_EXTID (new string("xn"));
boost::shared_ptr<string> CreateRoomRequest::KEY_EXTCLASS (new string("xc"));
boost::shared_ptr<string> CreateRoomRequest::KEY_EXTPROP (new string("xp"));
boost::shared_ptr<string> CreateRoomRequest::KEY_AUTOJOIN (new string("aj"));
boost::shared_ptr<string> CreateRoomRequest::KEY_ROOM_TO_LEAVE (new string("rl"));

// -------------------------------------------------------------------
// Constructor
// -------------------------------------------------------------------
CreateRoomRequest::CreateRoomRequest(boost::shared_ptr<RoomSettings> settings, bool autoJoin, boost::shared_ptr<Room> roomToLeave)
	: BaseRequest (RequestType_CreateRoom)
{
	Init(settings, autoJoin, roomToLeave);
}

// -------------------------------------------------------------------
// Constructor
// -------------------------------------------------------------------
CreateRoomRequest::CreateRoomRequest(boost::shared_ptr<RoomSettings> settings, bool autoJoin)
	: BaseRequest (RequestType_CreateRoom)
{
	Init(settings, autoJoin, boost::shared_ptr<Room>());
}

// -------------------------------------------------------------------
// Constructor
// -------------------------------------------------------------------
CreateRoomRequest::CreateRoomRequest(boost::shared_ptr<RoomSettings> settings)
	: BaseRequest (RequestType_CreateRoom)
{
	Init(settings, false, boost::shared_ptr<Room>());
}

// -------------------------------------------------------------------
// Destructor
// -------------------------------------------------------------------
CreateRoomRequest::~CreateRoomRequest()
{
}

// -------------------------------------------------------------------
// Init
// -------------------------------------------------------------------
void CreateRoomRequest::Init(boost::shared_ptr<RoomSettings> settings, bool autoJoin, boost::shared_ptr<Room> roomToLeave)
{
	this->settings = settings;
	this->autoJoin = autoJoin;
	this->roomToLeave = roomToLeave;
}

// -------------------------------------------------------------------
// Validate
// -------------------------------------------------------------------
void CreateRoomRequest::Validate(boost::shared_ptr<SmartFox> sfs)
{
	boost::shared_ptr<list<string> > errors (new list<string>());

	if (settings->Name()->empty()  || settings->Name()->size() == 0)
		errors->push_back("Missing room name");
				
	if (settings->MaxUsers() <= 0)
		errors->push_back("maxUsers must be > 0");
			
	if (settings->Extension() != NULL)	{
		if (settings->Extension()->ClassName() == NULL || settings->Extension()->ClassName()->empty() || settings->Extension()->ClassName()->size() == 0)
			errors->push_back("Missing Extension class name");
					
		if (settings->Extension()->Id() == NULL || settings->Extension()->Id()->empty() || settings->Extension()->Id()->size() == 0)
			errors->push_back("Missing Extension id");
	}
			
	if (errors->size() > 0)
	{
		boost::shared_ptr<string> message (new string("CreateRoom request error"));
		boost::shared_ptr<SFSValidationError> exception(new SFSValidationError(message, errors));
		throw exception;
	}
}

// -------------------------------------------------------------------
// Execute
// -------------------------------------------------------------------
void CreateRoomRequest::Execute (boost::shared_ptr<SmartFox> sfs)
{
	sfso->PutUtfString(KEY_NAME, settings->Name());
	sfso->PutUtfString(KEY_GROUP_ID, settings->GroupId());
	sfso->PutUtfString(KEY_PASSWORD, settings->Password());

	boost::shared_ptr<bool> valueIsGame (new bool());
	*valueIsGame = settings->IsGame();
	sfso->PutBool(KEY_ISGAME, valueIsGame);
	
	boost::shared_ptr<short int> valueMaxUsers (new short int());
	*valueMaxUsers = settings->MaxUsers();
	sfso->PutShort(KEY_MAXUSERS, valueMaxUsers); 

	boost::shared_ptr<short int> valueMaxSpectators (new short int());
	*valueMaxSpectators = settings->MaxSpectators();
	sfso->PutShort(KEY_MAXSPECTATORS, valueMaxSpectators); 

	boost::shared_ptr<short int> valueMaxVariables (new short int());
	*valueMaxVariables = settings->MaxVariables();
	sfso->PutShort(KEY_MAXVARS, valueMaxVariables);
			
	// Handle Room Variables
	if (settings->Variables() != NULL && settings->Variables()->size() > 0)
	{
		boost::shared_ptr<ISFSArray> roomVars = SFSArray::NewInstance();
	
		vector<boost::shared_ptr<RoomVariable> >::iterator iterator;
		for (iterator = settings->Variables()->begin(); iterator != settings->Variables()->end(); iterator++)
		{
			boost::shared_ptr<RoomVariable> item = (*iterator);
			roomVars->AddSFSArray(item->ToSFSArray());
		}
				
		sfso->PutSFSArray(KEY_ROOMVARS, roomVars);
	}
			
	// Handle Permissions	
	if (settings->Permissions() != NULL)
	{
		boost::shared_ptr<vector<boost::shared_ptr<bool> > > sfsPermissions (new vector<boost::shared_ptr<bool> >());

		boost::shared_ptr<bool> valueAllowNameChange (new bool());
		*valueAllowNameChange = settings->Permissions()->AllowNameChange();
		sfsPermissions->push_back(valueAllowNameChange);

		boost::shared_ptr<bool> valueAllowPasswordStateChange (new bool());
		*valueAllowPasswordStateChange = settings->Permissions()->AllowPasswordStateChange();
		sfsPermissions->push_back(valueAllowPasswordStateChange);

		boost::shared_ptr<bool> valueAllowPublicMessages (new bool());
		*valueAllowPublicMessages = settings->Permissions()->AllowPublicMessages();
		sfsPermissions->push_back(valueAllowPublicMessages);

		boost::shared_ptr<bool> valueAllowResizing (new bool());
		*valueAllowResizing = settings->Permissions()->AllowResizing();
		sfsPermissions->push_back(valueAllowResizing);
				
		sfso->PutBoolArray(KEY_PERMISSIONS, sfsPermissions);
	}
				
	// Handle Events
	if (settings->Events() != NULL)
	{
		boost::shared_ptr<vector<boost::shared_ptr<bool> > > sfsEvents (new vector<boost::shared_ptr<bool> >());

		boost::shared_ptr<bool> valueAllowUserEnter (new bool());
		*valueAllowUserEnter = settings->Events()->AllowUserEnter();
		sfsEvents->push_back(valueAllowUserEnter);

		boost::shared_ptr<bool> valueAllowUserExit (new bool());
		*valueAllowUserExit = settings->Events()->AllowUserExit();
		sfsEvents->push_back(valueAllowUserExit);

		boost::shared_ptr<bool> valueAllowUserCountChange (new bool());
		*valueAllowUserCountChange = settings->Events()->AllowUserCountChange();
		sfsEvents->push_back(valueAllowUserCountChange);

		boost::shared_ptr<bool> valueAllowUserVariablesUpdate (new bool());
		*valueAllowUserVariablesUpdate = settings->Events()->AllowUserVariablesUpdate();
		sfsEvents->push_back(valueAllowUserVariablesUpdate);
				
		sfso->PutBoolArray(KEY_EVENTS, sfsEvents);
	}
			
	// Handle Extension data
	if (settings->Extension() != NULL)
	{
		boost::shared_ptr<string> id (new string(*(settings->Extension()->Id())));
		sfso->PutUtfString(KEY_EXTID, id);

		boost::shared_ptr<string> name (new string(*(settings->Extension()->ClassName())));
		sfso->PutUtfString(KEY_EXTCLASS, name);
				
		// Send the properties file only if was specified
		boost::shared_ptr<string> properties (new string(*(settings->Extension()->PropertiesFile())));
		if (settings->Extension()->PropertiesFile() != NULL && !settings->Extension()->PropertiesFile()->empty() && settings->Extension()->PropertiesFile()->size() > 0)
			sfso->PutUtfString(KEY_EXTPROP, properties);
	}
			
	// AutoJoin
	boost::shared_ptr<bool> valueAutoJoin (new bool());
	*valueAutoJoin = autoJoin;
	sfso->PutBool(KEY_AUTOJOIN, valueAutoJoin);
			
	// Room to leave
	if (roomToLeave != NULL)
	{
		boost::shared_ptr<long> valueId (new long());
		*valueId = roomToLeave->Id();
		sfso->PutInt(KEY_ROOM_TO_LEAVE, valueId);
	}
}

}	// namespace Requests
}	// namespace Sfs2X

