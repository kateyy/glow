#pragma once

#include <string>

#include <glow/glow_api.h>

#include <glow/gl_extensions.h>

namespace glow {

GLOW_API bool hasExtension(Extension extension);
GLOW_API bool hasExtension(const std::string & extensionName);
GLOW_API bool isInCoreProfile(Extension extension);

GLOW_API std::string extensionString(Extension extension);
GLOW_API Extension extensionFromString(const std::string & extensionName);

} // namespace glow
