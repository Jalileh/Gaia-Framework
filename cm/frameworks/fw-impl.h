#pragma once

#define CM_FRAMEWORKS_H

#include "../_cmarch.h"
#include "../cm-metadata.h"

#include "../include/fnTemps.h"
 

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
////
////  @s.faster than light um-ke compability
////
////  .. keep this shit as simple as possible please, it's a gateway
////  - exists only for cross-compability with our r/w methods.
////    should not be used by consumer.
////


//  usermode - kernel cross-compability
//  sig has this too btw, but its ugly
class FasterThanLight;


#ifdef CM_KERNEL

#   include "comlib.h"
using communicator = pEproxv2;

#elif defined(CM_USERMODE)
#   include "umlib.h"
using communicator = pumlib;   // if you get this error define CM_KERNEL
#endif


class FasterThanLight {
 public:
   singleton_member communicator proxima;

 public:
#if defined(CM_KERNEL)

   // bug discovery in driver knownProcess we impplement it here instead
   singleton_member auto
   is_KnownProcess (astr &Name, bool kernel_result_discarded) -> bool {


      if (false == kernel_result_discarded)
         return false;

      // kernel thinks it is knownProcess so confirm


      static int clock_ = 0;

      if (clock_ < 0) {

         clock_++;
         return true;
      }
      else {
         // check name from Eprocess
         auto &eprox = proxima->old_eprox_v1;

         char file_name[15];

         u64 Eprocess = *(u64 *)eprox->GetCurrentEProcess();
         bool res;
         if (proxima->readVir(Eprocess, 0x5a8, 15, file_name)) {
            clock_ = -999;
            //   process is alive result here
            u32 active_threads;
            proxima->readVir(Eprocess, 0x5f0, 4, &active_threads);

            return active_threads;
            ///////////////////////////////////////
         }
      }
      return false;
   };
   fnimpl_fnObject setHandle = [] (astr Name) -> bool {
      crypt Namec(Name.cstr());
      bool result = proxima->setHandle(Namec);

      // CRITICAL BUG DISCOVERY ON FAILURE WITH NAMES LIKE [REDACTED] UM RESTART NEEDS TO BE SET TO FALSE
      // - OTHERWISE IT WILL RETURN TRUE ON NEXT RUN
      {

         if (false == is_KnownProcess(Name, result))
            proxima->data->um_restart = true;
      }

      return result;
   };
   fnimpl_fnObject GetModuleBase = [] () {
      return proxima->data->ModuleBase;
   };

   fnimpl_fnObject GetRawBuffer = [] () {
      return proxima->buffer;
   };

   fnimpl_fnObject findSubModules = [] (astr Name) {
      return proxima->tools.currentProcess.findSubModule(Name);
   };


   fnimpl_fnObject readMemory = [] (u64 Destination, size_t length, pvoid buffer) {
      return proxima->readVir(Destination, 0, length, buffer);
   };

   fnimpl_fnObject writeMemory = [] (u64 Destination, size_t length, pvoid buffer) {
      return proxima->writeVir(Destination, 0, length, buffer);
   };

#elif defined(CM_USERMODE)

   fnimpl_fnObject setHandle = [] (astr Name) -> bool {
      bool result = (!proxima->curHandle) ? (bool)proxima->openHandle(Name.pwide_str()) : true;
      return result;
   };
   fnimpl_fnObject GetModuleBase = [] () {
      return proxima->modulebase;
   };


   fnimpl_fnObject findSubModules = [] (astr Name) -> u64 {
      u64 ret_submodule = 0;
      auto modules_size = proxima->data.numofsubModules;

      for (int i = 0; i < modules_size; i++) {
         auto info = proxima->data.modules[i];

         auto modname = astr(crypt(info.szModule).decrypt());


         if (modname == Name) {
            ret_submodule = (u64)info.modBaseAddr;
            break;
         }
      }


      return ret_submodule;
   };


   singleton_member pvoid um_autoBuffer = 0;

