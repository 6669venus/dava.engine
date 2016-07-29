#pragma once

#include "Base/BaseTypes.h"
#include "Base/Singleton.h"

namespace DAVA
{
class ZipArchive;

class AssetsManagerAndroid : public Singleton<AssetsManagerAndroid>
{
public:
    AssetsManagerAndroid();
    virtual ~AssetsManagerAndroid();

    void Init(const String& packageName);
    inline bool IsInitialized() const
    {
        return !!apk;
    }

    bool HasDirectory(const String& relativeDirName) const;
    bool HasFile(const String& relativeFilePath) const;
    bool LoadFile(const String& relativeFilePath, Vector<uint8>& output) const;

private:
    String packageName;

    std::unique_ptr<ZipArchive> apk;
};
};
