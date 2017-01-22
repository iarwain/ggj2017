//! Includes
#define __SCROLL_IMPL__
#include "Waves.h"
#undef __SCROLL_IMPL__

#include "orxArchive.c"


//! Defines


//! Constants


//! Structs/Classes

//! Source
class Source : public ScrollObject
{
public:

  void OnCreate()
  {
    bIsActive   = orxFALSE;
    bIsSelected = orxFALSE;
    u32Count++;
  }

  void OnDelete()
  {
    u32Count--;
  }

  orxBOOL OnRender(orxRENDER_EVENT_PAYLOAD *_pstPayload)
  {
    return orxTRUE;
  }

  void Update(const orxCLOCK_INFO &_rstInfo)
  {
    ScrollObject *poSource;

    Waves &roGame = Waves::GetInstance();

    // Gets object
    poSource = roGame.GetRunTimeObject("Source");

    // Selected?
    if(poSource == this)
    {
      // Picked?
      if(bIsSelected == orxFALSE)
      {
        // Updates status
        bIsActive   = orxFALSE;
        bIsSelected = orxTRUE;

        // Pushes config section
        PushConfigSection();

        // Can be moved?
        if(orxConfig_GetBool("Movable") != orxFALSE)
        {
          ScrollObject *poCursor;

          // Gets Cursor
          poCursor = roGame.GetRunTimeObject("Cursor");

          // Valid?
          if(poCursor != orxNULL)
          {
            // Attaches source to it
            orxObject_Attach(GetOrxObject(), poCursor->GetOrxObject());
          }
        }

        // Pops config section
        PopConfigSection();
      }
    }
    else
    {
      // Dropped?
      if(bIsSelected != orxFALSE)
      {
        // Updates status
        bIsActive   = orxTRUE;
        bIsSelected = orxFALSE;

        // Detaches source from cursor
        orxObject_Attach(GetOrxObject(), orxNULL);
      }
    }
  }

  orxBOOL bIsActive;
  orxBOOL bIsSelected;
  static orxU32 u32Count;
};

orxU32 Source::u32Count;


//! Code
static orxBOOL orxFASTCALL SaveCallback(const orxSTRING _zSectionName, const orxSTRING _zKeyName, const orxSTRING _zFileName, orxBOOL _bUseEncryption)
{
  //! Done!
  return (orxString_Compare(_zSectionName, "Save") == 0) ? orxTRUE : orxFALSE;
}

ScrollObject *Waves::GetInteractionObject() const
{
  ScrollObject *poResult;

  // Updates result
  poResult = GetObject(mu64InteractionID);

  // Done!
  return poResult;
}

orxSTATUS Waves::Load()
{
  const orxSTRING zSavePath;
  orxSTATUS       eResult = orxSTATUS_FAILURE;

  // Pushes game config section
  orxConfig_PushSection("Game");

  // Gets save path
  zSavePath = orxFile_GetApplicationSaveDirectory(orxConfig_GetString("SaveFile"));

  // Pops config section
  orxConfig_PopSection();

  // Success?
  if(zSavePath != orxSTRING_EMPTY)
  {
    // Saves to disk
    eResult = orxConfig_Load(zSavePath);
  }

  // Done!
  return eResult;
}

orxSTATUS Waves::Save()
{
  const orxSTRING zSavePath;
  orxSTATUS       eResult = orxSTATUS_FAILURE;

  // Pushes game config section
  orxConfig_PushSection("Game");

  // Gets save path
  zSavePath = orxFile_GetApplicationSaveDirectory(orxConfig_GetString("SaveFile"));

  // Pops config section
  orxConfig_PopSection();

  // Success?
  if(zSavePath != orxSTRING_EMPTY)
  {
    // Saves to disk
    eResult = orxConfig_Save(zSavePath, orxFALSE, SaveCallback);
  }

  // Done!
  return eResult;
}

