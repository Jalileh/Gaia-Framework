 
 
#include "cm/cm.h"

 

template < typename T, typename member >
auto confirm_rat_write (auto dst, T data, member T::*confirm_dataWritten, auto markid) {
   cm.rat.write(dst, data, markid);

   T dataConfirm = {0};
   cm.rat.steal(dst, dataConfirm);

   if (data.*confirm_dataWritten == dataConfirm.*confirm_dataWritten) {
      print << markid >> "Was Succesfully modified by new value" >> dataConfirm.*confirm_dataWritten;
   }
}



class sdkGameData {
 public:
   pchar MapName;
   int Player_Amount;
   bool player_godmode;
   bool game_ended;
   int checksum;
   int ServerScore;
};
sdkGameData gamesdk = {0};
Entitylist test(sdkGameData{});

class _gaiaTest {
   astr subject;

   auto GetTestSubjectName () {
      auto str = dir::GetExeDirectoryOnDisk();
      files2 _gaiaFile(str.append("settings_UT__gaia.txt"));
      astr buffer = _gaiaFile.getContent();
      astr testSubjectName = buffer.ExtractWord("subject_exe.\a");
      if (testSubjectName.NotNull()) {
         return testSubjectName;
      }
      else {
         _gaiaFile.setContent(" // supply testSubjectdotToken executable below : \n\n\n subject_exe    [.] <-- ");
         throw str;
      }

      return testSubjectName;
   }

 public:
   auto tryHost ();
   auto sigLangConf ();

 public:
   noret unit_sig ();

   noret unit_rat ();
};

_gaiaTest _gaia;

noret fnXXXv(sdkGameData &  player,auto  EBaseAddr){
 EBaseAddr = 0;
 player.checksum = 3;

}

auto _gaiaTest::tryHost() {

  astr subject = "_gaia-subject.exe";
 
   test.route(fnXXXv);
   
   cm.rat.SetProcessModule(subject);
}

auto units () {
#ifdef UT_RAT
   _gaia.unit_rat();
#endif
#ifdef UT_SIG

   _gaia.unit_sig();
#endif
}
auto _gaiaTest_T2_RUNTIME () {

   measureExecutionTime(units);
   print << (astr("_gaia units Complete") + astr(" Pause initiated ").append(UT_PAUSE));

   return;
};

auto _gaiaTest_T2_init () {
   _gaia.tryHost();
   return;
};


// use confirm_rat_write for reads please
noret _gaiaTest::unit_rat() {
}

noret _gaiaTest::unit_sig() {

   auto guimaster = rat.GetModuleByName("guimaster.dll");
   auto objectaddr = sig.findObject("gamedata.pos");
   auto offset_checksum =  sig.findOffset<u16>("checksum");

   uint checksumvalue;
   
   

   cm.rat.stealv(
      objectaddr, gamesdk, "gamesdk",
      objectaddr + offset_checksum, checksumvalue, "offset_checksum"

   );

   print << astr("current player count = ") >> (gamesdk.Player_Amount);
   print << astr("current score  = ") >> gamesdk.ServerScore;
   print << astr("rat-alterated offset_checksum = ") >> (int)checksumvalue;

   int new_checksum = 666;
   cm.rat.write(objectaddr + offset_checksum, new_checksum, "rewrite");

   print << "";
}

auto _gaiaTest::sigLangConf() {


   sig.addObject(
      {


         //  mov     rax, cs:game        // mov rax appears to be x48/x8b/x0D
         
          "id.gamedata.pos xxx????x?????x???? pos.3 \\x48\\x8B\\x0D\\xBC\\x52\\x03\\x00\\xFF\\x15\\xAE\\xBF\\x02\\x00\\x48\\x8D"

      
      }
   );

   //  caching siglang offsets this will revolutionize our NAME_REDACTEDs.
   sig.addOffset(
      {// checksum           cmp     dword ptr [rax+10h], 539h
       "id.checksum  ?xx?xx??   \\x00\\x81\\x78\\x10\\x39\\x05\\x00\\x00    " 
     


      }
   );

  cd("WTF");
   return;
}


#include "../cm/include/fnTemps.h"

noret cma::config() {
 
   _gaia.sigLangConf();


   cme.fnroutes.NAME_REDACTEDT2_init.Link(_gaiaTest_T2_init);
   cme.fnroutes.NAME_REDACTEDT2_inf.Link(_gaiaTest_T2_RUNTIME);


   return;
}


cm_app("none", "pause con");
