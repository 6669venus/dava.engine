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



#ifndef __RESOURCEEDITORQT__VISIBILITYTOOLPROPERTIESVIEW__
#define __RESOURCEEDITORQT__VISIBILITYTOOLPROPERTIESVIEW__

#include <QWidget>
#include "DAVAEngine.h"
#include "../Scene/System/VisibilityToolSystem.h"

using namespace DAVA;

class SceneEditor2;

namespace Ui
{
	class VisibilityToolPropertiesView;
}

class VisibilityToolPropertiesView: public QWidget
{
	Q_OBJECT

public:
	explicit VisibilityToolPropertiesView(QWidget* parent = 0);
	~VisibilityToolPropertiesView();

	void Init();

private slots:
	void SceneActivated(SceneEditor2* scene);
	void SceneDeactivated(SceneEditor2* scene);

	void SetVisibilityToolButtonsState(SceneEditor2* scene,
									   VisibilityToolSystem::eVisibilityToolState state);
	void Toggle();
	void SaveTexture();
	void SetVisibilityPoint();
	void SetVisibilityArea();
	void SetVisibilityAreaSize(int areaSize);

private:
	Ui::VisibilityToolPropertiesView* ui;
	SceneEditor2* activeScene;

	void SetWidgetsState(bool enabled);
	void BlockAllSignals(bool block);
	void UpdateFromScene(SceneEditor2* scene);
};

#endif /* defined(__RESOURCEEDITORQT__VISIBILITYTOOLPROPERTIESVIEW__) */
