 
![App Screenshot](.doc/png/gaia2.png)
 
 
 
 

 For detailed information on compile modes, see the dedicated documentation:
[Compile Modes](.doc/binary-config/compile_modes.md)






cm-metadata::EntityList usage Example 
```cpp
  Entitylist elist(BPlayer{}, gameinfo{}, BPlayer::Addrs{});
  Client client(BPlayer{}, BPlayer::Addrs{});
  using player = Entity<BPlayer, BPlayer::Addrs>; // callback param deduction necessary
  
``` 

entry point for application as well as config Example :
```cpp
noret cma::config() {
 
   cm.sig.addObject(
      {

         "id.entityGameSystem  xxx????xxxxxxxxx     \\x48\\x8B\\x0D\\x65\\x09\\x44\\x01\\x8B\\xF3\\xC1\\xEB\\x0E\\x81\\xE6\\xFF\\x3F",


         "id.viewMatrix  xxx????xxxx \\x48\\x8D\\x0D\\x16\\x4A\\x70\\x01\\x48\\xC1\\xE0\\x06    ",
         "id.viewAngleBase xxxxx????xxx???? \\x33\\xC9\\x48\\x89\\x05\\xD8\\x57\\x88\\x01\\x48\\x8D\\x05\\x91\\xDF\\x26\\x01 dpos.5",


         "id.localPlayerPawn   xxx????xxx????????xx?   \\x48\\x89\\x0D\\x40\\xDB\\x69\\x01\\x48\\x89\\x0D\\x41\\xDB\\x69\\x01\\x89\\x0D\\x43\\xDB\\x69\\x01\\x48\\x89\\x0D"
      },
      "client.dll"
   );

   cm.sig.addOffset(
      {//   -> eax, [rdi+7Ch]

       "id.viewAngles xxxx?xxxxxxx????  \\xF3\\x0F\\x10\\x50\\x20\\x0F\\x14\\xC1\\xF2\\x0F\\x11\\x87\\x90\\x53\\x00\\x00 dpos.12",
       "id.controller.mhPlayerPawn  xxx???xxxxxx???xxx????xxxxxxxxxxx?xxx??   \\x0F\\x84\\xC3\\x00\\x00\\x00\\x4D\\x85\\xFF\\x0F\\x84\\xBA\\x00\\x00\\x00\\x41\\x8B\\x8F\\xE4\\x07\\x00\\x00\\x83\\xF9\\xFF\\x74\\x7F\\x4C\\x8B\\x0D\\x96\\xC1\\xD7\\x00\\x4D\\x85\\xC9\\x74\\x73   dpos.18"

      },
      "client.dll"
   );

   cme.GetCMUIConfig().Containers.config().menu = false;
}

 cm_app("cs2.exe", "wait.4s con +pause con ");

```

