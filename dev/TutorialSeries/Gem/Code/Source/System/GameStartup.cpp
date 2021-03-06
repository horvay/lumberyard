
#include "StdAfx.h"
#include "GameStartup.h"
#include "Core/TutorialSeriesGame.h"
#include "Core/EditorGame.h"
#include "IHardwareMouse.h"
#include <IPlayerProfiles.h>
#include <CryLibrary.h>
#include <IPlatformOS.h>

using namespace TutorialSeries;

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

GameStartup::GameStartup()
    : m_Framework(nullptr)
    , m_Game(nullptr)
{
}

GameStartup::~GameStartup()
{
    Shutdown();
}

IGameRef GameStartup::Init(SSystemInitParams& startupParams)
{
    IGameRef gameRef = nullptr;
    startupParams.pGameStartup = this;

    CryGameFrameworkBus::BroadcastResult(m_Framework, &CryGameFrameworkRequests::InitFramework, startupParams);
    if (m_Framework)
    {
        ISystem* system = m_Framework->GetISystem();
        startupParams.pSystem = system;

        // register system listeners
        system->GetISystemEventDispatcher()->RegisterListener(this);

        IEntitySystem* entitySystem = system->GetIEntitySystem();
        IComponentFactoryRegistry::RegisterAllComponentFactoryNodes(*entitySystem->GetComponentFactoryRegistry());

        gameRef = Reset();

        if (m_Framework->CompleteInit())
        {
            ISystemEventDispatcher* eventDispatcher = system->GetISystemEventDispatcher();
            eventDispatcher->OnSystemEvent(
                ESYSTEM_EVENT_RANDOM_SEED,
                static_cast<UINT_PTR>(gEnv->pTimer->GetAsyncTime().GetMicroSecondsAsInt64()),
                0
                );

            if (startupParams.bExecuteCommandLine)
            {
                system->ExecuteCommandLine();
            }

            system->GetPlatformOS()->UserDoSignIn(0);
        }
        else
        {
            gameRef->Shutdown();
            gameRef = nullptr;
        }
    }

    return gameRef;
}

IGameRef GameStartup::Reset()
{
    static char gameBuffer[sizeof(TutorialSeries::TutorialSeriesGame)];

    ISystem* system = m_Framework->GetISystem();
    ModuleInitISystem(system, "TutorialSeries");

    m_Game = new(reinterpret_cast<void*>(gameBuffer)) TutorialSeries::TutorialSeriesGame();
    const bool initialized = (m_Game && m_Game->Init(m_Framework));

    return initialized ? &m_Game : nullptr;
}

void GameStartup::Shutdown()
{
    if (m_Game)
    {
        m_Game->Shutdown();
        m_Game = nullptr;
    }
    CryGameFrameworkBus::Broadcast(&CryGameFrameworkRequests::ShutdownFramework);
}

int GameStartup::Update(bool haveFocus, unsigned int updateFlags)
{
    return (m_Game ? m_Game->Update(haveFocus, updateFlags) : 0);
}

int GameStartup::Run(const char* autoStartLevelName)
{
    gEnv->pConsole->ExecuteString("exec autoexec.cfg");
    gEnv->pConsole->ExecuteString("map TutorialLevel");
#ifdef WIN32
    while (true)
    {
        ISystem* pSystem = gEnv ? gEnv->pSystem : nullptr;
        if (!pSystem)
        {
            break;
        }

        if (pSystem->PumpWindowMessage(false) == -1)
        {
            break;
        }

        if (!Update(true, 0))
        {
            // need to clean the message loop (WM_QUIT might cause problems in the case of a restart)
            // another message loop might have WM_QUIT already so we cannot rely only on this
            pSystem->PumpWindowMessage(true);
            break;
        }
    }
#else
    while (true)
    {
        if (!Update(true, 0))
        {
            break;
        }
    }
#endif // WIN32

    return 0;
}

void GameStartup::OnSystemEvent(ESystemEvent event, UINT_PTR wparam, UINT_PTR lparam)
{
    switch (event)
    {
    case ESYSTEM_EVENT_RANDOM_SEED:
        cry_random_seed(gEnv->bNoRandomSeed ? 0 : (uint32)wparam);
        break;

    case ESYSTEM_EVENT_CHANGE_FOCUS:
        // 3.8.1 - disable / enable Sticky Keys!
        GetISystem()->GetISystemEventDispatcher()->OnSystemEvent(ESYSTEM_EVENT_RANDOM_SEED, (UINT_PTR)gEnv->pTimer->GetAsyncTime().GetMicroSecondsAsInt64(), 0);
        break;

    case ESYSTEM_EVENT_LEVEL_POST_UNLOAD:
        STLALLOCATOR_CLEANUP;
        break;

    case ESYSTEM_EVENT_LEVEL_LOAD_START:
    case ESYSTEM_EVENT_FAST_SHUTDOWN:
    default:
        break;
    }
}


//////////////////////////////////////////////////////////////////////////
#ifdef WIN32

bool GameStartup::HandleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
    switch (msg)
    {
    case WM_SYSCHAR: // Prevent ALT + key combinations from creating 'ding' sounds
    {
        const bool bAlt = (lParam & (1 << 29)) != 0;
        if (bAlt && wParam == VK_F4)
        {
            return false;     // Pass though ALT+F4
        }

        *pResult = 0;
        return true;
    }
    break;

    case WM_SIZE:
        break;

    case WM_SETFOCUS:
        // 3.8.1 - set a hasWindowFocus CVar to true
        break;

    case WM_KILLFOCUS:
        // 3.8.1 - set a hasWindowFocus CVar to false
        break;

    case WM_SETCURSOR:
    {
        // This is sample code to change the displayed cursor for Windows applications.
        // Note that this sample loads a texture (ie, .TIF or .DDS), not a .ICO or .CUR resource.
        const char* const cDefaultCursor = "EngineAssets/Textures/Cursor_Green.tif";
        IHardwareMouse* const pMouse = gEnv ? gEnv->pHardwareMouse : nullptr;
        assert(pMouse && "HWMouse should be initialized before window is shown, check engine initialization order");
        const bool bResult = pMouse ? pMouse->SetCursor(cDefaultCursor) : false;
        if (!bResult)
        {
            GameWarning("Unable to load cursor %s, does this file exist?", cDefaultCursor);
        }
        *pResult = bResult ? TRUE : FALSE;
        return bResult;
    }
    break;
    }
    return false;
}
#endif // WIN32
