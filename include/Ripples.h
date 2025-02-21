#ifndef __RIPPLES_H__
#define __RIPPLES_H__

//! Includes
#include "Scroll/Scroll.h"


//! Defines


//! Ripples
class Ripples : public Scroll<Ripples>
{
public:

  enum GameState
  {
    GameStateSplash = 0,
    GameStateMenu,
    GameStateRun,
    GameStatePause,
    GameStateGameOver,
    GameStateEnd,
    GameStateReset,

    GameStateNumber,

    GameStateNone = orxENUM_NONE
  };


                orxDOUBLE       GetTime() const             {return mdTime;}
                GameState       GetGameState() const        {return meGameState;}
                ScrollObject *  GetInteractionObject() const;
                const orxVECTOR &GetMousePosition() const   {return mvMousePosition;}

                orxSTATUS       Load();
                orxSTATUS       Save();

                void            DeleteRunTimeObject(const orxSTRING _zObjectName);
                ScrollObject *  GetRunTimeObject(const orxSTRING _zObjectName) const;

                void            Validate();


private:

                orxSTATUS       Bootstrap() const;

                void            Reset();
                void            LoadMenu();

                void            InitSplash();

                void            UpdateInteraction(const orxCLOCK_INFO &_rstInfo);
                void            UpdateShader(const orxCLOCK_INFO &_rstInfo);

                void            UpdateGame(const orxCLOCK_INFO &_rstInfo);
                void            UpdatePause(const orxCLOCK_INFO &_rstInfo);
                void            UpdateInput(const orxCLOCK_INFO &_rstInfo);

                void            Update(const orxCLOCK_INFO &_rstInfo);

                orxSTATUS       Init();
                orxSTATUS       Run();
                void            Exit();
                void            BindObjects();

  static        orxSTATUS orxFASTCALL EventHandler(const orxEVENT *_pstEvent);


private:

                orxVECTOR       mvMousePosition;
                orxU64          mu64InteractionID;
                orxDOUBLE       mdTime;
                GameState       meGameState;
                orxU8           mau8ValidateBuffer[1920 * 1080 * sizeof(orxRGBA)];
};

#endif // __RIPPLES_H__