   fnimpl_fnObject GetRawBuffer = [] () {
      return um_autoBuffer;
   };

   fnimpl_fnObject readMemory = [] (u64 Destination, size_t length, pvoid buffer) -> bool {
      if (buffer == null) {

         if (um_autoBuffer)
            disposeMemory(um_autoBuffer);

         um_autoBuffer = allocnull< pchar >(length);

         proxima->read(pvoid(Destination), length, um_autoBuffer);
      }
      else {

         proxima->read(pvoid(Destination), length, buffer);
      }

      return true;
   };

   fnimpl_fnObject writeMemory = [] (u64 Destination, size_t length, pvoid buffer) -> bool {
      proxima->write(pvoid(Destination), length, buffer);
      return true;
   };

#endif
};

singleton FasterThanLight fsl;

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
////
////  @s.rat
////
////
////
enum class enumRatError {
   Great,
   Process_is_not_running,
   submodule_not_found,
   ratcache_error,


};
#include "api/templates.h"
class RatExceptions : public exceptions_templ::Base< enumRatError > {
 public:
   using exceptions_templ::Base< enumRatError >::Base;
   RatExceptions(enumRatError e, astr estring) : Base(e, estring) {}
   RatExceptions(enumRatError e) : Base(e) {}
};

class cma::systems::Rat {
 protected:
   friend fw::frameworkLoader;
   singleton_member communicator comsend;

 public:
   auto zlsendGet () -> auto {
   }

 private:
   struct SubModuleInfo {
      u64 ModuleBase;
      cryptInput ModuleName;
   };

   singleton_member CachedData< SubModuleInfo > NAME_REDACTED_info;

 public:
   u64 SetProcessModule (astr Name) {

      if (fsl.setHandle.fn(Name)) {

         return fsl.GetModuleBase.fn();
      }
      else {
         throw RatExceptions(enumRatError::Process_is_not_running, astrc("GAIA ERROR: Unable to locate the Game, are you sure it is running?"));
      }
      return u64(null);
   }
   auto GetMainModule () {
      return fsl.GetModuleBase.fn();
   }

   u64 GetModuleByName (astr Name) {

      SubModuleInfo aptr_ CachedSubModule = NAME_REDACTED_info.getbyID(Name, &SubModuleInfo::ModuleName);

      if (CachedSubModule == nullptr) {

         // performance wise, we only run occasionally findSubModule, we also read M to see if module good.

         u64 requested_module = (u64)fsl.findSubModules.fn(Name);

         if (requested_module > null) {

            NAME_REDACTED_info.AddItem({requested_module, Name.c_str()});

            return requested_module;
         }
         else {
            throw RatExceptions(enumRatError::submodule_not_found, "unable to find submodule");
         }
      }
      else {

         // confirm the fucking module hasnt been unloaded before returning it
         u64 submodule_value = CachedSubModule->ModuleBase;

         char PESymbol = 'M',
              PECheck = '\0';

         fsl.readMemory.fn(submodule_value, sizeof(char), &PECheck);

         if (PESymbol == PECheck) {
            return submodule_value;
         }
         else {

            CachedSubModule->ModuleBase = (u64)fsl.findSubModules.fn(Name);

            cmverbose(" mitigated Error: CachedSubModule maybe got unloaded, we pulled it again for caching ", "cm-rat-fw");

            return CachedSubModule->ModuleBase;
         }
      }


      return null;
   }

 public:
   // throws exception if provided with 0
   template < typename ret >
   ret read (u64 destination, u64 size, cmMarkID id) {
      reAlert.guard({destination, size, id.decrypt()});
      // read occurs down here
      fsl.readMemory.fn(destination, size, 0);

      // return (misc::type_is<ret*, ret>::result) ?
      //(ret)fsl.GetRawBuffer.fn()
      //: *(ret *)fsl.GetRawBuffer.fn();


      return *(ret *)fsl.GetRawBuffer.fn();
   }

