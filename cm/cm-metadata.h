#pragma once
#define CM_METADATA_H
#include "_cmarch.h"
#include "_engine-abs.h"

// do not use this for client object
CM_IMPL bool
IsolRCallbacks (auto readFn, auto &Entity) {
   try {
      readFn(Entity);
   } catch (astr &e) {
      auto e_ = e;
      return false;
   };

   return true;
}

namespace {
   enum class __nomd { no_attached_data };
   class __nomd_class {
    public:
      bool no_attached_data;
   };
}   // namespace


namespace cma {
   template < typename T, typename t_info, typename T_Addrs, typename Attached_t >
   class Entitylist;
   template < typename T, typename t_info, typename t_Addrs, typename Attached_t >
   class EInstances;

   template < typename T, typename T_Addrs >
   class Entity;

   template < typename T, typename T_Addrs >
   Entity(T, T_Addrs) -> Entity< T, T_Addrs >;

}   // namespace cma

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
////
////  @s.Entity Base
////
////
////
////

#define teamNum_unused -0xdeadbeef
namespace cma {

   template < typename T, typename T_Addrs >
   class Entity : public T {
    public:
      struct core_intel {
         int AccessID, slot;
         bool IsClientFromList;
         bool GoodEntity;
      };

    protected:
      T_Addrs AddressPages;
      core_intel intel;

    public:
      int teamid;
      bool team_b;

      template < typename A, typename B, typename C, typename D >
      friend class Entitylist;
      template < typename A, typename B, typename C, typename D >
      friend class EInstances;

    public:
      Entity(T, T_Addrs) : T(unsets), AddressPages{unsets}, intel(unsets), teamid(teamNum_unused), team_b(unset) {}


      auto &m_GetAddress () {
         return AddressPages;
      }

      auto &ml () {
         return AddressPages;
      }
      auto m_isGoodEntity () {
         return intel.GoodEntity;
      }
      const core_intel _ref m_intel () noexcept {
         return intel;
      }
   };

   template < typename T, typename T_Addrs >
   class Client : public Entity< T, T_Addrs > {
      using Entity< T, T_Addrs >::Entity;

      // client serving data
    public:
      template < typename fn1 = bool (*)(Entity< T, T_Addrs > &self) >
      noret Serve (fn1 cb_client) {
         this->intel.GoodEntity = cb_client(*this);
      }
   };

   template < typename T, typename T_Addrs >
   Client(T, T_Addrs) -> Client< T, T_Addrs >;

}   // namespace cma


template < typename T, typename T_Addrs >
using EntityBase = cma::Entity< T, T_Addrs >;


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
////
////  @s.entitylist abstracts and more
////
////  r
////
////


namespace __cm_entity_sub {
   // entity instances data core
   template < typename T >
   class EBaseInfo {

    public:
      noret SetPlayerCount (int num) {
         player_count = num;
      }

      noret SetEntityBaseAddress (auto Address) {
         entryAddress = Address;
      }

      auto &GetEntryAddress () {
         return entryAddress;
      }

      int &GetPlayerCount () {
         return player_count;
      }

    private:
      int player_count;   // player count duh.
      u64 entryAddress;


    public:
    public:
      T extended;

      EBaseInfo() : player_count(unset), entryAddress(unset) {}
   };


   // custom inc default is 8, you can set your own walk method
   // euclid is also based here in cache
   class EWalker {
    public:
      class Base {
       public:
         singleton_member noret Entity_inc (u64 &Address, int &count, int inc_by_entity) {


            Address = (count)
                         ? Address + inc_by_entity   // bytes...
                         : Address;
            count--;                                 // decrement backwards its cooler.
         }
      };

    protected:
      int custom_int_inc;
      bool has_custom_inc;


    public:
      auto SetCustomIncEntity (int num) {
         custom_int_inc = num;
      }
      auto GetCustomIncEntity () {
         return custom_int_inc;
      }


      EWalker() : has_custom_inc(false), custom_int_inc(8) {};
   };


}   // namespace __cm_entity_sub
namespace {
   using namespace __cm_entity_sub;
}


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
////
////  @s.entity safety and read / walk Predicter
////
////
////
////

