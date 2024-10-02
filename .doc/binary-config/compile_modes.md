```cpp
  #define CM_FW_FORCENET true
  // forces net loading despite non-consumerbuild  
  
  #define CM_NOSHADOWLIBS true 
  // only in local loading.
  // no ghost wasteland for our dlls same as regular pjc but override. 
  // - WARNING use with caution OS load exposes us. 
  
  #define ENABLE_CM_VERBOSE true 
  // informative logging, do not use with errors please.
  // stripped from compiled binary on consumerbuilds.
  

  #define CM_VERBOSE_SIG true 
  // provides detail on all attempted signatures

  #define CM_NO_ROUTE   
  // disables gaia fn::definitions and relies on- 
  // CMCBlist being fed inside config at rt;
  
  #define CM_ROUTE_SPEED 
  // measure the execution speed of ROUTE
  
  
```


