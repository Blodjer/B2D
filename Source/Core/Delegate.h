#pragma once

// Delegate with optional parameters
template<typename... params>
class Delegate
{
	// Void function signature of the caller
	using Signature = void(*)(void* object, params...);

	void* mObject; // generic pointer
	Signature mCaller; // pointer to function

public:
	Delegate() = default;
	Delegate(void* object, Signature caller) : mObject(object), mCaller(caller) {}

	// F needs to be a member function of T and has to have the the same parameters as the declared delegate
	template <class T, void(T::*F)(params...)>
	static Delegate Create(T* object)
	{
		// Create delegate object and wrap the target function in the caller
		return Delegate(object, &Call<T, F>);
	}

	void operator()(params... p) const
	{
		// Call the function template and feed it with the target object
		(*this->mCaller)(this->mObject, p...);
	}

private:
	// Template wrapper to make the call
	template <class T, void(T::*F)(params...)>
	static void Call(void* object, params... p)
	{
		// Call the target function on the target object
		T* const o = static_cast<T*>(object);
		(o->*F)(p...);
	}
};

#define DECLARE_DELEGATE(DelegateName, ...) typedef Delegate<__VA_ARGS__> DelegateName
//#define MAKE_DELEGATE(Delegate, Object, F) Delegate::Create<std::remove_pointer_t<decltype(Object)>, F>(Object)
#define Create(Function) Create<std::remove_pointer_t<decltype(this)>, Function>(this)
#define CreateExternal(Object, Function) Create<std::remove_pointer_t<decltype(Object)>, Function>(Object)
