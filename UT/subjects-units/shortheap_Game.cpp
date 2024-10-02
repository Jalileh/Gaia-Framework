
#include "misc.h"

#include "api/epworker.h"

 


class GameData {
 public:
   char *MapName = "Verdun.map";
   int Player_Amount = 0;
   bool player_godmode = false;
   bool game_ended = false;
   u64 game_Changed5 = 11111111;
   u64 game_Changed4 = 11111111;
   u64 game_Changed3 = 11111111;
   int checksum = 1337;
   int ServerScore = 0;
};
GameData *game = null;


auto init_heapData () {
   if (game) {
      free(game);

      sleep(5s);
   }

   print << "GameData Object Allocated";
   game = new (GameData);

   if(!game->Player_Amount){
         cd("initializing player amount 44", "player count set dynamically");
   }
   game->Player_Amount = 44; 

   print << "init as base ServerCount  " >> game->Player_Amount;
   print << "GodMode is off by default [no NAME_REDACTEDing in mp mode]  " >> game->player_godmode;
   print << "server vote next map " >> game->MapName;
}

auto realloc_heapData () {
   print << "Data re-init -- [ FREEING OBJECT how will sig-fw react? ]";
   init_heapData();
}

auto runHeapGame () {
   static int curplayr = 0;
    clmsg()

   if (game->player_godmode) {
      print << "Player is hacking, client has godmode enabled !!!!!!!!!!!!! boning!!";
   }
   else if (curplayr != game->Player_Amount) {
      curplayr = game->Player_Amount;

      print << "current Server-Player-Count changed to" >> game->Player_Amount;
   }
   else if (game->game_ended or game->Player_Amount <= 1) {
      realloc_heapData();
   }
   else if (game->checksum != 1337) {
      static bool once_ = false;
      if (once_ == false) {
         once_ = !once_;
         print << "checksum changed to " >> game->checksum;
      }
   }
}

auto startup () {
   init_heapData();
   
   fw::frameworkLoader().load_Graphical_Components();
   
  auto cg = cm.gui.getCG();
  cd("LOADED GAIA");
   
   while (true) {
      runHeapGame();
   }
}

ep_winapp("HeapGame", startup, "pause con ")