namespace cma::exceptions {
   class Entity : public class_templ::GetSet< astr > {
    public:
   };
}   // namespace cma::exceptions


enum class isoe : u16 {
   ZERO_ENTITIES_WERE_GOOD = 0x1000,
   ENTITIES_WAS_SERVED = 0x500,
   ENTITIES_BEING_SERVED
};

class IsoEntitiesRWExceptHandler {
   bool bResourceStateSaving;

 public:
   class Flags {
    public:
      bool dis_bResourceIntelligence;
   };
   Flags flags;

   IsoEntitiesRWExceptHandler() : flags(unsets), bResourceStateSaving((false)) {

                                  };


   // algorithm for us to know when game is in play and save resources

   template < typename T, typename T_addr >
   inline auto MonitorResourceIntel (const EntityBase< T, T_addr > _ref Entity_structure, const u16 NumOfEInstances, const u16 &CurrentEinstance, const u16 &bWalkerResult) {

      if (!bResourceStateSaving and misc::is_empty< T >::value) {
         bResourceStateSaving = true;
      }
      else if (true) {
      }
      else {
      }
   }
   // algorithm for us to know when game is in play and save resources
   auto inline ResourceIntelState() {
      return bResourceStateSaving;
   }
};


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
////
////  @s.core entity api
////
////
////
////


template < typename T, typename t_info, typename t_Addrs, typename Attached_t >
class cma::EInstances {

 protected:
   friend class cma::Entitylist< T, t_info, t_Addrs, Attached_t >;
   nodem< EntityBase< T, t_Addrs > > list;


   template < typename attachments >
   class attachedInstanceData {
    public:
      attachedInstanceData() {
         if (misc::type_is< attachments, __nomd >::result == true) {
            hasAttachments = false;
         }
         else {
            hasAttachments = true;
         }
      }

      auto update (uint hash, const pvoid pEntiy) {
         holder< attachments > d_t;
         d_t.hash_id = hash;
         d_t.pEntity = pEntiy;
         list.addNew(d_t);
      }

      template < typename T_hashed_attach >
      class holder {
       public:
         holder() : hash_id(null), pEntity(null) {}
         uint hash_id;
         pvoid pEntity;
         T_hashed_attach data;
      };

      nodem< holder< attachments > > list;
      bool hasAttachments;

      holder< attachments > *current_attached_pointer;
   };

   attachedInstanceData< Attached_t > list_EntityAttachments;

   auto
   createNewEntity () {
      cma::Entity new_entity_instance(T{}, t_Addrs{});
      return list.addNew(new_entity_instance);
   }

   auto
   Entity_Update (u64 relativeAddress, int currentEntityCount, auto requirement_result) {
      cma::exceptions::Entity handler;
      // dog please dont be this stupid
      {

         if (!requirement_result) {
            handler.Set(acstr("CM-EList not ready"));
            throw handler;
         }
      }

      if (currentEntityCount == list.Length()) {
         return;
      }
      else if (!list.occupied() or
               list.Length() < currentEntityCount and currentEntityCount < 100) {

         // update entity
         for (int i = 0; i < currentEntityCount; i++) {
            cma::Entity new_entity_instance(T{}, t_Addrs{});
            new_entity_instance.intel.slot = i;
            auto ptr = list.addNew(new_entity_instance);

            list_EntityAttachments.update((u64)(ptr) & 0xFFFF, ptr);
         }
      }
      else {
         handler.Set(acstr("Crazy Entity num supplied"));
         throw handler;
      }
   }
   auto SyncAttachedInstancebyHash (T &entity) {
      auto hash_input = ((u64)(&entity) & 0xFFFF);

      if (auto recent = this->list_EntityAttachments.list.GetTraveler()) {
         if (recent->data.hash_id == hash_input)
            this->list_EntityAttachments.current_attached_pointer = &recent->data;
         return;
      }

      for (auto &attachments : this->list_EntityAttachments.list) {
         if (hash_input == attachments.hash_id) {
            this->list_EntityAttachments.current_attached_pointer = &attachments;
            list_EntityAttachments.list.SetTraveler(list_EntityAttachments.list.iterator_scope);
            break;
         }
      }
   }
   auto User_Receive_AttachedInstance () {
      return list_EntityAttachments.current_attached_pointer;
   }
};


