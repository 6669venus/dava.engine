#include <QUndoStack>
#include <QApplication>
#include <QAction>
#include <QFileInfo>
#include <QMessageBox>
#include "basecontroller.h"
#include "UI/Package/PackageWidget.h"
#include "Model/PackageHierarchy/ControlNode.h"
#include "Model/PackageHierarchy/PackageNode.h"


BaseController::BaseController(QObject *parent)
    : QObject(parent)
    , mainWindow(nullptr) //nullptr is parent
    , undoGroup(this)
    , count(0)
    , currentIndex(-1)
{
    mainWindow.CreateUndoRedoActions(undoGroup);
    connect(&mainWindow, &MainWindow::TabClosed, this, &BaseController::CloseDocument);
    connect(mainWindow.GetPackageWidget(), &PackageWidget::SelectionControlChanged, this, &BaseController::OnSelectionControlChanged);
    connect(mainWindow.GetPackageWidget(), &PackageWidget::SelectionRootControlChanged, this, &BaseController::OnSelectionRootControlChanged);
    connect(&mainWindow, &MainWindow::CloseProject, this, &BaseController::CloseProject);
    connect(&mainWindow, &MainWindow::ActionExitTriggered, this, &BaseController::Exit);
    connect(&mainWindow, &MainWindow::RecentMenuTriggered, this, &BaseController::RecentMenu);
    connect(&mainWindow, &MainWindow::OpenPackageFile, this, &BaseController::OnOpenPackageFile);
    connect(&mainWindow, &MainWindow::SaveAllDocuments, this, &BaseController::SaveAllDocuments, Qt::DirectConnection);
    connect(&mainWindow, &MainWindow::SaveDocument, this, &BaseController::SaveDocument);
    connect(&mainWindow, &MainWindow::CurrentTabChanged, this, &BaseController::SetCurrentIndex);
}

BaseController::~BaseController()
{

}

void BaseController::start()
{
    mainWindow.show();
}

void BaseController::Exit()
{
    CloseProject();
    {
        QCoreApplication::exit();
    }
}

void BaseController::OpenProject(const QString &path)
{
    CloseProject();

    if (!project.CheckAndUnlockProject(path))
    {
        return;
    }
    Result result;
    if (!project.Open(path))
    {
        result.addError(Result::CriticalError, tr("Error while loading project"));
    }
    mainWindow.OnProjectOpened(result, path);
}

void BaseController::RecentMenu(QAction *recentProjectAction)
{
    QString projectPath = recentProjectAction->data().toString();

    if (projectPath.isEmpty())
    {
        return;
    }
    OpenProject(projectPath);
}

int BaseController::CreateDocument(PackageNode *package)
{
    std::shared_ptr<Document> document(new Document(&project, package, this));
    //TODO : implement this to Add Document

    undoGroup.addStack(document->GetUndoStack());
    documents.push_back(document);
    SetCount(Count() + 1);
    int index = mainWindow.AddTab(QString(document->PackageFilePath().GetBasename().c_str()));
    return index;
}

void BaseController::CloseProject()
{
    for (int i = 0; i < documents.size(); i++)
    {
        CloseDocument(i);
    }
}

int BaseController::GetIndexByPackagePath(const QString &fileName) const
{
    QString canonicalFilePath = QFileInfo(fileName).canonicalFilePath();
    DAVA::FilePath davaPath(canonicalFilePath.toStdString());

    for (int index = 0; index < documents.size(); ++index)
    {
        if (documents.at(index)->PackageFilePath() == davaPath)
            return index;
    }

    return -1;
}

void BaseController::OnSelectionRootControlChanged(const QList<ControlNode *> &activatedRootControls, const QList<ControlNode *> &deactivatedRootControls)
{
    if (0 == Count() || -1 == CurrentIndex())
    {
        return;
    }
    documents[CurrentIndex()]->OnSelectionRootControlChanged(activatedRootControls, deactivatedRootControls);
}

void BaseController::OnSelectionControlChanged(const QList<ControlNode *> &activatedControls, const QList<ControlNode *> &deactivatedControls)
{
    if (0 == Count() || -1 == CurrentIndex())
    {
        return;
    }
    documents[CurrentIndex()]->OnSelectionControlChanged(activatedControls, deactivatedControls);
}

