#include <DLCManager/DLCManager.h>
#include <FileSystem/File.h>
#include <FileSystem/FileSystem.h>
#include <DLC/Downloader/DownloadManager.h>
#include <Concurrency/Thread.h>
#include <Logger/Logger.h>
#include <Engine/Engine.h>
#include <EmbeddedWebServer.h>

#include "UnitTests/UnitTests.h"

#ifndef __DAVAENGINE_WIN_UAP__

const DAVA::FilePath documentRootDir("~res:/");
const char* const localPort = "8080";

struct FSMTest02
{
    enum State
    {
        WaitInitializationFinished,
        WaitSecondConnectAttempt,
        WaitDownloadAllFourPacks,
    };
    State state = WaitInitializationFinished;
    DAVA::float32 time = 0.0f;
    DAVA::float32 waitSecondConnect = 3.0f;
    DAVA::DLCManager::Progress progressAfterInit;

    bool Update(DAVA::float32 dt)
    {
        DAVA::DLCManager& dlcManager = *DAVA::GetEngineContext()->dlcManager;

        time += dt;

        switch (state)
        {
        case WaitInitializationFinished:
        {
            if (dlcManager.IsInitialized())
            {
                state = WaitSecondConnectAttempt;
                DAVA::StopEmbeddedWebServer();
                progressAfterInit = dlcManager.GetProgress();
                return false;
            }
        }
        break;
        case WaitSecondConnectAttempt:
        {
            // TODO how to check second connect Attemp?
            TEST_VERIFY(dlcManager.IsInitialized());

            TEST_VERIFY(dlcManager.IsRequestingEnabled());

            auto currentProgress = dlcManager.GetProgress();
            TEST_VERIFY(currentProgress.alreadyDownloaded <= currentProgress.total);
            TEST_VERIFY(currentProgress.inQueue == progressAfterInit.inQueue);

            waitSecondConnect -= dt;
            if (waitSecondConnect <= 0.f)
            {
                if (!DAVA::StartEmbeddedWebServer(documentRootDir.GetAbsolutePathname().c_str(), localPort))
                {
                    TEST_VERIFY(false && "can't start server");
                }
                state = WaitDownloadAllFourPacks;
                return false;
            }
        }
        break;
        case WaitDownloadAllFourPacks:
        {
            auto currentProgress = dlcManager.GetProgress();
            TEST_VERIFY(currentProgress.alreadyDownloaded <= currentProgress.total);
            TEST_VERIFY(currentProgress.inQueue <= progressAfterInit.inQueue);
            progressAfterInit = currentProgress;

            if (currentProgress.inQueue == 0)
            {
                auto r0 = dlcManager.RequestPack("0");
                TEST_VERIFY(r0->IsDownloaded());
                auto r1 = dlcManager.RequestPack("1");
                TEST_VERIFY(r1->IsDownloaded());
                auto r2 = dlcManager.RequestPack("2");
                TEST_VERIFY(r2->IsDownloaded());
                auto r3 = dlcManager.RequestPack("3");
                TEST_VERIFY(r3->IsDownloaded());

                // now stop server for next tests
                dlcManager.Deinitialize();
                DAVA::StopEmbeddedWebServer();
                return true;
            }
        }
        break;
        }

        if (time > 30.0f) // timeout
        {
            TEST_VERIFY(false && "time out wait second connection")
            return true;
        }

        return false;
    }
};

DAVA_TESTCLASS (DLCManagerFullTest)
{
    FSMTest02 fsm02;
    bool TestAfterInitStopServer02_done = false;

    bool TestComplete(const DAVA::String& testName) const override
    {
        if (testName == "TestAfterInitStopServer02")
        {
            return TestAfterInitStopServer02_done;
        }
        return true;
    }

    void Update(DAVA::float32 timeElapsed, const DAVA::String& testName) override
    {
        if (testName == "TestAfterInitStopServer02")
        {
            TestAfterInitStopServer02_done = fsm02.Update(timeElapsed);
        }
    }

    DAVA_TEST (TestInitializeBadFolder01)
    {
        using namespace DAVA;

        DLCManager& dlcManager = *GetEngineContext()->dlcManager;

        bool getException = false;

        try
        {
            dlcManager.Initialize("C:/Windows/", "http://127.0.0.1:8080/superpack_for_unittests.dvpk", DLCManager::Hints());
        }
        catch (Exception& ex)
        {
            Logger::Info("get known exception: %s", ex.what());
            getException = true;
        }

        TEST_VERIFY(getException && "can't write or no such folder exception missing");

        dlcManager.Deinitialize();
    }

    DAVA_TEST (TestAfterInitStopServer02)
    {
        using namespace DAVA;

        DLCManager& dlcManager = *GetEngineContext()->dlcManager;

        const DLCManager::IRequest* r = dlcManager.RequestPack("1"); // pack "1" have one dependent pack "0"
        TEST_VERIFY(r != nullptr);

        if (!StartEmbeddedWebServer(documentRootDir.GetAbsolutePathname().c_str(), localPort))
        {
            TEST_VERIFY(false && "can't start embedded web server");
            return;
        }

        auto hints = DLCManager::Hints();
        hints.retryConnectMilliseconds = 3000;

        FilePath packDir("~doc:/UnitTests/DLCManagerTest/packs/");
        FileSystem::Instance()->DeleteDirectory(packDir, true);

        dlcManager.Initialize(packDir,
                              "http://127.0.0.1:8080/superpack_for_unittests.dvpk",
                              hints);

        auto request = dlcManager.RequestPack("3"); // pack "3" depends on "0, 1, 2" packs
        TEST_VERIFY(request != nullptr);
    }

    DAVA_TEST (TestServerDownDuringDownload03)
    {
    }

    DAVA_TEST (TestAddRequestAfterDisableRequesting04)
    {
    }

    DAVA_TEST (TestContinueDownloadingAfterEnableRequesting05)
    {
    }
};

#endif // __DAVAENGINE_WIN_UAP__
