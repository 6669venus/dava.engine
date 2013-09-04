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


#ifndef LevelPerformanceTest_SettingsManager_h
#define LevelPerformanceTest_SettingsManager_h

#include "DAVAEngine.h"

using namespace DAVA;

class SettingsManager: public Singleton<SettingsManager>
{
public:
    void InitWithFile(const FilePath& filename);
	
	Vector2 GetLandscapePartitioningSize() const;
	const String GetLandscapeNodeName() const;
	float32 GetCameraElevation() const;
	float32 GetCameraRotationSpeed() const;
	float32 GetCameraMovementSpeed() const;
	float32 GetMinFps() const;
	uint32 GetMinFpsSectorCount() const;
	uint8 GetColorTransparency() const;
	Color GetColorByFps(float32 fps) const;
private:
	Vector2 landscapePartitioningSize;
	String landscapeNodeName;
	float32 cameraElevation;
	float32 cameraRotationSpeed;
	float32 cameraMovementSpeed;
	float32 minFps;
	uint32 minFpsSectorCount;
	uint8 colorTransparency;
	Vector<Color> colorList;
	Vector<float32> fpsList;
};

#endif
