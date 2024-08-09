#include "main.hpp"

#include "scotland2/shared/modloader.h"

#include "GlobalNamespace/ResultsViewController.hpp"
#include "GlobalNamespace/LevelCompletionResults.hpp"
#include "HMUI/ImageView.hpp"
#include "TMPro/TextMeshProUGUI.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Transform.hpp"

using namespace GlobalNamespace;
using namespace TMPro;

static modloader::ModInfo modInfo{MOD_ID, VERSION, 0};
// Stores the ID and version of our mod, and is sent to
// the modloader upon startup

// Loads the config from disk using our modInfo, then returns it for use
// other config tools such as config-utils don't use this config, so it can be
// removed if those are in use
Configuration &getConfig() {
  static Configuration config(modInfo);
  return config;
}


void DidActivate(HMUI::ViewController* self, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling){
    if(firstActivation){
        // Make Touchable
        self->get_gameObject()->AddComponent<HMUI::Touchable*>();

        // Create Container
        auto* container = BSML::Lite::CreateScrollableSettingsContainer(self->get_transform());
        
        // Add Options  
        AddConfigValueInputString(parent, getModConfig().FullComboText);
    }
}


static void setFullComboUI(UnityEngine::GameObject *clearedBannerGo)
{
    try
    {
        clearedBannerGo->GetComponentsInChildren<TextMeshProUGUI *>()[0]->set_text(il2cpp_utils::newcsstr(getModConfig().FullComboText);
    }
    catch (const std::exception &e)
    {
        getLogger().log(Logging::ERROR, "Ow! Failed to set text :/");
    }

    try
    {
        clearedBannerGo->GetComponentsInChildren<HMUI::ImageView *>()[0]->set_color(UnityEngine::Color::get_yellow());
    }
    catch (const std::exception &e)
    {
        getLogger().log(Logging::ERROR, "Ow! Failed to set color to background :(");
    }
}

static void setNotFullComboUI(UnityEngine::GameObject *clearedBannerGo)
{
    // no need to reset text to LEVEL CLEARED

    // reset background to red color
    try
    {
        const auto originColor = UnityEngine::Color(1, 0.374, 0, 1);
        clearedBannerGo->GetComponentsInChildren<HMUI::ImageView *>()[0]->set_color(originColor);
    }
    catch (const std::exception &e)
    {
        getLogger().log(Logging::ERROR, "Ow! Failed to set color to background :(");
    }
}

MAKE_HOOK_MATCH(Results, &ResultsViewController::SetDataToUI, void, ResultsViewController *self)
{
    Results(self);

    if (self->levelCompletionResults->levelEndStateType == LevelCompletionResults::LevelEndStateType::Cleared)
    {
        if (self->levelCompletionResults->fullCombo)
        {
            self->newHighScore = true;
            setFullComboUI(self->clearedBannerGo);
        } else {
            setNotFullComboUI(self->clearedBannerGo);
        }
    }
}

// Called at the early stages of game loading
MOD_EXTERN_FUNC void setup(CModInfo *info) noexcept {
  *info = modInfo.to_c();

  getConfig().Load();

  // File logging
  Paper::Logger::RegisterFileContextId(PaperLogger.tag);

  PaperLogger.info("Completed setup!");
}

// Called later on in the game loading - a good time to install function hooks
MOD_EXTERN_FUNC void late_load() noexcept {
  il2cpp_functions::Init();

  PaperLogger.info("Installing hooks...");
  INSTALL_HOOK(getLogger(), Results);
  PaperLogger.info("Installed all hooks!");
}