   template < typename t_pointer >
   t_pointer _CReadPointerRaw (u64 destination, u64 size, cmMarkID id) {
      reAlert.guard({destination, size, id.decrypt()});

      fsl.readMemory.fn(destination, size, 0);

      return (t_pointer)(fsl.GetRawBuffer.fn());
   }

   // cleaner code to navigate data structures and pull address values off pointers
 
   template < typename ret >
   ret readPointer (const u64 &Pointer, int math, cmMarkID id) {
      auto to_read = Pointer + math;
      return read< ret >(to_read, sizeof(ret), id);
   }
   
 
 
   template < typename T, typename... Args >
   bool readPointers (T &OUT Store_result_info, u64 ref_Pointer, int On_Pointer_math, cmMarkID id, Args &&...args) {

      try {
         Store_result_info = readPointer< T >(ref_Pointer, On_Pointer_math, id);
      } catch (astr &e) {
         // failure here
         return false;
      }

      if constexpr (sizeof...(args) > 0)
         return readPointers(args...);

      return true;
   }

   template < typename ret >
   ret readBytes (const auto Address, u64 numBytesToRead, cmMarkID id) {
      return read< ret >(Address, numBytesToRead);
   }


   // throws exception if provided with destination num 0
   //
   //  High Level approach:
   //   - : dataStruct receives the data from destination.
   //   - : size is auto because we read by the dataStruct size.
   template < typename T >
   noret steal (const u64 destination, T &dataStruct, cmMarkID id = "untracked") {
      dataStruct = read< T >(destination, sizeof(dataStruct), id);
   }

   // throws exception if provided with destination num 0
   //
   //  High Level approach args... Version:
   //   - : dataStruct receives the data from destination.
   //   - : size is auto because we read by the dataStruct size.
   //   -- : Recursive case for stealv
   template < typename T, typename... Args >
   noret stealv (const u64 destination, T &dataStruct, cmMarkID id, Args &&...args) {
      steal(destination, dataStruct, id);         // Process the current argument
      if constexpr (sizeof...(args) > 0) {        // Check if there are more arguments
         stealv(std::forward< Args >(args)...);   // Recursively call stealv with the remaining arguments
      }
   }


   // throws exception if provided result fails.
   template < typename data >
   bool write (const u64 destination, data value, cmMarkID id) {
      auto result = fsl.writeMemory.fn(destination, sizeof(value), ref_ value);
      if (!result)
         reAlert.guard({0, 0, id.decrypt()});

      return result;
   }


 private:
   class REAlert {
    public:
      class ratCache;
      noret guard (ratCache readFeed) {
         if (!readFeed.destination or !(readFeed.destination >> (8 * 4))) {
            astr RE_Error;
            if (!lastCache.destination)
               RE_Error = acstr(
                  "failure mark caught insanely early, something is very wrong \nmark: "
               );
            else
               RE_Error = genfailMark(readFeed.destination, lastCache.destination, lastCache.id);


            throw RE_Error;
         }
         else {
            lastCache = readFeed;
            return;
         }
      }

    private:
      astr genfailMark (astr failedMark, u64 GoodAddrMark, astr GoodMarkId) {
         astr alertform;

         astr markTeller = acstr("re-failure by markid -> "),
              markTeller2 = acstr("\n HINT [?] last good Mark was ... "),
              markTeller3 = astrc(" and Address was ... ");

    
         return alertform.append_v(markTeller.append(failedMark),
         markTeller2.append(GoodMarkId), 
         markTeller3.append(GoodAddrMark));
      }

    public:
      class ratCache {
       public:
         u64 destination = unset, size;
         astr id;
      };

    private:
      ratCache lastCache;
   };
   REAlert reAlert;
};

#include "gmmath.h"
template < typename type >
class ratvar : public gmvar< type >, public cma::systems::Rat {

 public:
   using gmvar< type >::gmvar;
   ratvar(type ReVar) : gmvar< type >(ReVar) {};
   ratvar(ratvar &other) {
      *this = other;
   }

   u64 Addr;

