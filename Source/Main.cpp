#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_gui_extra/juce_gui_extra.h>

#include "Hub/PluginCatalog.h"
#include "License/LicenseManager.h"

namespace
{
    class LicenseGateComponent final : public juce::Component
    {
    public:
        LicenseGateComponent (std::function<void()> onUnlocked)
            : onUnlocked_ (std::move (onUnlocked))
        {
            setSize (420, 220);

            title_.setText ("StickTone Trial", juce::dontSendNotification);
            title_.setJustificationType (juce::Justification::centredLeft);
            title_.setFont (juce::Font (22.0f, juce::Font::bold));
            addAndMakeVisible (title_);

            message_.setText ("Your trial is active. Enter the unlock code to access the full version, plugin hub, and premium packs.", juce::dontSendNotification);
            message_.setJustificationType (juce::Justification::centredLeft);
            message_.setMinimumHorizontalScale (1.0f);
            addAndMakeVisible (message_);

            codeEditor_.setText ("", false);
            codeEditor_.setTextToShowWhenEmpty ("Enter license code", juce::Colours::grey);
            addAndMakeVisible (codeEditor_);

            unlockButton_.setButtonText ("Unlock");
            unlockButton_.onClick = [this]
            {
                const auto code = codeEditor_.getText().trim();

                if (LicenseManager::isValidLicenseCode (code))
                {
                    LicenseManager::getInstance().setLicenseCode (code);
                    status_.setText ("Unlocked. Thank you for supporting StickTone.", juce::dontSendNotification);
                    status_.setColour (juce::Label::textColourId, juce::Colours::darkgreen);
                    onUnlocked_();
                }
                else
                {
                    status_.setText ("Invalid code. Please try again.", juce::dontSendNotification);
                    status_.setColour (juce::Label::textColourId, juce::Colours::crimson);
                }
            };
            addAndMakeVisible (unlockButton_);

            status_.setText ("Trial mode remains active until a valid license is entered.", juce::dontSendNotification);
            status_.setColour (juce::Label::textColourId, juce::Colours::darkgrey);
            addAndMakeVisible (status_);
        }

        void resized() override
        {
            auto r = getLocalBounds().reduced (18);
            title_.setBounds (r.removeFromTop (32));
            message_.setBounds (r.removeFromTop (52));
            codeEditor_.setBounds (r.removeFromTop (34));
            unlockButton_.setBounds (r.removeFromTop (30).withTrimmedLeft (250));
            status_.setBounds (r.removeFromTop (26));
        }

    private:
        juce::Label title_;
        juce::Label message_;
        juce::TextEditor codeEditor_;
        juce::TextButton unlockButton_ { "Unlock" };
        juce::Label status_;
        std::function<void()> onUnlocked_;
    };

    class PluginHubView final : public juce::Component
    {
    public:
        explicit PluginHubView (const PluginHub& hub)
            : hub_ (hub)
        {
            setSize (700, 500);

            title_.setText ("Plugin and Pack Hub", juce::dontSendNotification);
            title_.setFont (juce::Font (24.0f, juce::Font::bold));
            addAndMakeVisible (title_);

            auto plugins = hub_.getPlugins();
            for (const auto& plugin : plugins)
            {
                auto* label = new juce::Label();
                const auto access = hub_.canAccessPlugin (plugin, LicenseManager::getInstance()) ? "Access: OPEN" : "Access: LOCKED";
                label->setText (plugin.name + " | " + plugin.category + " | " + access, juce::dontSendNotification);
                label->setColour (juce::Label::textColourId, juce::Colours::black);
                addAndMakeVisible (label);
                labels_.add (label);
            }

            auto packs = hub_.getSoundPacks();
            if (!packs.isEmpty())
            {
                auto* packTitle = new juce::Label();
                packTitle->setText ("Sound Packs", juce::dontSendNotification);
                packTitle->setFont (juce::Font (18.0f, juce::Font::bold));
                addAndMakeVisible (packTitle);
                packTitles_.add (packTitle);
            }

            for (const auto& pack : packs)
            {
                auto* label = new juce::Label();
                const auto access = hub_.canDownloadPack (pack, LicenseManager::getInstance()) ? "Download: OPEN" : "Download: LOCKED";
                label->setText (pack.name + " | " + pack.category + " | " + access, juce::dontSendNotification);
                label->setColour (juce::Label::textColourId, juce::Colours::black);
                addAndMakeVisible (label);
                packLabels_.add (label);
            }
        }

        void resized() override
        {
            auto r = getLocalBounds().reduced (20);
            title_.setBounds (r.removeFromTop (34));

            int y = r.getY();
            for (auto* label : labels_)
            {
                label->setBounds (r.getX(), y, r.getWidth(), 24);
                y += 28;
            }

            y += 12;
            for (auto* title : packTitles_)
            {
                title->setBounds (r.getX(), y, r.getWidth(), 24);
                y += 28;
            }

            for (auto* label : packLabels_)
            {
                label->setBounds (r.getX(), y, r.getWidth(), 24);
                y += 28;
            }
        }

