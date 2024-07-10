# Retro Installer

This is installer is inspired mostly by [qjimbo's legacy installer project](https://github.com/qjimbo/NMSLegacyVersionInstaller) but with extra patches and runs without goldberg so it isn't actually piracy.

For the record this isn't affiliated with the No Man's Sky Retro group I just couldn't think of a better title.

## What does it do?

You're provided 4 copies (the same as qjimbo's, which are the gold standard I assume) to install being the final patched versions of the Initial Release, Foundations, Path Finder and Atlas Rises builds.

The installer patches the save directories in each binary for custom save dirs (C:/Users/*/AppData/Roaming/HelloGames/1_**/..) and runs [Steamless](https://github.com/atom0s/Steamless) on the Path Finder and Atlas Rises versions of the executable.

## To Do

* Try fix Discovery server stuff, probably won't happen without any actual internal stuff because new Atlas auth broker stuff requires OAuth bearer from ITkUserService.
* Apply [Ethan's Shader Fixes](https://github.com/EthanRDoesMC/RetroShaderFix) automatically.
* Start menu shortcuts.
* Mod loading on versions that don't support it, also might mean a retro internal.
