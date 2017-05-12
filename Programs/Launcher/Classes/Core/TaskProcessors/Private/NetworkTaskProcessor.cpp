#include "Core/TaskProcessors/NetworkTaskProcessor.h"
#include "Core/Tasks/DownloadTask.h"
#include "Core/Receiver.h"

#include <QNetworkReply>

NetworkTaskProcessor::TaskParams::TaskParams(std::unique_ptr<BaseTask>&& task_, ReceiverNotifier notifier_)
    : task(static_cast<DownloadTask*>(task_.release()))
    , notifier(notifier_)
{
}

NetworkTaskProcessor::NetworkTaskProcessor()
    : QObject(nullptr)
    , networkAccessManager(new QNetworkAccessManager(this))
{
    connect(networkAccessManager, &QNetworkAccessManager::finished, this, &NetworkTaskProcessor::OnDownloadFinished);
    connect(networkAccessManager, &QNetworkAccessManager::networkAccessibleChanged, this, &NetworkTaskProcessor::OnAccessibleChanged);
}

NetworkTaskProcessor::~NetworkTaskProcessor() = default;

void NetworkTaskProcessor::AddTask(std::unique_ptr<BaseTask>&& task, ReceiverNotifier notifier)
{
    Q_ASSERT(task->GetTaskType() == BaseTask::DOWNLOAD_TASK);

    tasks.emplace_back(new TaskParams(std::move(task), notifier));
    StartNextTask();
}

void NetworkTaskProcessor::Terminate()
{
    if (currentTask)
    {
        //QNetworkReply::abort can call OnDownloadFinished directly and after last call currentTask will be removed
        for (int i = 0, count = currentTask->requests.size(); i < count; ++i)
        {
            auto iter = std::next(currentTask->requests.begin(), i);
            (*iter)->abort();
        }
    }
}

void NetworkTaskProcessor::StartNextTask()
{
    if (tasks.empty() || currentTask != nullptr)
    {
        return;
    }

    currentTask = std::move(tasks.front());
    tasks.pop_front();

    currentTask->notifier.NotifyStarted(currentTask->task.get());

    //on mac os x when connection fails once QNetworkAccessManager aborting each next  connection
    if (networkAccessManager->networkAccessible() != QNetworkAccessManager::Accessible)
    {
        networkAccessManager->clearAccessCache();
    }

    std::vector<QUrl> urls = currentTask->task->GetUrls();
    Q_ASSERT(urls.empty() == false);
    for (const QUrl& url : urls)
    {
        QNetworkReply* reply = networkAccessManager->get(QNetworkRequest(url));
        connect(reply, &QNetworkReply::downloadProgress, this, &NetworkTaskProcessor::OnDownloadProgress);
        currentTask->requests.push_back(reply);
    }
}

void NetworkTaskProcessor::OnDownloadFinished(QNetworkReply* reply)
{
    Q_ASSERT(currentTask->task->GetTaskType() == BaseTask::DOWNLOAD_TASK);
    reply->deleteLater();

    currentTask->requests.remove(reply);
    if (reply->error() != QNetworkReply::NoError)
    {
        if (reply->error() != QNetworkReply::OperationCanceledError)
        {
            currentTask->task->SetError(QObject::tr("Network error: %1").arg(reply->errorString()));
            OnNetworkError();
        }
        else
        {
            currentTask->task->SetError(QObject::tr("Operation cancelled"));
        }
    }
    else
    {
        currentTask->task->AddLoadedData(reply->readAll());
    }
    if (currentTask->requests.empty())
    {
        currentTask->notifier.NotifyFinished(currentTask->task.get());
        currentTask = nullptr;
        StartNextTask();
    }
}

void NetworkTaskProcessor::OnAccessibleChanged(QNetworkAccessManager::NetworkAccessibility accessible)
{
    if (accessible == QNetworkAccessManager::NotAccessible)
    {
        OnNetworkError();
    }
    //run next non-network task
    StartNextTask();
}

void NetworkTaskProcessor::OnNetworkError()
{
    Terminate();
}

void NetworkTaskProcessor::OnDownloadProgress(qint64 bytes, qint64 total)
{
    //on connection abort we have an empty currentTask and downloadProgress with arguments 0, 0
    if (currentTask == nullptr)
    {
        return;
    }
    size_t size = currentTask->task->GetUrls().size();
    Q_ASSERT(size > 0);
    float multiplier = (100.0f * (size - currentTask->requests.size() + 1)) / size;
    int progress = ((static_cast<float>(bytes)) / total) * multiplier;
    currentTask->notifier.NotifyProgress(currentTask->task.get(), progress);
}
