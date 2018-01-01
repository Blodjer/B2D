#pragma once

template<typename... params>
class Delegate
{
	typedef void(*Type)(void* callee, params...);

	void* Object;
	Type FunctionCaller;

public:
	Delegate() {}
	Delegate(void* callee, Type function) : Object(callee), FunctionCaller(function) {}

	template <class T, void(T::*function)(params...)>
	static Delegate Create(T* object)
	{
		return Delegate(object, &Call<T, function>);
	}

	void operator()(params... xs) const
	{
		return (*this->FunctionCaller)(this->Object, xs...);
	}

private:
	template <class T, void(T::*function)(params...)>
	static void Call(void* object, params... xs)
	{
		T* p = static_cast<T*>(object);
		(p->*function)(xs...);
	}
};

#define FUNC_CONCAT(...) __VA_ARGS__
#define DECLARE_DELEGATE(DelegateName, ...) typedef Delegate<__VA_ARGS__> DelegateName
#define MAKE_DELEGATE(Delegate, Object, F) Delegate::Create<std::remove_pointer_t<decltype(Object)>, F>(Object)
#define Create(Object, Function) Create<std::remove_pointer_t<decltype(Object)>, Function>(Object)
