#include <vector>

#ifndef __NENA_DELEGATES_INCLUDED__
#define __NENA_DELEGATES_INCLUDED__

namespace Nena
{

	template<typename _TyReturn, typename... _TyParamsExpansion>
	struct Delegate
	{
		typedef _TyReturn(*_TyFunctionCdeclStub)(void *, _TyParamsExpansion...);

		void* callee;
		_TyFunctionCdeclStub callee_callback;

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

		_TyReturn operator()(_TyParamsExpansion... xs) const
		{
			return (*callee_callback)(callee, xs...);
		}
	};

	template<typename _TyCallee, typename _TyReturn, typename... _TyParamsExpansion>
	struct DelegateMaker
	{
		template<_TyReturn(_TyCallee::*foo)(_TyParamsExpansion...)>
		static _TyReturn CallMethod(void* o, _TyParamsExpansion... xs)
		{
			return (static_cast<_TyCallee*>(o)->*foo)(xs...);
		}

		template<_TyReturn(_TyCallee::*foo)(_TyParamsExpansion...)>
		inline static Delegate<_TyReturn, _TyParamsExpansion...> Bind(_TyCallee* o)
		{
			return Delegate<_TyReturn, _TyParamsExpansion...>(o, &DelegateMaker::CallMethod<foo>);
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
		typedef std::vector<_TyExpectedDelegate> Vector;

		Vector subscriptions;

		Event() : subscriptions()
		{
		}

		Event &operator+=(_TyExpectedDelegate const &callback)
		{
			subscriptions.push_back(callback);
			return (*this);
		}

		Event const &operator()(_TyParamsExpansion... xs) const
		{
			/// @note	
			///		i want this function running fast
			///		this is the fastest way i know to loop through
			///		container (vector has contigious memory layout, so its safe)
			///		its true both for debug and release builds

#if 1
			auto const sz = subscriptions.size();
			decltype(subscriptions.size()) i = 0;
			auto const *ref = &subscriptions.front();
			for (; i < sz; i++) (*ref)(xs...), ref++;

#else		
			/// this approach looks much simpler and safer
			/// but a lot slower (especially when debugging)
			for (auto &ref : subscriptions)
				ref(xs...);
#endif

			return (*this);
		}
	};
	
};

// A a; auto delegate = _Nena_Delegate_MakeBind(&A::crazy, &a);
#define _Nena_Delegate_MakeBind(method, instance) (MakeDelegate(method).Bind<method>(instance))

#endif // !__NENA_DELEGATES_INCLUDED__
