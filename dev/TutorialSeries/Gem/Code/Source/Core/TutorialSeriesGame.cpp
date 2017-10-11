
#include "StdAfx.h"
#include "Game/Actor.h"
#include "TutorialSeriesGame.h"
#include "IGameFramework.h"
#include "IGameRulesSystem.h"
#include "TutorialSeriesGameRules.h"
#include "IPlatformOS.h"
#include <functional>

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

#define REGISTER_FACTORY(host, name, impl, isAI) \
    (host)->RegisterFactory((name), (impl*)0, (isAI), (impl*)0)

namespace TutorialSeries
{
    TutorialSeriesGame* g_Game = nullptr;

    //////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////

    TutorialSeriesGame::TutorialSeriesGame()
        : m_clientEntityId(INVALID_ENTITYID)
        , m_gameRules(nullptr)
        , m_gameFramework(nullptr)
        , m_defaultActionMap(nullptr)
        , m_platformInfo()
    {
        g_Game = this;
        GetISystem()->SetIGame(this);
    }

    TutorialSeriesGame::~TutorialSeriesGame()
    {
        m_gameFramework->EndGameContext(false);

        // Remove self as listener.
        m_gameFramework->UnregisterListener(this);
        m_gameFramework->GetILevelSystem()->RemoveListener(this);
        gEnv->pSystem->GetISystemEventDispatcher()->RemoveListener(this);

        g_Game = nullptr;
        GetISystem()->SetIGame(nullptr);

        ReleaseActionMaps();
    }

    bool TutorialSeriesGame::Init(IGameFramework* framework)
    {
        m_gameFramework = framework;

        // Register the actor class so actors can spawn.
        // #TODO If you create a new actor, make sure to register a factory here.
        REGISTER_FACTORY(m_gameFramework, "Actor", Actor, false);

        // Listen to system events, so we know when levels load/unload, etc.
        gEnv->pSystem->GetISystemEventDispatcher()->RegisterListener(this);
        m_gameFramework->GetILevelSystem()->AddListener(this);

        // Listen for game framework events (level loaded/unloaded, etc.).
        m_gameFramework->RegisterListener(this, "Game", FRAMEWORKLISTENERPRIORITY_GAME);

        // Load actions maps.
        LoadActionMaps("config/input/actionmaps.xml");

        // Register game rules wrapper.
        REGISTER_FACTORY(framework, "TutorialSeriesGameRules", TutorialSeriesGameRules, false);
        IGameRulesSystem* pGameRulesSystem = g_Game->GetIGameFramework()->GetIGameRulesSystem();
        pGameRulesSystem->RegisterGameRules("DummyRules", "TutorialSeriesGameRules");

        GetISystem()->GetPlatformOS()->UserDoSignIn(0);

        return true;
    }

    bool TutorialSeriesGame::CompleteInit()
    {
        return true;
    }

    int TutorialSeriesGame::Update(bool hasFocus, unsigned int updateFlags)
    {
        const float frameTime = gEnv->pTimer->GetFrameTime();

        const bool continueRunning = m_gameFramework->PreUpdate(true, updateFlags);
        m_gameFramework->PostUpdate(true, updateFlags);

        return static_cast<int>(continueRunning);
    }

    void TutorialSeriesGame::PlayerIdSet(EntityId playerId)
    {
        m_clientEntityId = playerId;
    }

    void TutorialSeriesGame::Shutdown()
    {
        this->~TutorialSeriesGame();
    }

    void TutorialSeriesGame::LoadActionMaps(const char* fileName)
    {
        if (g_Game->GetIGameFramework()->IsGameStarted())
        {
            CryLogAlways("[Profile] Can't change configuration while game is running (yet)");
            return;
        }

        XmlNodeRef rootNode = m_gameFramework->GetISystem()->LoadXmlFromFile(fileName);
        if (rootNode && ReadProfile(rootNode))
        {
            IActionMapManager* actionMapManager = m_gameFramework->GetIActionMapManager();
            actionMapManager->SetLoadFromXMLPath(fileName);
            m_defaultActionMap = actionMapManager->GetActionMap("default");
        }
        else
        {
            CryLogAlways("[Profile] Warning: Could not open configuration file");
        }
    }