   noret read (const u64 destination, u64 size, cmMarkID id) {
      gmvar< type >::gmvar::value =
         cma::systems::Rat::read< type >(destination, size, id);

      Addr = destination;
      return;
   }

   // throws exception if provided result fails.
   template < typename data >
   noret write (const u64 destination, data value, cmMarkID id) {
      cma::systems::Rat::write(destination, value, id);
      return;
   }
};

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
////
////  @s.sigframework
////
////
////
////


#include "sigmaster.h"
using sigLangSTR = astr;

class sigLang_Object {
 public:
   sigLang_Object() : signature(nullsets) {}

 public:
   sigLangSTR id;

   struct Source_Pattern {
      sigLangSTR interpreted_string;
      pchar pattern;
      uint length;
      int DynamicPosition;
      bool HasCustomDynamicPos;
   };

   Source_Pattern signature;

   sigLangSTR masking;
   sigLangSTR module;
   sigLangSTR section;

   enum Type { offset,
               object,
               none };

   Type type;

#if CM_VERBOSE_SIG == true
   class VerboseInfo {
    public:
      VerboseInfo() : info_wasLogged(false) {}
      astr string;
      bool info_wasLogged;

      inline noret OnDynamicLog (auto dynamicResolve, Type typeSig) {
         if (info_wasLogged == true)
            return;
         else {
            astr type;
            if (typeSig == Type::object)
               type = "\nResolved Dynamic object : ";
            else
               type = "\nResolved Dynamic Offset: ";


            string.append(type.prepend("\n")).append(astr(dynamicResolve).hexStr()).print("[CM-SIGFW : VERBOSE]");
            info_wasLogged = true;
         }
      };
   };
   VerboseInfo m_VerboseInfo;

#endif

   class PerformedData {
    public:
      PerformedData()
         : instruction_address(null), dynamic_value(null) {}

      auto SetInstructionAddress (auto address) {
         instruction_address = address;
      }
      // could be either game data ptrs or offsets
      auto SetDynamicValue (auto value) {
         dynamic_value = value;
      }

      auto GetInstructionAddress () {
         return instruction_address;
      }
      // could be either game data ptrs or offsets
      auto GetDynamicValue () {
         return dynamic_value;
      }

    private:
      u64 instruction_address;
      u64 dynamic_value;
   };

   PerformedData results;
};

using SigMasterlib = psigmScanner;

class sigLang {
 private:
   bool _manualFind_DynamicPos (sigLang_Object &object, astr &plain_text) {

      astr token;
      astr dynamicPos[2] = {"\bpos.", "dpos."};

      if (plain_text.match({dynamicPos[0], dynamicPos[1]}, &token)) {

         astr str_dptext = plain_text.ExtractWord(token);

         int DYNAMICPOS = str_dptext.ExtractWord(token + '\a');

         if (!DYNAMICPOS and str_dptext.pos(-2) != '.') {
            cle_private("MANUAL POS.[?] NO VALUE", "[CM] pos. with no value sig-siglang");
         }
         else {
            plain_text = plain_text.replace(token + DYNAMICPOS, "");
            object.signature.DynamicPosition = (DYNAMICPOS);
            object.signature.HasCustomDynamicPos = true;
         }

         return true;
      }
      else {
         return false;
      }
   }

 public:
   noret handleManualConfig (sigLang_Object &object, astr &plain_text) {
      if (!_manualFind_DynamicPos(object, plain_text)) {
         object.signature.DynamicPosition = 3;
      }
   }


   noret CompensateSignature (pchar &Signature, ulong m_signature_size, astr &m_Mask, astr log_id) {

      auto sig_size = m_signature_size;
      auto mask_size = (m_Mask.size());

      long difference = sig_size - mask_size;
      if (difference == null) {
         cmverbose("maximized-good-signature", log_id);
         return;
      }

      astr relativediff = (int)difference;

      if (difference > null) {
         astr lastMaskFiller = m_Mask.pos(-1);
         m_Mask = str::format::fill_character(m_Mask, lastMaskFiller, difference);

         cmverbose(
            relativediff.prepend("weak/short Masking, added last token as filling \n hint : add to it by "),
            log_id
         );

         return;
      }
      else if (difference < null) {
         astr MaskFix = m_Mask.pop(difference);
         cmverbose(relativediff.prepend("overbearing was mask was cutdown by "), log_id);
         return;
      }
   }