void Waves::DeleteRunTimeObject(const orxSTRING _zObjectName)
{
  orxU64        u64ObjectGUID;
  ScrollObject *poObject;

  // Pushes runtime section
  orxConfig_PushSection("RunTime");

  // Gets object GUID
  u64ObjectGUID = orxConfig_GetU64(_zObjectName);

  // Gets object
  poObject = GetObject(u64ObjectGUID);

  // Valid?
  if(poObject != orxNULL)
  {
    // Deletes it
    DeleteObject(poObject);

    // Clears GUID
    orxConfig_SetU64(_zObjectName, orxU64_UNDEFINED);
  }

  // Pops config section
  orxConfig_PopSection();
}

ScrollObject *Waves::GetRunTimeObject(const orxSTRING _zObjectName) const
{
  ScrollObject *poResult;

  // Pushes runtime section
  orxConfig_PushSection("RunTime");

  // Gets object
  poResult = GetObject(orxConfig_GetU64(_zObjectName));

  // Pops config section
  orxConfig_PopSection();

  // Done
  return poResult;
}

orxSTATUS Waves::Bootstrap() const
{
  orxSTATUS eResult = orxSTATUS_SUCCESS;

  // Inits custom Zip archive handler
  orxArchive_Init();

  // Adds default release config paths
  orxResource_AddStorage(orxCONFIG_KZ_RESOURCE_GROUP, "../data.wvs", orxFALSE);
  orxResource_AddStorage(orxCONFIG_KZ_RESOURCE_GROUP, "data.wvs", orxFALSE);
  orxResource_AddStorage(orxCONFIG_KZ_RESOURCE_GROUP, "../../data/config", orxFALSE);
  orxResource_AddStorage(orxCONFIG_KZ_RESOURCE_GROUP, "../data/config", orxFALSE);

  // Done!
  return eResult;
}

void Waves::Reset()
{
}

void Waves::LoadMenu()
{
  // Updates state
  meGameState = GameStateMenu;

  // Creates main menu
  CreateObject("O-MainMenu");
}

void Waves::InitSplash()
{
  orxVECTOR vPosition;

  // Updates listener position
  orxCamera_GetPosition(GetMainCamera(), &vPosition);
  orxSoundSystem_SetListenerPosition(&vPosition);

  // Creates splash screen
  if(CreateObject("O-Splash") == orxNULL)
  {
    // Loads menu
    LoadMenu();
  }
}

void Waves::UpdateInteraction(const orxCLOCK_INFO &_rstInfo)
{
  orxVECTOR     vMousePos, vPickPos;
  orxU64        u64PickedID = 0;
  ScrollObject *poPickedObject = orxNULL, *poInteractionObject;

  // Gets world mouse position
  if(orxRender_GetWorldPosition(orxMouse_GetPosition(&vMousePos), orxNULL, &vMousePos) != orxNULL)
  {
    // Stores it
    orxVector_Set(&mvMousePosition, vMousePos.fX, vMousePos.fY, orxFLOAT_0);
  }

  // Gets picking position
  orxVector_Set(&vPickPos, mvMousePosition.fX, mvMousePosition.fY, -orxFLOAT_1);

  // For all pickable groups
  for(orxU32 i = 0, u32Number = orxConfig_GetListCounter("PickGroupList"); i < u32Number; i++)
  {
    // Picks object in it
    poPickedObject = PickObject(vPickPos, orxString_GetID(orxConfig_GetListString("PickGroupList", i)));

    // Found?
    if(poPickedObject)
    {
      // Updates picked ID
      u64PickedID = poPickedObject->GetGUID();

      // Stops
      break;
    }
  }

  // Gets current interaction object
  poInteractionObject = (mu64InteractionID != 0) ? GetObject(mu64InteractionID) : orxNULL;

  // New picking?
  if(u64PickedID != mu64InteractionID)
  {
    // Has current interaction object?
    if(poInteractionObject)
    {
      // Adds contextual track
      poInteractionObject->AddConditionalTrack("OnLeaveTrack");
    }

    // Picked new object?
    if(poPickedObject)
    {
      // Adds conditional track
      poPickedObject->AddConditionalTrack(orxInput_IsActive("Action") ? "OnClickTrack" : "OnEnterTrack");
    }

    // Stores new interaction ID
    mu64InteractionID = u64PickedID;
  }
  else
  {
    // Has interaction object?
    if(poInteractionObject)
    {
      // Change of action?
      if(orxInput_HasNewStatus("Action"))
      {
        // Adds conditional track
        poInteractionObject->AddConditionalTrack(orxInput_IsActive("Action") ? "OnClickTrack" : "OnReleaseTrack");
      }
    }
  }
}

