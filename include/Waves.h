#ifndef __WAVES_H__
#define __WAVES_H__

//! Includes
#define __NO_SCROLLED__ // Uncomment this define to prevent the embedded editor (ScrollEd) from being compiled
#include "Scroll.h"


//! Defines


//! Waves
class Waves : public Scroll<Waves>
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
};

#endif // __WAVES_H__