template < typename T, typename t_info, typename T_Addrs, typename Attached_t >
class cma::Entitylist : private cma::EInstances< T, t_info, T_Addrs, Attached_t > {
 public:
   Entitylist(T EntityData) {};

   Entitylist(T EntityData, t_info Metadata) {
      memset(&metadata.extended, null, sizeof(metadata.extended));
   };

   Entitylist(T EntityData, t_info Metadata, T_Addrs i_Addr) {
      memset(&metadata.extended, null, sizeof(metadata.extended));
   };

 private:
   EBaseInfo< t_info > metadata;
   EWalker walker;
   using EInstances< T, t_info, T_Addrs, Attached_t >::list;
   using EInstances< T, t_info, T_Addrs, Attached_t >::list_EntityAttachments;

   FnRouteList< void (*)(T &, u64) > routes_rw;
   FnRouteList< void (*)(T &) > routes_features;
   FnRouteList< void (*)(T &) > routes_walker;

   int i = 0;

 private:
   bool RouteCallRequirements () {
      return metadata.GetEntryAddress() > null && metadata.GetPlayerCount() > null;
   }

   auto SetPlayerAccessNumber (u32 i_new) {
      i = i_new;
      return i;
   }
   auto IncPlayerAccessNumber (u32 i_inc_by) {
      i = ((i + i_inc_by) < metadata.GetPlayerCount()) ? i + i_inc_by : 1;
      return i;
   }

 public:
   auto _ref _abstract () {
      return metadata;
   }

   auto _ref GetUserMeta () {
      return metadata.extended;
   }
   auto _ref GetAttachedData () {
      return *this->User_Receive_AttachedInstance();
   }

   int GetPlayerAccessNumber () {
      return i;
   }

   class Configurative_Algorithm {
      bool bResourceStateSaving;

    public:
      class Flags {
       public:
         bool dis_bCallsToRWS;
         bool dis_bResourceIntelligence;
      };
      Flags flags;

      Configurative_Algorithm() : flags(unsets), bResourceStateSaving((false)) {

                                  };


      // algorithm for us to know when game is in play and save resources

      inline auto MonitorResourceIntel (const T _ref Entity_structure, const u16 NumOfEInstances, const u16 &CurrentEinstance, const u16 &bWalkerResult) {

         if (!bResourceStateSaving and misc::is_empty< T >::value) {
            bResourceStateSaving = true;
         }
         else if (true) {
         }
         else {
         }
      }
      // algorithm for us to know when game is in play and save resources
      auto inline ResourceIntelState() {
         return bResourceStateSaving;
      }
   };
   Configurative_Algorithm Config;

   Configurative_Algorithm::Flags &config () {

      return Config.flags;
   }

   auto _ref GetList () {
      return this->list;
   }


   using Efnp = noret (*)(u64 &entity, int &num);
   using EfnWalker = noret (*)(u64 &Walked_address);
   using PfnRWC = noret (*)(T &entity, u64 entity_address);


   /* EntityProviderfn whatever Entity Address you want you must provide num of entities

   ReadFn you read here [ you receive entity and current entity_address ]

   WriteFn you write here  [ you receive entity and current entity_address ]

   WalkerFn you navigate the entity    [ you receive entity and current entity_address ]

   */


 private:
 public:
   // you set count and base address yourself  inside callback provider
   // remember i is all that is needed to walk entity i.e (0x8 * i )
   template < typename cfw, typename cwr, typename cww >
   noret ServeInstances (cfw walkerFn, cwr ReadFn, cww WriteFn = null) {

      u64 EntityBase = this->metadata.GetEntryAddress();
      int entity_count = metadata.GetPlayerCount();


      u64 relativeAddress = EntityBase;

      this->Entity_Update(relativeAddress, entity_count, RouteCallRequirements());


      list.for_each([&] (Entity< T, T_Addrs > &entity) {
         SetPlayerAccessNumber(entity.intel.slot);

         SyncAttachedInstancebyHash(entity);


         if ((entity.intel.GoodEntity = walkerFn(relativeAddress, entity))) {

            if (!config().dis_bCallsToRWS) {

               if (!IsolRCallbacks(ReadFn, entity))
                  return;

               if ((pqword)(&WriteFn) > (pqword)0)
                  WriteFn(entity);
            }

            entity.intel.AccessID = GetPlayerAccessNumber();
         }
         else {
            // youd handle GoodEntity failure here in some later impl using the resource bullshit thingy or whatever
         }
      });
   }


