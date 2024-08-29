#include "main.hpp"

#include "custom-types/shared/register.hpp"

#include "scotland2/shared/modloader.h"

#include "GlobalNamespace/ResultsViewController.hpp"
#include "GlobalNamespace/LevelCompletionResults.hpp"
#include "HMUI/ImageView.hpp"
#include "TMPro/TextMeshProUGUI.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Transform.hpp"

using namespace GlobalNamespace;
using namespace TMPro;


void DidActivate(HMUI::ViewController* self, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling){
    if(firstActivation){
        // Make Touchable
        self->get_gameObject()->AddComponent<HMUI::Touchable*>();

        // Create Container
        auto* container = BSML::Lite::CreateScrollableSettingsContainer(self->get_transform());
        
        // Add Options  
        AddConfigValueInputString(container->get_transform(), getModConfig().FullComboText);
    }
}


static void setFullComboUI(UnityEngine::GameObject *clearedBannerGo)
{
    try
    {
        std::string userId = getModConfig().FullComboText.GetValue();
        clearedBannerGo->GetComponentsInChildren<CurvedTextMeshPro *>()[0]->set_text(StringW(userId));
    }
    catch (const std::exception &e)
    {
        
    }

    try
    {
        clearedBannerGo->GetComponentsInChildren<HMUI::ImageView *>()[0]->set_color(UnityEngine::Color::get_yellow());
    }
    catch (const std::exception &e)
    {
        
    }
}

static void setNotFullComboUI(UnityEngine::GameObject *clearedBannerGo)
{
    try
    {
        const auto originColor = UnityEngine::Color(1, 0.374, 0, 1);
        clearedBannerGo->GetComponentsInChildren<HMUI::ImageView *>()[0]->set_color(originColor);
    }
    catch (const std::exception &e)
    {
        
    }
}

MAKE_HOOK_MATCH(Results, &ResultsViewController::SetDataToUI, void, ResultsViewController *self)
{
    Results(self);

    if (self->_levelCompletionResults->levelEndStateType == LevelCompletionResults::LevelEndStateType::Cleared)
    {
        if (self->_levelCompletionResults->fullCombo)
        {
            self->_newHighScore = true;
            setFullComboUI(self->_clearedBannerGo);
        } else {
            setNotFullComboUI(self->_clearedBannerGo);
        }
    }
}




#pragma region Mod setup
/// @brief Called at the early stages of game loading
/// @param info
/// @return
MOD_EXPORT_FUNC void setup(CModInfo& info) {
    info.id = MOD_ID;
    info.version = VERSION;

    Logger.info("Completed setup!");
}

/// @brief Called later on in the game loading - a good time to install function hooks
/// @return
MOD_EXPORT_FUNC void late_load() {
    il2cpp_functions::Init();

    getModConfig().Init(modInfo);
    BSML::Init();

    BSML::Register::RegisterSettingsMenu("FullComboEffects", DidActivate, true);
    Logger.info("Installing hooks...");

    INSTALL_HOOK(Logger, Results);

    Logger.info("Installed all hooks!");
}
#pragma endregion