   // base formatter  interpreter and signature conv
   //  MEMORY IS NOT DEALLOCATED for SLOBJECT
   sigLang_Object BaseSigLConvFormat (sigLangSTR interpret, sigLang_Object &gen) {
      Parser parse(interpret);

      astr idToken = parse.ExtractWord("id");
      auto sectionToken = parse.ExtractWord("section");

      if (idToken.Positive()) {
         gen.id = idToken.ExtractWord("id.\a");
         parse = parse.replace(idToken, "");
      }
      if (sectionToken.Positive()) {
         gen.section = sectionToken.ExtractWord("section.\a");
         parse = parse.replace(sectionToken, "");
      }
      else {
         gen.section = acstr(".text");
      }


      gen.masking = parse.ExtractWord("xx");

      if (gen.masking) {
         parse = parse.replace(gen.masking.prepend("\b"), "");
      }


      {
         // convert signature to literal binary
         // store this allocated data in signature { } .
         // we currently do not free them AC could walk pages to use our sigs lol.

         parse.gmode = true;
         auto signature_pre_conv = parse.replace(" ", "");

         if (signature_pre_conv.match("\\")) {
            gen.signature.length = (signature_pre_conv.size() / 4);
            gen.signature.pattern = str_conv::hexStr_to_bytes(signature_pre_conv);
            gen.signature.interpreted_string = signature_pre_conv;
         }
         else {
            cle_private("no signature to convert. have you forgot to use \" \\ \"", "SigLang error ");
         }
      }

      return gen;
   }

 public:
   noret
   API_siglang_interpretData (
      nodem< astr > &bundle, astr subModule, sigLang_Object::Type Type, CachedData< sigLang_Object > *sig_cache
   ) {
      try {
         astr duplicate_error;
         for (auto to_interpret : bundle) {


            sigLang_Object new_slobject;

            handleManualConfig(new_slobject, to_interpret);

            BaseSigLConvFormat(to_interpret, new_slobject);

            CompensateSignature(
               new_slobject.signature.pattern, new_slobject.signature.length,
               new_slobject.masking, new_slobject.id
            );

            // error checks go here
            {


               if (!new_slobject.masking)
                  cle_private("GEN MASKING NULL, MAKE SURE MASK COMES BEFORE PATTERN", "SigLang error");
            }

            new_slobject.type = Type;
            new_slobject.module = subModule;

            if (!new_slobject.signature.HasCustomDynamicPos) {
               sig_cache->AddItem_DuplicateCheck_ExNested< sigLangSTR, sigLangSTR, sigLang_Object::Source_Pattern >(
                  new_slobject,
                  &sigLang_Object::id,
                  &sigLang_Object::Source_Pattern::interpreted_string,
                  &sigLang_Object::signature,
                  duplicate_error
               );
            }
            else {
               sig_cache->AddItem(new_slobject);
            }
         }
      } catch (std::exception &e) {
         cle_private(e.what());
      }

      return;
   }
};


class cma::systems::Sig : private sigLang {
 protected:
   singleton_member SigMasterlib sigmaster;
   friend fw::frameworkLoader;


 private:
   singleton_member CachedData< sigLang_Object > sig_cache;

 private:
   u64 ModuleRequestSafeStr (astr ref_ module_member) {
      // cross using safely here.
      systems::Rat rat_Helper;

      if (module_member != "main") {
         return rat_Helper.GetModuleByName(module_member);
      }
      else {
         return fsl.GetModuleBase.fn();
      }
   }


