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

    FORCEINLINE void AddRef() const { ++m_refCount; }
    FORCEINLINE void RemoveRef() const { --m_refCount; }

    void RegisterChangeCallback(TResourceChangedDelegate const& callback) const
    {
        m_onChangeCallbacks.emplace_back(callback);
    }

    void RemoveChangeCallback(TResourceChangedDelegate const& callback)
    {
        B2D_BREAK();
    }

private:
    void OnChange() const
    {
        for (TResourceChangedDelegate const& callback : m_onChangeCallbacks)
        {
            callback();
        }
    }

private:
    mutable uint32 m_refCount = 0;
    mutable std::vector<TResourceChangedDelegate> m_onChangeCallbacks;
};

template <typename T>
class ResourcePtr
{
	friend IResourceManager;

private:
	ResourceList::value_type* m_ptr;

private:
	ResourcePtr(ResourceList::iterator& ptr)
		: m_ptr(&*ptr)
	{
        m_ptr->second->AddRef();
	}

public:
    ResourcePtr(ResourcePtr const& other)
    {
        m_ptr = other.m_ptr;
        m_ptr->second->AddRef();
    };

	ResourcePtr& operator=(ResourcePtr const& other)
	{
		m_ptr = other.m_ptr;
        m_ptr->second->AddRef();

		return *this;
	}

    ~ResourcePtr()
    {
        m_ptr->second->RemoveRef();
    }

	T const* operator->() const
	{
		return static_cast<T const*>(m_ptr->second);
	}

	bool operator==(ResourcePtr& other) const
	{
		return m_ptr == other.m_ptr;
	}

    FORCEINLINE bool IsValid() const
    {
        return m_ptr && m_ptr->second;
    }
};

class IResourceManager
{
private:
	static ResourceList ms_loadedResources;

public:
	static ResourceList const& GetLoadedResources() { return ms_loadedResources; }

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

    if (ms_loadedResources.find(path) == ms_loadedResources.end())
    {
#ifndef B2D_NO_LOGGING
        auto loadingStartTime = std::chrono::high_resolution_clock::now();
#endif

        IResource* resource = new T();
        if (resource->Load(path))
        {
            ms_loadedResources.emplace(path, resource);

#ifndef B2D_NO_LOGGING
            std::chrono::duration<double> elapsedTime = std::chrono::high_resolution_clock::now() - loadingStartTime;
            B2D_CORE_INFO("Loaded resource: {0} ({1}s)", path, elapsedTime.count());
#endif
        }
        else
        {
            delete resource;
            ms_loadedResources.emplace(path, nullptr);

            B2D_CORE_ERROR("Failed to load resource: {0}", path);

#ifdef RESOURCE_FALLBACK
            ms_loadedResources[path] = TryGetFallback<T>();
#endif
        }
    }

    ResourceList::iterator resourceIt = ms_loadedResources.find(path);
    return ResourcePtr<T>(resourceIt);
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

    ResourcePath const& path = resourcePtr.m_ptr->first;
    IResource const* resource = resourcePtr.m_ptr->second;

    B2D_ASSERT(ms_loadedResources.find(path) != ms_loadedResources.end());

    ResourceList::iterator fallback = ms_loadedResources.find(T::GetFallbackResourcePath());
    if (!resourcePtr.IsValid() || (fallback != ms_loadedResources.end() && fallback->second == resource))
    {
        IResource* newResource = new T();
        if (newResource->Load(path))
        {
            ms_loadedResources[path] = newResource;

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
        ms_loadedResources[path] = TryGetFallback<T>();
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

    if (ms_loadedResources.find(path) == ms_loadedResources.end())
    {
        IResource* fallbackResource = new T();
        if (fallbackResource->Load(path))
        {
            ms_loadedResources.emplace(path, fallbackResource);
            fallback = static_cast<T const*>(ms_loadedResources[path]);
        }
        else
        {
            B2D_CORE_ERROR("Failed to load fallback resource: {0}", path);

            delete fallbackResource;
            ms_loadedResources.emplace(path, nullptr);
            return nullptr;
        }
    }

    return fallback;
}
#endif
