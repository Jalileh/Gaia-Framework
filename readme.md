 
![App Screenshot](.doc/png/gaia2.png)
 
 
 
 

 For detailed information on compile modes, see the dedicated documentation:
[Compile Modes](.doc/binary-config/compile_modes.md)
 <h1>About</h1> 
 
       GAIA is a high level framework for experimental 3rd party software development ( hacking ).
       It is responsible for handling several low level .dll modules in order to speed up linear implementation,
       said modules range from kernel, usermode, graphics and machine code scanners and a Reversing tool, most of 
       the underlying source code of what GAIA uses is unavailable ( OBVIOUSLY ), but I'll try and open source more of its dependencies.

       NOTE: I do not condone hacking games and I'm nuking much of my source as I'm stepping away from the game hacking scene, 
	   I never enjoyed doing it, i did it only for learning computer science in a fun way and becoming a better programmer.
       
       This Public version lacks LUA integration and various tools and the development of Siphon reversing tool.
  
       To see the source of a project using this framework see .docs/example-cs2.cpp 
 

<h1>cm-metadata::EntityList usage Example </h1> 

```cpp
  Entitylist elist(BPlayer{}, gameinfo{}, BPlayer::Addrs{});
  Client client(BPlayer{}, BPlayer::Addrs{});
  using player = Entity<BPlayer, BPlayer::Addrs>;  
  
``` 
<h1>
See framework implementation on how to integrate your own low level dll's to work with the framework, supports kernel and usermode.
</h1> 

entry point for application as well as config Example ( using the sig framework ):
```cpp

noret cma::config() {


   cm.sig.addObject(
      {

         "id.entityGameSystem  xxx????xxxxxxxxx     \\x48\\x8B\\x0D\\x65\\x09\\x44\\x01\\x8B\\xF3\\xC1\\xEB\\x0E\\x81\\xE6\\xFF\\x3F",


         "id.viewMatrix  xxx????xxxx \\x48\\x8D\\x0D\\x16\\x4A\\x70\\x01\\x48\\xC1\\xE0\\x06  ",

         "id.viewAngleBase xx????xxxxxxx????x????xxxx????xxxx????xxxx???? \\xFF\\x15\\x00\\x00\\x00\\x00\\x84\\xC0\\x74\\x0C\\x48\\x8D\\x0D\\x00\\x00\\x00\\x00\\xE8\\x00\\x00\\x00\\x00\\x48\\x8B\\xB4\\x24\\x00\\x00\\x00\\x00\\x48\\x8B\\xCE\\xE8\\x00\\x00\\x00\\x00\\x48\\x8B\\xCE\\xE8\\x00\\x00\\x00\\x00 dpos.13",

         "id.localPlayerPawn   xxx????xxx????x????x?   \\x48\\x89\\x0D\\x40\\xDB\\x69\\x01\\x48\\x89\\x0D\\x41\\xDB\\x69\\x01\\x89\\x0D\\x43\\xDB\\x69\\x01\\x48\\x89\\x0D"
      },
      "client.dll"
   );


   cm.sig.addOffset(
      {"id.Pawn.BodyComponentGraph    xxx?xxxxxxxx?xxxxxx \\x48\\x8B\\x49\\x00\\x48\\x85\\xC9\\x74\\x39\\x48\\x8B\\x49\\x00\\x48\\x8B\\x01\\xFF\\x50\\x48 dpos.3",
       "id.BodyComponent.skeletonInstance xxx?xxxxxxxx?xxxxxx \\x48\\x8B\\x49\\x00\\x48\\x85\\xC9\\x74\\x39\\x48\\x8B\\x49\\x00\\x48\\x8B\\x01\\xFF\\x50\\x48 dpos.12",


       "id.SkeletonInstance.modelState xxx????xxxxx?xxxxxxxx???? \\x48\\x8B\\x89\\x00\\x00\\x00\\x00\\x48\\x8B\\x01\\xFF\\x50\\x00\\x48\\x85\\xC0\\x74\\x17\\x48\\x8B\\x80\\x00\\x00\\x00\\x00 pos.3",
       "id.Pawn.Health xxxx????xxxxxxxxxx????xxxxxxxxx?xxxxx?xxxxxxxxxxxxxxxx \\xF3\\x0F\\x10\\x81\\x00\\x00\\x00\\x00\\x0F\\x57\\xD2\\x0F\\x5A\\xF8\\xF3\\x0F\\x10\\x81\\x00\\x00\\x00\\x00\\x48\\x8D\\x4D\\xD0\\xF2\\x0F\\x11\\x7C\\x24\\x00\\xF2\\x0F\\x11\\x74\\x24\\x00\\x0F\\xC6\\xC9\\x55\\xF3\\x0F\\x5A\\xD9\\x0F\\x5A\\xE0\\xF3\\x41\\x0F\\x5A\\xD0 dpos.4",

       "id.Pawn.BodyPosition xxxx????xxxxx????xx????xx???? \\xF2\\x0F\\x11\\x83\\x00\\x00\\x00\\x00\\x8B\\x40\\x08\\x89\\x83\\x00\\x00\\x00\\x00\\x8B\\x83\\x00\\x00\\x00\\x00\\x89\\x83\\x00\\x00\\x00\\x00 dpos.4",

       "id.LocalPlayerPawn_pointer xxx?xxxxxxxx???? \\x45\\x03\\x4F\\x30\\x02\\xDA\\x8B\\x04\\xAF\\x41\\x81\\xC1\\x00\\x00\\x00\\x00 dpos.3",   // this ones very bad
       "id.Pawn.teamid xx?????xx????xxx \\x80\\xBE\\x00\\x00\\x00\\x00\\x00\\x0F\\x84\\x00\\x00\\x00\\x00\\x49\\x8B\\xCE pos.2",
       "id.viewAngles xxxxxx????xxxxxxxxx????xxxx?xxxx?xxxx?xxxxxxxxxxxxxx \\xF2\\x41\\x0F\\x10\\x84\\x30\\x00\\x00\\x00\\x00\\xF2\\x41\\x0F\\x11\\x01\\x41\\x8B\\x84\\x30\\x00\\x00\\x00\\x00\\x48\\x8B\\x5C\\x24\\x00\\x48\\x8B\\x6C\\x24\\x00\\x48\\x8B\\x74\\x24\\x00\\x41\\x89\\x41\\x08\\x48\\x83\\xC4\\x20\\x41\\x5F\\x41\\x5E\\x5F\\xC3  dpos.6",
       "id.controller.mhPlayerPawn  xxx???xxxxxx???xxx????x????xx?????xxx??   \\x0F\\x84\\xC3\\x00\\x00\\x00\\x4D\\x85\\xFF\\x0F\\x84\\xBA\\x00\\x00\\x00\\x41\\x8B\\x8F\\xE4\\x07\\x00\\x00\\x83\\xF9\\xFF\\x74\\x7F\\x4C\\x8B\\x0D\\x96\\xC1\\xD7\\x00\\x4D\\x85\\xC9\\x74\\x73   dpos.18"

      },
      "client.dll"
   );
}

cm_app("cs2.exe", " con2   epfile tkey.9");


```