    void TutorialSeriesGame::ReleaseActionMaps()
    {
        if (m_defaultActionMap && m_gameFramework)
        {
            IActionMapManager* actionMapManager = m_gameFramework->GetIActionMapManager();
            actionMapManager->RemoveActionMap(m_defaultActionMap->GetName());
            m_defaultActionMap = nullptr;
        }
    }

    bool TutorialSeriesGame::ReadProfile(const XmlNodeRef& rootNode)
    {
        bool successful = false;

        if (IActionMapManager* actionMapManager = m_gameFramework->GetIActionMapManager())
        {
            actionMapManager->Clear();

            // Load platform information in.
            XmlNodeRef platforms = rootNode->findChild("platforms");
            if (!platforms || !ReadProfilePlatform(platforms, GetPlatform()))
            {
                CryLogAlways("[Profile] Warning: No platform information specified!");
            }

            successful = actionMapManager->LoadFromXML(rootNode);
        }

        return successful;
    }

    bool TutorialSeriesGame::ReadProfilePlatform(const XmlNodeRef& platformsNode, TutorialSeries::Platform platformId)
    {
        bool successful = false;

        if (platformsNode && (platformId > ePlatform_Unknown) && (platformId < ePlatform_Count))
        {
            if (XmlNodeRef platform = platformsNode->findChild(s_PlatformNames[platformId]))
            {
                // Extract which Devices we want.
                if (!strcmp(platform->getAttr("keyboard"), "0"))
                {
                    m_platformInfo.m_devices &= ~eAID_KeyboardMouse;
                }

                if (!strcmp(platform->getAttr("xboxpad"), "0"))
                {
                    m_platformInfo.m_devices &= ~eAID_XboxPad;
                }

                if (!strcmp(platform->getAttr("ps4pad"), "0"))
                {
                    m_platformInfo.m_devices &= ~eAID_PS4Pad;
                }

                if (!strcmp(platform->getAttr("androidkey"), "0"))
                {
                    m_platformInfo.m_devices &= ~eAID_AndroidKey;
                }

                // Map the Devices we want.
                IActionMapManager* actionMapManager = m_gameFramework->GetIActionMapManager();

                if (m_platformInfo.m_devices & eAID_KeyboardMouse)
                {
                    actionMapManager->AddInputDeviceMapping(eAID_KeyboardMouse, "keyboard");
                }

                if (m_platformInfo.m_devices & eAID_XboxPad)
                {
                    actionMapManager->AddInputDeviceMapping(eAID_XboxPad, "xboxpad");
                }

                if (m_platformInfo.m_devices & eAID_PS4Pad)
                {
                    actionMapManager->AddInputDeviceMapping(eAID_PS4Pad, "ps4pad");
                }

                if (m_platformInfo.m_devices & eAID_AndroidKey)
                {
                    actionMapManager->AddInputDeviceMapping(eAID_AndroidKey, "androidkey");
                }

                successful = true;
            }
            else
            {
                GameWarning("TutorialSeriesGame::ReadProfilePlatform: Failed to find platform, action mappings loading will fail");
            }
        }

        return successful;
    }

    TutorialSeries::Platform TutorialSeriesGame::GetPlatform() const
    {
        TutorialSeries::Platform platform = ePlatform_Unknown;

    #if defined(ANDROID)
        platform = ePlatform_Android;
    #elif defined(IOS)
        platform = ePlatform_iOS;
    #elif defined(WIN32) || defined(WIN64) || defined(DURANGO) || defined(APPLE) || defined(LINUX)
        platform = ePlatform_PC;
    #endif

        return platform;
    }

    void TutorialSeriesGame::OnActionEvent(const SActionEvent& event)
    {
        switch (event.m_event)
        {
        case eAE_unloadLevel:
            /*!
             * #TODO
             * Add clean up code here.
             */
            break;
        }
    }

} // namespace TutorialSeries