   // debug version seperate loops
   // this one runs walkers seperately better for debugging purposes
   // NOT IMPLEMENTED OR READY TO USE TODO

   // crucial data for the high-algo to work.
   auto provideInfo (u64 Entry_Address, int count) {
      metadata.SetPlayerCount(count);
      metadata.SetEntityBaseAddress(Entry_Address);
   };


   auto provideInfo (Efnp EProviderfn) {
      auto &BaseEnt = metadata.GetEntryAddress();
      auto &count = metadata.GetPlayerCount();
      EProviderfn(BaseEnt, count);

      metadata.SetEntityBaseAddress(BaseEnt);
      metadata.SetPlayerCount(count);
   }


 private:
   Entity< T, T_Addrs > _ptr MarkedClient = null;

 public:
   // can be super useful if you want to use your entity data
   auto MarkAsClient (Entity< T, T_Addrs > &entity) {
      entity.intel.IsClientFromList = true;
      MarkedClient = &entity;
   }
   // the marked client in the entitylist
   // do NOT fully depend on the data structure of the client in the entitylist.
   // localplayer data is always superior to its clone unless we have data in it we dont want to re-read
   auto &Client () {
      if (!MarkedClient) {
         auto unset_markClient_error = acstr("swear-word YOU HAVENT CALLED MARKASCLI");
         throw unset_markClient_error;
      }
      else {
         return deref MarkedClient;
      }
   }

   bool m_isclient (Entity< T, T_Addrs > &entity) {
      return entity.intel.IsClientFromList;
   }
   bool m_isbadEntOrClient (Entity< T, T_Addrs > &entity) {
      return (entity.intel.IsClientFromList or !entity.m_isGoodEntity()) ? true : false;
   }

   // triggers entity features routes, should only be used ONE PLACE!!!!
   // MULTITHREAD COMPATIBLE route > in other threaad < consume!
   auto run_features () {
      list.for_each([&] (T &entity) {
         routes_features.run(true, entity);
      });
   }

   auto run_rws (EfnWalker dowalk) {
      auto entry = metadata.GetEntryAddress();
      int count = metadata.GetPlayerCount();
      int i = 0;
      this->Entity_Update(entry, count, this->RouteCallRequirements());

      const auto rw_list = routes_rw._abstract();

      list.for_each([&] (T &entity) {
         SyncAttachedInstancebyHash(entity);
         routes_rw.run(true, entity, entry);
         dowalk(entry);
         i++;
      });
   }


   // callback  feature
   template < typename localfn >
   auto CBFeature (localfn fnCallback) {
      list.for_each([&] (T &entity) {
         fnCallback(entity);
      });
   }


   // read-write impl route [ requires run fn ]
   noret route (PfnRWC EfnRoute) {
      routes_rw.Link(EfnRoute);
   }


   using feature_route = void (*)(T);
   // Feature impl route [ requires run fn ]
   noret route_features (feature_route FeatureFn) {
      routes_features.Link(FeatureFn);
   }
};

namespace cma {

   template < typename T >
   Entitylist(T) -> Entitylist< T, __nomd, __nomd, __nomd >;

   template < typename T, typename t_info >
   Entitylist(T, t_info) -> Entitylist< T, t_info, __nomd, __nomd >;

   template < typename T, typename t_info, typename t_Addrs >
   Entitylist(T, t_info, t_Addrs) -> Entitylist< T, t_info, t_Addrs, __nomd >;

   template < typename T, typename t_info, typename t_Addrs, typename Attached_t >
   Entitylist(T, t_info, t_Addrs, Attached_t) -> Entitylist< T, t_info, t_Addrs, Attached_t >;
}   // namespace cma