void BaseController::OnControlSelectedInEditor(ControlNode *activatedControls)
{
    if (0 == Count() || -1 == CurrentIndex())
    {
        return;
    }
    documents[CurrentIndex()]->OnControlSelectedInEditor(activatedControls);
}


void BaseController::OnAllControlDeselectedInEditor()
{
    if (0 == Count() || -1 == CurrentIndex())
    {
        return;
    }
    documents[CurrentIndex()]->OnAllControlDeselectedInEditor();
}

void BaseController::OnOpenPackageFile(const QString &path)
{
    if (!path.isEmpty())
    {
        int index = GetIndexByPackagePath(path);
        if (index == -1)
        {
            DAVA::RefPtr<PackageNode> package = project.OpenPackage(path);
            if (nullptr != package)
            {
                index = CreateDocument(package.Get());
            }
        }
        mainWindow.SetCurrentTab(index);
    }
}

void BaseController::CloseDocument(int index)
{
    DVASSERT(index >= 0);
    DVASSERT(index < Count()); 
    QUndoStack *undoStack = documents.at(index)->GetUndoStack();
    if (!undoStack->isClean())
    {
        QMessageBox::StandardButton ret = QMessageBox::question(qApp->activeWindow(),
            tr("Save changes"),
            tr("The file has been modified.\n"
            "Do you want to save your changes?"),
            QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
            QMessageBox::Save);
        if (ret == QMessageBox::Cancel)
        {
            return;
        }
        else if (ret == QMessageBox::Save)
        {
            SaveDocument(index);
        }
    }
    int newIndex = mainWindow.CloseTab(index);
    SetCurrentIndex(newIndex);
    undoGroup.removeStack(undoStack);
    documents.removeAt(index);
    SetCount(Count() - 1);
}

void BaseController::SaveDocument(int index)
{
    DVASSERT(index >= 0);
    DVASSERT(index < Count());
    Document &document = *documents[index].get();
    if (document.GetUndoStack()->isClean())
        return;

    DVVERIFY(project.SavePackage(document.GetPackage())); //TODO:log here
    document.GetUndoStack()->setClean();
}

void BaseController::SaveAllDocuments()
{
    for (auto &document : documents)
    {
        DVVERIFY(project.SavePackage(document->GetPackage()));
        document->GetUndoStack()->setClean();
    }
}

//properties
int BaseController::Count() const
{
    return count;
}

int BaseController::CurrentIndex() const
{
    DVASSERT(currentIndex >= -1);
    DVASSERT(currentIndex < Count());
    return currentIndex;
}

void BaseController::SetCount(int arg)
{
    if (count == arg)
        return;

    count = arg;
    emit CountChanged(arg);
}

void BaseController::SetCurrentIndex(int arg)
{
    if (currentIndex == arg) //arg = -1 when close last tab 
    {
        return;
    }

    DVASSERT(arg < documents.size());
    if (currentIndex >= 0 && currentIndex < documents.size())
    {
        Document *document = documents.at(currentIndex).get();

        disconnect(document, &Document::controlSelectedInEditor, mainWindow.GetPackageWidget(), &PackageWidget::OnControlSelectedInEditor);
        disconnect(document, &Document::allControlsDeselectedInEditor, mainWindow.GetPackageWidget(), &PackageWidget::OnAllControlsDeselectedInEditor);
        disconnect(document->GetUndoStack(), &QUndoStack::cleanChanged, &mainWindow, &MainWindow::OnCleanChanged);
    }
    mainWindow.SetDocumentToWidgets(nullptr);

    currentIndex = arg;
    if (currentIndex != -1)
    {
        Document *document = documents.at(currentIndex).get();
        connect(document, &Document::controlSelectedInEditor, mainWindow.GetPackageWidget(), &PackageWidget::OnControlSelectedInEditor);
        connect(document, &Document::allControlsDeselectedInEditor, mainWindow.GetPackageWidget(), &PackageWidget::OnAllControlsDeselectedInEditor);
        connect(document->GetUndoStack(), &QUndoStack::cleanChanged, &mainWindow, &MainWindow::OnCleanChanged);
        mainWindow.SetDocumentToWidgets(document);
    }

    emit CurrentIndexChanged(arg);
}