void Waves::UpdateShader(const orxCLOCK_INFO &_rstInfo)
{
  // For all shaders
  for(orxSHADER *pstShader = orxSHADER(orxStructure_GetFirst(orxSTRUCTURE_ID_SHADER));
      pstShader != orxNULL;
      pstShader = orxSHADER(orxStructure_GetNext(pstShader)))
  {
    orxVECTOR vRes = {};

    // Updates its time
    orxShader_SetFloatParam(pstShader, "Time", 0, &_rstInfo.fTime);

    // Updates its Res
    orxDisplay_GetScreenSize(&vRes.fX, &vRes.fY);
    orxShader_SetVectorParam(pstShader, "Res", 0, &vRes);

    // Level?
    if(!orxString_Compare(orxShader_GetName(pstShader), "S-Level"))
    {
      orxVECTOR avSourceList[4] = {};
      orxVECTOR avSourceColorList[4] = {};
      orxFLOAT  afSourceRadiusList[4] = {};

      for(Source *poSource = GetNextObject<Source>();
          poSource;
          poSource = GetNextObject<Source>(poSource))
      {
        orxU32 u32ID;

        // Pushes its section
        poSource->PushConfigSection();

        // Gets its ID
        u32ID = orxConfig_GetU32("ID");

        // Gets its position
        poSource->GetPosition(avSourceList[u32ID]);
        avSourceList[u32ID].fX /= 1920.f;
        avSourceList[u32ID].fY /= 1080.f;

        // Gets its tint
        orxConfig_GetVector("Tint", &avSourceColorList[u32ID]);

        // Gets its radius
        afSourceRadiusList[u32ID] = orxConfig_GetFloat("Radius");

        // Pops config section
        poSource->PopConfigSection();
      }

      // Updates shader parameters
      orxShader_SetVectorParam(pstShader, "SourceList", 4, avSourceList);
      orxShader_SetVectorParam(pstShader, "SourceColorList", 4, avSourceColorList);
      orxShader_SetFloatParam(pstShader, "SourceRadiusList", 4, afSourceRadiusList);

      // for(orxU32 i = 0; i < 4; i++)
      // {
      //   orxLOG("Source%d: At (%f, %f) Col (%f, %f, %f) Rad %f", i, avSourceList[i].fX, avSourceList[i].fY, avSourceColorList[i].fR, avSourceColorList[i].fG, avSourceColorList[i].fB, afSourceRadiusList[i]);
      // }
    }
  }
}

void Waves::UpdateGame(const orxCLOCK_INFO &_rstInfo)
{
  // Updates in game time
  mdTime += orx2D(_rstInfo.fDT);

  // Updates input
  UpdateInput(_rstInfo);
}

void Waves::UpdatePause(const orxCLOCK_INFO &_rstInfo)
{
  // Pausing?
  if(orxInput_IsActive("Pause"))
  {
    // Pause game
    PauseGame(!IsGamePaused());
  }
}

void Waves::UpdateInput(const orxCLOCK_INFO &_rstInfo)
{
  // Updates pause
  UpdatePause(_rstInfo);
}

