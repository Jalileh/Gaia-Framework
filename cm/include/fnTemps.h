#pragma once
// first assignment lambda locks object.
template < typename T >
class fnObject {
 public:
   fnObject(T Fncallback) : fn(Fncallback) {
   }
   operator T() {
      return fn;
   }

   template < typename... param >
   noret operator()(param... args) const {
      fn(args...);
   }

   noret setfn (const T &newFn) {
      // if you get an error here, you probably ..
      // locked the function pointer to a lambda function as first assignment.
      fn = newFn;
   }

   noret operator=(const T &Replacement) {
      // if you get an error here, you probably ..
      // locked the function pointer to a lambda function as first assignment.

      fn = Replacement;
   }

   auto &operator=(T &Replacement) const {
      // if you get an error here, you probably ..
      // locked the function pointer to a lambda function as first assignment.
      fn = Replacement;
      return this;
   }

   T fn;
};

// basically fnObject but with attached metadata of your choice
template < typename Function, typename Metadata >
class fnObject2 : public fnObject< Function > {
 public:
   using fnObject< Function >::fnObject;

   fnObject2(Function Fncallback, Metadata data) : fnObject< Function >(Fncallback), meta(data) {}
 
   // FN:CALL with desired args..
   template < typename... Args >
   noret operator()(Args &...args) {
      this->fn(args...);
   }


   // configurate object metadata through a callback
   // please call config
   template < typename cbfnMeta >
   noret metafn (cbfnMeta modifyMeta) {
      modifyMeta();
   }


   // returns metaconfig & reference
   Metadata & config () {
      return meta;
   }

 private:
   Metadata meta;
};
// Type deduction guide for fnmdata/fnobject2 class
template < typename Function, typename Metadata >
fnObject2(Function, Metadata) -> fnObject2< Function, Metadata >;


#define fnimpl const static inline


#define fnimpl_fnObject fnimpl fnObject

// has data attachment
#define fnimpl_fnObject2 fnimpl fnObject2

#ifdef some_snippet_guides
// Function template with a non-type template parameter N representing the number of arguments
template < typename ClassType, typename... Args >
void callCallback (ClassType &object, void (ClassType::*callback)(Args...), Args... args)
   requires (not_int< Args > && ...)
{
   auto size = sizeof...(Args);
   (object.*callback)(args...);
}


#endif

 