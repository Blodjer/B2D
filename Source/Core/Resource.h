#pragma once

#include <string>
#include <type_traits>
#include <unordered_map>

class IResource;

template <typename T>
class ResourcePtr;

class IResourceManager;

using ResourcePath = std::string;
using ResourceList = std::unordered_map<ResourcePath, IResource const*>;


class IResource
{
	friend IResourceManager;

public:
	virtual ~IResource() = default;

public:
	virtual bool Load(ResourcePath const& filePath) = 0;
	static ResourcePath const GetFallbackResourcePath() { return ""; };
};

template <typename T>
class ResourcePtr
{
	friend IResourceManager;

private:
	ResourceList::value_type* mPtr;

private:
	ResourcePtr(ResourceList::iterator& ptr)
		: mPtr(&*ptr)
	{
	}

public:
	static const ResourcePtr<T> InvalidPtr;

	ResourcePtr()
		: mPtr(nullptr)
	{
	}

	ResourcePtr& operator=(ResourcePtr const& other)
	{
		mPtr = other.mPtr;

		return *this;
	}

	T const* operator->() const
	{
		return static_cast<T const*>(mPtr->second);
	}

	bool operator==(ResourcePtr& other) const
	{
		return mPtr == other.mPtr;
	}

	bool operator==(nullptr_t) const
	{
		return !mPtr || !mPtr->second;
	}

	bool operator!=(nullptr_t) const
	{
		return mPtr && mPtr->second;
	}
};

template<typename T>
ResourcePtr<T> const ResourcePtr<T>::InvalidPtr = ResourcePtr<T>();

class IResourceManager
{
	static ResourceList mLoadedResources;

public:
	static ResourceList const& GetLoadedResources() { return mLoadedResources; }

	template<typename T>
    static ResourcePtr<T> Get(ResourcePath const& filePath)
    {
		static_assert(std::is_base_of<IResource, T>::value, "Not a resource!");
		
        if (mLoadedResources.find(filePath) == mLoadedResources.end())
		{
			T* resource = new T();
			if (resource->Load(filePath))
			{
				mLoadedResources.emplace(filePath, resource);
			}
			else
			{
				delete resource;
				mLoadedResources.emplace(filePath, nullptr);
#ifndef RELEASE
				mLoadedResources[filePath] = TryGetFallback<T>();
#endif
			}
        }

        return ResourcePtr<T>(mLoadedResources.find(filePath));
    }

	template<typename T>
	static bool Reload(ResourcePtr<T> const resourcePtr)
	{
#ifndef RELEASE
		static_assert(std::is_base_of<IResource, T>::value, "Not a resource!");

		ResourcePath const& path = resourcePtr.mPtr->first;
		if (mLoadedResources.find(path) == mLoadedResources.end())
		{
			return false;
		}

#ifndef MEMDEBUG
		_CrtMemState memBefore;
		_CrtMemCheckpoint(&memBefore);
#endif

		IResource const* resource = resourcePtr.mPtr->second;

		ResourceList::iterator fallback = mLoadedResources.find(T::GetFallbackResourcePath());
		if (resourcePtr == nullptr || (fallback != mLoadedResources.end() && fallback->second == resource))
		{
			T* newResource = new T();
			if (newResource->Load(path))
			{
				mLoadedResources[path] = newResource;
				return true;
			}
			else
			{
				delete newResource;
				return false;
			}
		}

		IResource* newResource = const_cast<IResource*>(resource);

		resourcePtr->~T();
		new(newResource) T();
		
		if (!newResource->Load(path))
		{
			delete resource;
			mLoadedResources[path] = TryGetFallback<T>();
			return false;
		}

#ifndef MEMDEBUG
		_CrtMemState memAfter;
		_CrtMemCheckpoint(&memAfter);

		_CrtMemState memDiff;
		if (_CrtMemDifference(&memDiff, &memBefore, &memAfter))
		{
			_CrtMemDumpStatistics(&memDiff);
			__debugbreak();
		}
#endif

		return true;
#else
		return false;
#endif
	}

#ifndef RELEASE
	template<typename T>
	static T const* TryGetFallback()
	{
		static bool tried = false;
		static T const* fallback = nullptr;
		if (fallback != nullptr)
		{
			return fallback;
		}

		if (tried)
		{
			return nullptr;
		}

		tried = true;

		ResourcePath const& path = T::GetFallbackResourcePath();
		if (path.empty())
		{
			return nullptr;
		}

		if (mLoadedResources.find(path) == mLoadedResources.end())
		{
			T* fallbackResource = new T();
			if (fallbackResource->Load(path))
			{
				mLoadedResources.emplace(path, fallbackResource);
				fallback = static_cast<T const*>(mLoadedResources[path]);
			}
			else
			{
				delete fallbackResource;
				mLoadedResources.emplace(path, nullptr);
				return nullptr;
			}
		}

		return fallback;
	}
#endif
};