    private:
        PluginHub hub_;
        juce::Label title_;
        juce::OwnedArray<juce::Label> labels_;
        juce::OwnedArray<juce::Label> packTitles_;
        juce::OwnedArray<juce::Label> packLabels_;
    };

    class StickToneMainComponent final : public juce::Component
    {
    public:
        StickToneMainComponent()
        {
            setSize (760, 520);

            PluginCatalogItem corePlugin;
            corePlugin.id = "core-kick";
            corePlugin.name = "Core Kick Kit";
            corePlugin.category = "Drums";
            corePlugin.version = "1.0.0";
            corePlugin.requiresLicense = false;
            corePlugin.trialEnabled = true;
            corePlugin.description = "Starter drum kits and core kick presets";

            PluginCatalogItem premiumPlugin;
            premiumPlugin.id = "premium-bass";
            premiumPlugin.name = "Premium Bass Engine";
            premiumPlugin.category = "Synth";
            premiumPlugin.version = "2.1.0";
            premiumPlugin.requiresLicense = true;
            premiumPlugin.trialEnabled = true;
            premiumPlugin.description = "Premium synth engine for advanced drum and bass shaping";

            SoundPackItem starterPack;
            starterPack.id = "starter-sounds";
            starterPack.name = "Starter Sound Pack";
            starterPack.category = "Drums";
            starterPack.description = "A selection of starter drum samples";

            SoundPackItem premiumPack;
            premiumPack.id = "premium-sounds";
            premiumPack.name = "Premium Sound Pack";
            premiumPack.category = "Drums";
            premiumPack.description = "Expanded premium drum library";

            hub_.addPlugin (corePlugin);
            hub_.addPlugin (premiumPlugin);
            hub_.addSoundPack (starterPack);
            hub_.addSoundPack (premiumPack);

            title_.setText ("STICKTONE BY WAVEY BEAT", juce::dontSendNotification);
            title_.setFont (juce::Font (28.0f, juce::Font::bold));
            addAndMakeVisible (title_);

            status_.setText (LicenseManager::getInstance().isUnlocked() ? "Full version unlocked" : "Trial mode active", juce::dontSendNotification);
            status_.setColour (juce::Label::textColourId, LicenseManager::getInstance().isUnlocked() ? juce::Colours::darkgreen : juce::Colours::darkorange);
            addAndMakeVisible (status_);

            auto* pointer = new PluginHubView (hub_);
            pluginHubView_ = pointer;
            addAndMakeVisible (pluginHubView_);
        }

        void paint (juce::Graphics& g) override
        {
            g.fillAll (juce::Colours::black);
        }

        void resized() override
        {
            auto r = getLocalBounds().reduced (16);
            title_.setBounds (r.removeFromTop (48));
            status_.setBounds (r.removeFromTop (24));
            if (pluginHubView_ != nullptr)
                pluginHubView_->setBounds (r.reduced (8));
        }

    private:
        PluginHub hub_;
        juce::Label title_;
        juce::Label status_;
        juce::Component* pluginHubView_ = nullptr;
    };
}

class StickToneApplication final : public juce::JUCEApplication
{
public:
    StickToneApplication() = default;

    const juce::String getName() override { return "STICKTONE BY WAVEY BEAT"; }
    const juce::String getApplicationName() override { return getName(); }
    bool moreThanOneInstanceAllowed() override { return true; }

    void initialise (const juce::String&) override
    {
        LicenseManager::getInstance().initialise();
        LicenseManager::getInstance().recordLaunch();

        mainWindow_ = std::make_unique<juce::DocumentWindow> (
            "STICKTONE BY WAVEY BEAT",
            juce::Colours::black,
            juce::DocumentWindow::allButtons);

        mainWindow_->setUsingNativeTitleBar (true);

        if (LicenseManager::getInstance().isUnlocked() || !LicenseManager::getInstance().isTrialExpired())
        {
            mainWindow_->setContentOwned (new StickToneMainComponent(), true);
        }
        else
        {
            mainWindow_->setContentOwned (
                new LicenseGateComponent ([this]
                {
                    if (mainWindow_ != nullptr)
                        mainWindow_->setContentOwned (new StickToneMainComponent(), true);
                }), true);

            juce::AlertWindow::showMessageBox (
                juce::MessageBoxIconType::WarningIcon,
                "Trial expired",
                "Your trial is over. Enter a valid license code to access the full version and hub.");
        }

        mainWindow_->centreWithSize (760, 520);
        mainWindow_->setVisible (true);
    }

    void shutdown() override
    {
        mainWindow_ = nullptr;
    }

    void systemRequestedQuit() override
    {
        quit();
    }

    void anotherInstanceStarted (const juce::String&) override {}

private:
    std::unique_ptr<juce::DocumentWindow> mainWindow_;
};

START_JUCE_APPLICATION (StickToneApplication)
