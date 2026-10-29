#include "Hub/PluginCatalog.h"

juce::ValueTree PluginCatalogItem::toValueTree() const
{
    juce::ValueTree tree ("plugin");
    tree.setProperty ("id", id, nullptr);
    tree.setProperty ("name", name, nullptr);
    tree.setProperty ("category", category, nullptr);
    tree.setProperty ("version", version, nullptr);
    tree.setProperty ("requiresLicense", requiresLicense, nullptr);
    tree.setProperty ("trialEnabled", trialEnabled, nullptr);
    tree.setProperty ("price", price, nullptr);
    tree.setProperty ("fileUrl", fileUrl, nullptr);
    tree.setProperty ("description", description, nullptr);
    return tree;
}

PluginCatalogItem PluginCatalogItem::fromValueTree (const juce::ValueTree& tree)
{
    PluginCatalogItem item;
    item.id = tree.getProperty ("id").toString();
    item.name = tree.getProperty ("name").toString();
    item.category = tree.getProperty ("category").toString();
    item.version = tree.getProperty ("version").toString();
    item.requiresLicense = tree.getProperty ("requiresLicense");
    item.trialEnabled = tree.getProperty ("trialEnabled");
    item.price = static_cast<double> (tree.getProperty ("price"));
    item.fileUrl = tree.getProperty ("fileUrl").toString();
    item.description = tree.getProperty ("description").toString();
    return item;
}

juce::ValueTree SoundPackItem::toValueTree() const
{
    juce::ValueTree tree ("soundPack");
    tree.setProperty ("id", id, nullptr);
    tree.setProperty ("name", name, nullptr);
    tree.setProperty ("category", category, nullptr);
    tree.setProperty ("price", price, nullptr);
    tree.setProperty ("fileUrl", fileUrl, nullptr);
    tree.setProperty ("description", description, nullptr);
    return tree;
}

SoundPackItem SoundPackItem::fromValueTree (const juce::ValueTree& tree)
{
    SoundPackItem item;
    item.id = tree.getProperty ("id").toString();
    item.name = tree.getProperty ("name").toString();
    item.category = tree.getProperty ("category").toString();
    item.price = static_cast<double> (tree.getProperty ("price"));
    item.fileUrl = tree.getProperty ("fileUrl").toString();
    item.description = tree.getProperty ("description").toString();
    return item;
}

void PluginHub::addPlugin (const PluginCatalogItem& plugin)
{
    plugins_.add (plugin);
}

void PluginHub::addSoundPack (const SoundPackItem& pack)
{
    soundPacks_.add (pack);
}

juce::Array<PluginCatalogItem> PluginHub::getPlugins() const
{
    return plugins_;
}

juce::Array<SoundPackItem> PluginHub::getSoundPacks() const
{
    return soundPacks_;
}

bool PluginHub::canAccessPlugin (const PluginCatalogItem& plugin,
                                const LicenseManager& manager) const
{
    if (!plugin.requiresLicense)
        return true;

    if (plugin.trialEnabled && !manager.isTrialExpired())
        return true;

    return manager.isUnlocked();
}

bool PluginHub::canDownloadPack (const SoundPackItem& pack,
                                const LicenseManager& manager) const
{
    juce::ignoreUnused (pack);
    return manager.isUnlocked();
}

juce::ValueTree PluginHub::toValueTree() const
{
    juce::ValueTree tree ("pluginHub");

    for (const auto& plugin : plugins_)
        tree.appendChild (plugin.toValueTree(), nullptr);

    for (const auto& pack : soundPacks_)
        tree.appendChild (pack.toValueTree(), nullptr);

    return tree;
}

PluginHub PluginHub::fromValueTree (const juce::ValueTree& tree)
{
    PluginHub hub;

    for (const auto& child : tree)
    {
        if (child.hasType ("plugin"))
            hub.addPlugin (PluginCatalogItem::fromValueTree (child));
        else if (child.hasType ("soundPack"))
            hub.addSoundPack (SoundPackItem::fromValueTree (child));
    }

    return hub;
}