 public:
   //     format :  signature .. mask .. section .= default[text]
   //     currently decapricated DO NOT USE
   auto z_decap_rtScan (astr raw_form, u64 modulebase) {

      raw_form = raw_form.prepend(acstr("id.none "));

      sigLang_Object gendata;

      BaseSigLConvFormat(raw_form, gendata);

      CompensateSignature(gendata.signature.pattern, gendata.signature.length, gendata.masking, gendata.id);


      if (!sigmaster->explicit_region(gendata.signature.pattern, gendata.masking, modulebase, gendata.section)) {
         onScanFailure(gendata.id);
      }


      return sigmaster->GetScanResult();
   }


 private:
   // TODO implement failure of scanning attempt -
   noret onScanFailure (astr failed_sig_id = 0) {
      // request sigmasters recent nearest-closest data.
      auto NearestIndex = sigmaster->GetLastScan_NearestIndex();
      astr failure_str = acstr(" nearest/failed index : ");
      failure_str = failure_str.prepend(failed_sig_id).append(NearestIndex);

      clmsg(failure_str, "[CM] signature recovery and guide.");
   }

   sigLibEcode attempt_scan (sigLang_Object aptr_ sigData) {

      u64 module_value = ModuleRequestSafeStr(sigData->module);


      auto result = sigmaster->explicit_region(
         sigData->signature.pattern,
         sigData->masking,
         module_value,
         sigData->section
      );


      if (result == sigLibEcode::unknownName_sections) {
         cle_private("sig-core unknown section!", "sigmaster report");
      }
      else if (result == sigLibEcode::caught_errors_runAbort) {
         cle_private("sig-core input error!", "sigmaster report");
      }

      return result;
   }

   // we have succesfully managed to get our pattern.
   bool isGoodSigCacheData (sigLang_Object *sigdata) {
      return sigdata->results.GetInstructionAddress() > null;
   }

   u64 fallbackScan_MultiID (astr id) {

      u64 fallback_result = null;

      auto cache = sig_cache.GetCache();

      cache->for_each_await([&] (sigLang_Object &sigData) {
         if (sigData.id != id)
            return StatusCode::Unresolved;

         auto FeedbackComplete = attempt_scan(ref_ sigData);

         if (FeedbackComplete == sigLibEcode::Scanner_Success) {
            fallback_result = sigmaster->GetScanResult();

            return StatusCode::Complete;
         }
         else {
            onScanFailure(sigData.id);
            cache->remove_with_object(&sigData);
            return StatusCode::Unresolved;
         }
      });

      return fallback_result;
   }

 public:
   

   auto findObject (astr id, aint opt_read_into_offset = 0xdeadbeef) {
      auto object_siglang = findCaches(id, sigLang_Object::object);
 

      u64 instruction = object_siglang->results.GetInstructionAddress();

      // u64 objectRead_sigmaster = sigmaster->Trace_dotText_ObjectData(
      //    instruction,
      //    object_siglang->signature.DynamicPosition
      // );

      u64 objectRead = null;

      
      {
         u64 instAddress = instruction;
         u32 instrel = 0;

         fsl.readMemory(instAddress + object_siglang->signature.DynamicPosition, sizeof(instrel), &instrel);
 

         u16 is_using_farInstructAsSig_modifier = (object_siglang->signature.DynamicPosition - 3);

         objectRead = ((instAddress + is_using_farInstructAsSig_modifier) + 7) + instrel;
      }

      object_siglang->results.SetDynamicValue(objectRead);

#if ENABLE_CM_VERBOSE_SIG == true
      object_siglang->m_VerboseInfo.OnDynamicLog(objectRead, sigLang_Object::Type::object);
#endif

      if (opt_read_into_offset != 0xdeadbeef) {
         fsl.readMemory(objectRead + opt_read_into_offset, 8, &objectRead);
      }

      return objectRead;
   }


   class offset_bytes {
    public:
      offset_bytes(auto val) : data(val) {}
      offset_bytes() : data(0) {}

      union A {
         u16 offset_16 : 16;
         u16 offset_8 : 8;
      };
      A data;


