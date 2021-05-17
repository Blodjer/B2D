#pragma once

#define VULKAN_HPP_ASSERT(_expr) B2D_ASSERT_s(_expr, Log::ESink::External)
#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1

#include <vulkan/vulkan.hpp>

//namespace vulkan = VULKAN_HPP_NAMESPACE;
