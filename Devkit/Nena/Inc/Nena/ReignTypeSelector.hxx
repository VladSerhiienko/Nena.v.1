#if _MSC_VER
 #pragma once
#endif

#ifndef __REIGN_TYPESELECTOR_INCLUDED__
 #define __REIGN_TYPESELECTOR_INCLUDED__

namespace Reign
{
namespace Foundation
{

	// Type selector
	template <bool Condition, typename Then, typename Else> struct If;
	template <typename Then, typename Else> struct If <true, Then, Else> { typedef Then Result; };
	template <typename Then, typename Else> struct If <false, Then, Else> { typedef Else Result; };

	// Type selector
	template <bool Condition, typename Then, typename Else> struct If
	{
		template <bool InnerCondition> struct Selector;
		template <> struct Selector <true> { typedef Then Result; };
		template <> struct Selector <false> { typedef Else Result; };
		typedef typename Selector<Condition>::Result Result;
	};

}; // Foundation
}; // Reign

#endif // !__REIGN_TYPESELECTOR_INCLUDED__