      operator u8() {
         return this->data.offset_8;
      }
      operator u16() {
         return this->data.offset_16;
      }
   };

   //   we do some reads here with our sig-core response
   //   auto updating offsets never made easier
   template < typename T = u16 >
   T findOffset (astr id) {
      auto object_siglang = findCaches(id, sigLang_Object::offset);

      uint game_offset = object_siglang->results.GetDynamicValue();

      if (game_offset) {
         offset_bytes offset_pulled = u16(game_offset);
         return (T)offset_pulled;
      }
      else {
         // save the offset for the first time
         u64 instruction = object_siglang->results.GetInstructionAddress();


         offset_bytes offset_pulled;


         fsl.readMemory.fn(
            instruction + object_siglang->signature.DynamicPosition,
            sizeof(offset_pulled.data),
            pvoid(&offset_pulled.data)
         );

         // save it moron
         object_siglang->results.SetDynamicValue(offset_pulled.data.offset_16);

#if ENABLE_CM_VERBOSE_SIG == true
         object_siglang->m_VerboseInfo.OnDynamicLog((T)offset_pulled, sigLang_Object::Type::offset);
#endif

         return (T)offset_pulled;
      }
   }


  

   noret addObject (nodem< sigLangSTR > bundle, astr subModule = "main") {
      API_siglang_interpretData(bundle, subModule, sigLang_Object::object, ref_ sig_cache);
   }


  

   noret addOffset (nodem< astr > bundle, astr subModule = "main") {
      API_siglang_interpretData(bundle, subModule, sigLang_Object::offset, ref_ sig_cache);
   }

 private:
   auto Resolve_SLObject_Completed (sigLang_Object *object, u64 instruction) {
#if ENABLE_CM_VERBOSE_SIG == true
      auto &verbose = object->m_VerboseInfo.string;

      verbose = sigmaster->generateInstructorInfo(
         instruction
      );

      verbose = astr("VERBOSE:SIG Succesful name =  [ ").append(object->id.append(" ] \n")) + verbose.prepend(" | ");
#endif

      object->results.SetInstructionAddress(instruction);
   }

   auto Resolve_SLObject_Failure (sigLang_Object *&object) {

      onScanFailure(object->id);

      auto failed_sig_id = object->id;

      sig_cache.GetCache()->remove_with_object(object);
      object = 0;

      u64 fallback_value = fallbackScan_MultiID(failed_sig_id);

      if (fallback_value == null) {
         cle_private("bad scan multi-id failed or never utilized result!", " [-CMFWSIG TOTAL FAILURE-]   ");
      }
      else {
         cmverbose("MultiID saved us, good job", "[CM-SIGFW] [ MULTI_ID RESOLVED BROKEN SIGNATURE : VERY NICE ]");

         object = sig_cache.getbyID_s(failed_sig_id, ref_ sigLang_Object::id);
         Resolve_SLObject_Completed(object, fallback_value);
      }
   }

   sigLang_Object *findCaches (astr &input_id, sigLang_Object::Type expecting_type) {

      sigLang_Object *sigData = null;

      sigData = sig_cache.getbyID_s(input_id, ref_ sigLang_Object::id);

      if (sigData->type != expecting_type) {
         cle_private(astr("cache-invalidation you stupid fuck => ").append(sigData->id), " [CM] cache-invalidation  object/offset api's have been mixed!!!");
      }

      if (isGoodSigCacheData(sigData)) {
         return sigData;
      }
      else {

         auto FeedbackComplete = attempt_scan(sigData);

         if (FeedbackComplete == sigLibEcode::Scanner_Success) {
            Resolve_SLObject_Completed(sigData, sigmaster->GetScanResult());
         }
         else {
            Resolve_SLObject_Failure(sigData);
         }
      }

      return sigData;
   }


    
};


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
////
////  @s.gui extended framework
////
////
////
////


#include "gmapi.h"
class cma::systems::Gui : public gma::gmapi {
 public:
};