void Waves::Update(const orxCLOCK_INFO &_rstInfo)
{
  // Updates interaction
  UpdateInteraction(_rstInfo);

  // Updates shader
  UpdateShader(_rstInfo);

  // Depending on game state
  switch(meGameState)
  {
    case GameStateSplash:
    {
      // Should load menu?
      if(orxInput_IsActive("LoadMenu"))
      {
        // Loads menu
        LoadMenu();
      }

      break;
    }

    case GameStateMenu:
    {
      // Shoud start?
      if(orxInput_IsActive("Start"))
      {
        // Updates state
        meGameState = GameStateRun;
      }

      // Leaving state?
      if(meGameState != GameStateMenu)
      {
        // Deletes main menu
        DeleteRunTimeObject("MainMenu");
      }

      break;
    }

    case GameStateRun:
    {
      // Updates in-game
      UpdateGame(_rstInfo);

      break;
    }

    case GameStatePause:
    {
      // Updates pause
      UpdatePause(_rstInfo);

      break;
    }

    case GameStateEnd:
    {
      break;
    }
  }
}

orxSTATUS Waves::Init()
{
  orxSTATUS eResult = orxSTATUS_SUCCESS;

  // Init values
  meGameState       = GameStateSplash;
  mu64InteractionID = 0;
  mdTime            = orx2D(0.0);
  Source::u32Count  = 0;
  orxVector_Copy(&mvMousePosition, &orxVECTOR_0);

  // Loads config
  Load();

  // Pushes main section
  orxConfig_PushSection("Game");

  // For all viewports
  for(orxU32 i = 0, iCount = orxConfig_GetListCounter("ViewportList");
      i < iCount;
      i++)
  {
    // Creates it
    orxViewport_CreateFromConfig(orxConfig_GetListString("ViewportList", i));
  }

  // Registers event handler
  orxEvent_AddHandler(orxEVENT_TYPE_SHADER, &EventHandler);
  orxEvent_AddHandler(orxEVENT_TYPE_RESOURCE, &EventHandler);

  // Inits splash screen
  InitSplash();

  // Done!
  return eResult;
}

orxSTATUS Waves::Run()
{
  orxSTATUS eResult = orxSTATUS_SUCCESS;

  // Quitting?
  if(orxInput_IsActive("Quit"))
  {
    // Updates result
    eResult = orxSTATUS_FAILURE;
  }

  // Done!
  return eResult;
}

void Waves::Exit()
{
  // Saves data
  Save();

  // Unregisters event handler
  orxEvent_RemoveHandler(orxEVENT_TYPE_SHADER, &EventHandler);
  orxEvent_RemoveHandler(orxEVENT_TYPE_RESOURCE, &EventHandler);
}

void Waves::BindObjects()
{
  // Binds objects
  ScrollBindObject<Source>("O-Source");
}

orxSTATUS orxFASTCALL Waves::EventHandler(const orxEVENT *_pstEvent)
{
  orxSTATUS eResult = orxSTATUS_SUCCESS;

  // Depending on type
  switch(_pstEvent->eType)
  {
    case orxEVENT_TYPE_SHADER:
    {
      // Param?
      if(_pstEvent->eID == orxSHADER_EVENT_SET_PARAM)
      {
        orxSHADER_EVENT_PAYLOAD *pstPayload;

        // Gets payload
        pstPayload = (orxSHADER_EVENT_PAYLOAD *)_pstEvent->pstPayload;
      }

      break;
    }

    case orxEVENT_TYPE_RESOURCE:
    {
      /* Add or update? */
      if((_pstEvent->eID == orxRESOURCE_EVENT_ADD) || (_pstEvent->eID == orxRESOURCE_EVENT_UPDATE))
      {
      }

      break;
    }

    default:
    {
      break;
    }
  }

  // Done!
  return eResult;
}

int main(int argc, char **argv)
{
  // Executes game
  Waves::GetInstance().Execute(argc, argv);

  // Done!
  return EXIT_SUCCESS;
}

#ifdef __orxWINDOWS__
#include "windows.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
  // Executes game
  Waves::GetInstance().Execute();

  // Done!
  return EXIT_SUCCESS;
}
#endif // __orxWINDOWS__
