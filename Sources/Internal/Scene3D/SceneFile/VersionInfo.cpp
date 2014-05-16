/*==================================================================================
 Copyright (c) 2008, binaryzebra
 All rights reserved.
 
 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
 
 * Redistributions of source code must retain the above copyright
 notice, this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright
 notice, this list of conditions and the following disclaimer in the
 documentation and/or other materials provided with the distribution.
 * Neither the name of the binaryzebra nor the
 names of its contributors may be used to endorse or promote products
 derived from this software without specific prior written permission.
 
 THIS SOFTWARE IS PROVIDED BY THE binaryzebra AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL binaryzebra BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 =====================================================================================*/

#include "VersionInfo.h"
#include "Utils/StringFormat.h"

#include <sstream>


namespace DAVA
{

    VersionInfo::VersionInfo()
    {
        FillVersionHistory();
        SetCurrentBranch();
    }

    VersionInfo::~VersionInfo()
    {
    }

    void VersionInfo::AddVersion(const VersionInfo::SceneVersion& version)
    {
        DVASSERT(versionMap.find(version.version) == versionMap.end());
        versionMap.insert(VersionMap::value_type(version.version, version));
    }

    const VersionInfo::SceneVersion& VersionInfo::GetCurrentVersion() const
    {
        DVASSERT(!versionMap.empty());
        return versionMap.rbegin()->second;
    }

    String VersionInfo::UnsupportedTagsMessage(const SceneVersion& version) const
    {
        const TagsMap& allTags = GetTags();
        const TagsMap& errTags = GetTagsDiff(allTags, version.tags);    // List of tags that not supported by current version of framework
        const String& msg = FormatTagsString(errTags);

        return msg;
    }

    String VersionInfo::NoncompatibleTagsMessage(const SceneVersion& version) const
    {
        const TagsMap& allTags = GetTags(version.version);
        const TagsMap& warnTags = GetTagsDiff(version.tags, allTags);   // List of tags that will be added to scene
        const String& msg = FormatTagsString(warnTags);

        return msg;
    }

    VersionInfo::TagsMap VersionInfo::GetTagsDiff(const VersionInfo::TagsMap& from, const VersionInfo::TagsMap& what)
    {
        TagsMap result;

        for (TagsMap::const_iterator it = from.begin(); it != from.end(); it++)
        {
            if (what.find( it->first ) == what.end())
            {
                result.insert(TagsMap::value_type(it->first,it->second));
            }
        }

        return result;
    }

    String VersionInfo::FormatTagsString(const VersionInfo::TagsMap& tags)
    {
        std::stringstream ss;
        for (TagsMap::const_iterator it = tags.begin(); it != tags.end(); it++)
        {
            ss << it->first << '_' << it->second << std::endl;
        }

        return ss.str();
    }

    VersionInfo::TagsMap VersionInfo::GetTags(uint32 minVersion) const
    {
        TagsMap tags;

        for (VersionMap::const_iterator itVersion = versionMap.begin(); itVersion != versionMap.end(); itVersion++)
        {
            if (itVersion->first < minVersion)
                continue;

            const SceneVersion& version = itVersion->second;
            tags.insert(version.tags.begin(), version.tags.end());
        }

        return tags;
    }

    VersionInfo::eStatus VersionInfo::TestVersion(const SceneVersion& version) const
    {
        const SceneVersion& current = GetCurrentVersion();

        // Checking version
        if (current.version < version.version)
            return INVALID;

        // Checking tags
        const TagsMap& tags = version.tags;
        const TagsMap& fwAllTags = GetTags();
        const TagsMap& fwVersionedTags = GetTags( version.version );

        const TagsMap& errTags = GetTagsDiff( tags, fwAllTags );            // List of tags that not supported by current version of framework
        const TagsMap& warnTags = GetTagsDiff( fwVersionedTags, tags );     // List of tags that will be added to scene

        if ( errTags.size() > 0 )
            return INVALID;

        if ( warnTags.size() > 0 )
            return COMPATIBLE;

        return VALID;
    }

    // TODO: cleanup after debug/test/review
    void VersionInfo::FillVersionHistory()
    {
        // List of all supported featues:

        // V12
        SceneVersion v12;
        v12.version = 12;
        v12.tags.insert( TagsMap::value_type( "water", 1 ) );
        v12.tags.insert( TagsMap::value_type( "sky", 2 ) );
        AddVersion( v12 );

        // Current version
        SceneVersion currentVersion;
        currentVersion.version = 13;    // Current version of scene
        AddVersion(currentVersion);
    }

    void VersionInfo::SetCurrentBranch()
    {
        // List of featues, that are under development in current branch

        DVASSERT(!versionMap.empty());
        TagsMap& tags = versionMap.rbegin()->second.tags;

        // TODO: cleanup after debug/test/review
        //tags.insert( TagsMap::value_type( "water", 1 ) );
        //tags.insert( TagsMap::value_type( "sky", 2 ) );
        //tags.insert( TagsMap::value_type( "grass", 3 ) );
    }

}
