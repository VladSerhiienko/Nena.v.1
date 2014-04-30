#include <set>
#include <vector>

#ifndef __NENA_DELEGATES_INCLUDED__
#define __NENA_DELEGATES_INCLUDED__

#if __NENA_INCLUDE_FAST_DELEGATES__
#include <FastDelegate.h>
#endif

#if __NENA_INCLUDE_SR_DELEGATES__
#ifndef SRUTIL_DELEGATE_PREFERRED_SYNTAX
#define SRUTIL_DELEGATE_PREFERRED_SYNTAX
#endif
#include <srutil\delegate\delegate.hpp>
#include <srutil\event\event.hpp>
#endif

namespace Nena
{
	template<typename _TyReturn, typename... _TyParamsExpansion>
	struct Delegate
	{
		typedef _TyReturn(*_TyFunctionCdeclStub)(void *, _TyParamsExpansion...);

		void* callee;
		_TyFunctionCdeclStub callee_callback;

		Delegate()
			: callee(nullptr)
			, callee_callback(nullptr)
		{
		}

		Delegate(void *callee, _TyFunctionCdeclStub function)
			: callee(callee)
			, callee_callback(function)
		{
		}

		template <class _TyCallee, _TyReturn(_TyCallee::*TMethod)(_TyParamsExpansion...)>
		static Delegate CreateFromFunction(_TyCallee *callee)
		{
			Delegate d(callee, &CallMethod<_TyCallee, TMethod>); return d;
		}

		template <class _TyCallee, _TyReturn(_TyCallee::*TMethod)(_TyParamsExpansion...)>
		static _TyReturn CallMethod(void *callee, _TyParamsExpansion... xs)
		{
			_TyCallee* p = static_cast<_TyCallee*>(callee); return (p->*TMethod)(xs...);
		}

		bool operator<(Delegate const &other) const
		{
			return (callee < other.callee);
		}

		bool operator==(Delegate const &other) const
		{
			return (callee == other.callee) && (callee_callback == other.callee_callback);
		}

		bool operator!=(Delegate const &other) const
		{
			return (callee != other.callee) || (callee_callback != other.callee_callback);
		}

		_TyReturn operator()(_TyParamsExpansion... xs) const
		{
			return (*callee_callback)(callee, xs...);
		}
	};

	template<typename _TyCallee, typename _TyReturn, typename... _TyParamsExpansion>
	struct DelegateMaker
	{
		template<_TyReturn(_TyCallee::*_TyCalleeFunction)(_TyParamsExpansion...)>
		static _TyReturn CallMethod(void* o, _TyParamsExpansion... xs)
		{
			return (static_cast<_TyCallee*>(o)->*_TyCalleeFunction)(xs...);
		}

		template<_TyReturn(_TyCallee::*_TyCalleeFunction)(_TyParamsExpansion...)>
		inline static Delegate<_TyReturn, _TyParamsExpansion...> Bind(_TyCallee* o)
		{
			return Delegate<_TyReturn, _TyParamsExpansion...>(o, &DelegateMaker::CallMethod<_TyCalleeFunction>);
		}
	};

	template<typename _TyCallee, typename _TyReturn, typename... _TyParamsExpansion>
	DelegateMaker<_TyCallee, _TyReturn, _TyParamsExpansion... > MakeDelegate(_TyReturn(_TyCallee::*)(_TyParamsExpansion...))
	{
		return DelegateMaker<_TyCallee, _TyReturn, _TyParamsExpansion...>();
	}


	template<typename _TyReturn, typename... _TyParamsExpansion>
	struct Event
	{
		typedef Delegate<_TyReturn, _TyParamsExpansion...> _TyExpectedDelegate;

		struct _TyExpectedDelegateHash
		{
			size_t operator()(const _TyExpectedDelegate &o) const { return o.callee + o.callee_callback; }
		};

		typedef std::vector<_TyExpectedDelegate> _TyDelegateVector;

		_TyDelegateVector subscriptions;

		Event() : subscriptions()
		{
		}

		Event &operator+=(_TyExpectedDelegate const &callback)
		{
			typedef std::set<_TyExpectedDelegate> _TyDelegateSet;

			/// Evil begins here...

			subscriptions.push_back(callback);
			_TyDelegateSet unique_delegates(subscriptions.begin(), subscriptions.end());
			subscriptions.assign(unique_delegates.begin(), unique_delegates.end());

			return (*this);
		}

		Event &operator-=(_TyExpectedDelegate const &callback)
		{
			subscriptions.erase(callback);
			return (*this);
		}

		Event const &operator()(_TyParamsExpansion... xs) const
		{
			for (auto &ref : subscriptions) ref(xs...);
			return (*this);
		}
	};

};

// A a; auto delegate = _Nena_Delegate_MakeBind(&A::crazy, &a);
#define _Nena_Delegate_MakeBind(method, instance) (Nena::MakeDelegate(method).Bind<method>(instance))
// A a; auto delegate = Nena_Delegate_MakeBind(&A::crazy, &a);
#define Nena_Delegate_MakeBind(instance, method) (Nena::MakeDelegate(method).Bind<method>(instance))

#endif // !__NENA_DELEGATES_INCLUDED__
