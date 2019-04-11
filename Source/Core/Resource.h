#pragma once

#include "Core/Core.h"

#if defined(B2D_EDITOR) || defined(B2D_BUILD_DEBUG)
    #define RESOURCE_FALLBACK
#endif

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
    IResource() = default;
	virtual ~IResource() = default;

private:
    IResource(IResource const&) = delete;
    IResource& operator=(IResource const&) = delete;

public:
	virtual bool Load(ResourcePath const& filePath) = 0;
	static constexpr auto GetFallbackResourcePath();
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
    static ResourcePtr<T> Get(ResourcePath const& path)
    {
        B2D_STATIC_ASSERT_TYPE(IResource, T);
		
        if (mLoadedResources.find(path) == mLoadedResources.end())
		{
#ifndef B2D_NO_LOGGING
			auto loadingStartTime = std::chrono::high_resolution_clock::now();
#endif

			T* resource = new T();
			if (resource->Load(path))
			{
				mLoadedResources.emplace(path, resource);

#ifndef B2D_NO_LOGGING
				std::chrono::duration<double> elapsedTime = std::chrono::high_resolution_clock::now() - loadingStartTime;
				B2D_CORE_INFO("Loaded resource: {0} ({1}s)", path, elapsedTime.count());
#endif
			}
			else
			{
				delete resource;
				mLoadedResources.emplace(path, nullptr);

				B2D_CORE_ERROR("Failed to load resource: {0}", path);

#ifdef RESOURCE_FALLBACK
				mLoadedResources[path] = TryGetFallback<T>();
#endif
			}
        }

        return ResourcePtr<T>(mLoadedResources.find(path));
    }

	template<typename T>
	static bool Reload(ResourcePtr<T> const resourcePtr)
	{
#ifdef B2D_EDITOR
        B2D_STATIC_ASSERT_TYPE(IResource, T);

#ifdef B2D_DEBUG_MEMORY
        _CrtMemState memBefore;
        _CrtMemCheckpoint(&memBefore);
#endif

		ResourcePath const& path = resourcePtr.mPtr->first;
        IResource const* resource = resourcePtr.mPtr->second;

        B2D_ASSERT(mLoadedResources.find(path) != mLoadedResources.end());

		ResourceList::iterator fallback = mLoadedResources.find(T::GetFallbackResourcePath());
		if (resourcePtr == nullptr || (fallback != mLoadedResources.end() && fallback->second == resource))
		{
			T* newResource = new T();
			if (newResource->Load(path))
			{
				mLoadedResources[path] = newResource;

				B2D_CORE_INFO("Reloaded resource: {0}", path);
				return true;
			}
			else
			{
				delete newResource;

				B2D_CORE_ERROR("Failed to reload resource: {0}", path);
				return false;
			}
		}

		IResource* newResource = const_cast<IResource*>(resource);

		resourcePtr->~T();
		new(newResource) T();
		
		if (!newResource->Load(path))
		{
			delete resource;
#ifdef RESOURCE_FALLBACK
			mLoadedResources[path] = TryGetFallback<T>();
#endif
			B2D_CORE_ERROR("Failed to reload resource: {0}", path);
			return false;
		}

#ifdef B2D_DEBUG_MEMORY
		_CrtMemState memAfter;
		_CrtMemCheckpoint(&memAfter);

		_CrtMemState memDiff;
		if (B2D_CHECKf(_CrtMemDifference(&memDiff, &memBefore, &memAfter), "Memory increased during reload of resource: {0}", path))
		{
			_CrtMemDumpStatistics(&memDiff);
		}
#endif

		B2D_CORE_INFO("Reloaded resource: {0}", path);
		return true;
#else
		return false;
#endif
	}

#ifdef RESOURCE_FALLBACK
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

		ResourcePath const path = T::GetFallbackResourcePath();
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
