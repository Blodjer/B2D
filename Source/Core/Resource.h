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

DECLARE_DELEGATE(TResourceChangedDelegate);

class IResource
{
	friend IResourceManager;

protected:
    IResource() = default;
	virtual ~IResource() = default;

private:
    IResource(IResource const&) = delete;
    IResource& operator=(IResource const&) = delete;

protected:
    virtual bool Load(ResourcePath const& path) = 0;
    virtual void Free() = 0;

public:
    static constexpr auto GetFallbackResourcePath();

    FORCEINLINE void AddRef() const { ++mRefCount; }
    FORCEINLINE void RemoveRef() const { --mRefCount; }

    void RegisterChangeCallback(TResourceChangedDelegate const& callback) const
    {
        mOnChangeCallbacks.emplace_back(callback);
    }

    void RemoveChangeCallback(TResourceChangedDelegate const& callback)
    {
        B2D_BREAK();
    }

private:
    void OnChange() const
    {
        for (TResourceChangedDelegate const& callback : mOnChangeCallbacks)
        {
            callback();
        }
    }

private:
    mutable uint32 mRefCount = 0;
    mutable std::vector<TResourceChangedDelegate> mOnChangeCallbacks;
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
        mPtr->second->AddRef();
	}

public:
    ResourcePtr(ResourcePtr const& other)
    {
        mPtr = other.mPtr;
        mPtr->second->AddRef();
    };

	ResourcePtr& operator=(ResourcePtr const& other)
	{
		mPtr = other.mPtr;
        mPtr->second->AddRef();

		return *this;
	}

    ~ResourcePtr()
    {
        mPtr->second->RemoveRef();
    }

	T const* operator->() const
	{
		return static_cast<T const*>(mPtr->second);
	}

	bool operator==(ResourcePtr& other) const
	{
		return mPtr == other.mPtr;
	}

    FORCEINLINE bool IsValid() const
    {
        return mPtr && mPtr->second;
    }
};

class IResourceManager
{
private:
	static ResourceList mLoadedResources;

public:
	static ResourceList const& GetLoadedResources() { return mLoadedResources; }

    template<typename T>
    static ResourcePtr<T> Get(ResourcePath const& path);

	template<typename T>
    static bool Reload(ResourcePtr<T> const resourcePtr);

#ifdef RESOURCE_FALLBACK
    template<typename T>
    static T const* TryGetFallback();
#endif
};

template<typename T>
ResourcePtr<T> IResourceManager::Get(ResourcePath const& path)
{
    B2D_STATIC_ASSERT_TYPE(IResource, T);

    if (mLoadedResources.find(path) == mLoadedResources.end())
    {
#ifndef B2D_NO_LOGGING
        auto loadingStartTime = std::chrono::high_resolution_clock::now();
#endif

        IResource* resource = new T();
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
static bool IResourceManager::Reload(ResourcePtr<T> const resourcePtr)
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
    if (!resourcePtr.IsValid() || (fallback != mLoadedResources.end() && fallback->second == resource))
    {
        IResource* newResource = new T();
        if (newResource->Load(path))
        {
            mLoadedResources[path] = newResource;

            B2D_CORE_INFO("Reloaded resource: {0}", path);
            resource->OnChange();
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

    //resourcePtr->~T();
    //new(newResource) T();

    newResource->Free();

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
    resource->OnChange();
    return true;
#else
    return false;
#endif
}

#ifdef RESOURCE_FALLBACK
template<typename T>
static T const* IResourceManager::TryGetFallback()
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
        IResource* fallbackResource = new T();
        if (fallbackResource->Load(path))
        {
            mLoadedResources.emplace(path, fallbackResource);
            fallback = static_cast<T const*>(mLoadedResources[path]);
        }
        else
        {
            B2D_CORE_ERROR("Failed to load fallback resource: {0}", path);

            delete fallbackResource;
            mLoadedResources.emplace(path, nullptr);
            return nullptr;
        }
    }

    return fallback;
}
#endif
