#pragma once

#include <juce_core/juce_core.h>
#include <juce_events/juce_events.h>

class LicenseManager
{
public:
    static LicenseManager& getInstance()
    {
        static LicenseManager instance;
        return instance;
    }

    void initialise()
    {
        const auto appData = juce::File::getSpecialLocation (juce::File::userApplicationDataDirectory)
                                 .getChildFile ("WaveyBeat")
                                 .getChildFile ("StickTone");

        appData.createDirectories();

        const auto settingsFile = appData.getChildFile ("license-settings.properties");
        settings_ = std::make_unique<juce::PropertiesFile> (
            settingsFile,
            juce::PropertiesFile::Options().withTargetFile (settingsFile).withoutStatusText());

        if (settings_->getValue ("licenseCode").isEmpty())
            settings_->setValue ("licenseCode", "");

        if (settings_->getValue ("trialStartedAt").isEmpty())
            settings_->setValue ("trialStartedAt", juce::String (juce::Time::getMillisecondCounterHiRes()));

        if (settings_->getValue ("trialLaunches").isEmpty())
            settings_->setValue ("trialLaunches", "0");

        settings_->save();
    }

    bool isUnlocked() const
    {
        const auto code = getStoredLicenseCode();
        return isValidLicenseCode (code);
    }

    bool isTrialExpired() const
    {
        if (isUnlocked())
            return false;

        const auto startedAtMs = settings_ != nullptr ? settings_->getValue ("trialStartedAt").getLargeIntValue() : 0;
        const auto nowMs = juce::Time::getMillisecondCounterHiRes();
        const auto launches = settings_ != nullptr ? settings_->getValue ("trialLaunches").getIntValue() : 0;

        const double trialDurationMs = 14.0 * 24.0 * 60.0 * 60.0 * 1000.0;
        const int trialLaunchLimit = 20;

        return (nowMs - startedAtMs > trialDurationMs) || (launches >= trialLaunchLimit);
    }

    void recordLaunch()
    {
        if (isUnlocked())
            return;

        if (settings_ == nullptr)
            initialise();

        int launches = settings_->getValue ("trialLaunches").getIntValue();
        settings_->setValue ("trialLaunches", juce::String (launches + 1));
        settings_->save();
    }

    juce::String getStoredLicenseCode() const
    {
        return settings_ != nullptr ? settings_->getValue ("licenseCode") : juce::String();
    }

    void setLicenseCode (const juce::String& code)
    {
        if (settings_ == nullptr)
            initialise();

        settings_->setValue ("licenseCode", code);
        settings_->save();
    }

    static bool isValidLicenseCode (const juce::String& code)
    {
        return code.trim().equalsIgnoreCase ("WAVYBEAT-STICKTONE-UNLOCK");
    }

private:
    std::unique_ptr<juce::PropertiesFile> settings_;
};
