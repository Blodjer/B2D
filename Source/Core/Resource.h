#pragma once

#include <string>
#include <unordered_map>

class IResource
{
public:
    virtual bool Load(std::string const& filePath) = 0;

private:
    std::string path;
};

template<typename T>
class IResourceManager
{
public:
    using ResourceList = std::unordered_map<std::string, T*>;

    static T const* Get(std::string const& filePath)
    {
        if (mLoadedResources.find(filePath) != mLoadedResources.end())
        {
            T* t = new T(filePath);
            //std::unordered_map<std::string, T>::iterator it = mLoadedResources.insert(filePath, t);
            return t;
        }

        return mLoadedResources[filePath];
    }

    static ResourceList mLoadedResources;
    
    static ResourceList const& GetLoadedResources() { return mLoadedResources; }
};