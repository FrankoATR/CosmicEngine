#ifndef PUZZLERPG_MENU_MUSIC_HPP
#define PUZZLERPG_MENU_MUSIC_HPP

#include "puzzle_asset_defines.hpp"

#include <CosmicEngine/interfaces/definitions.hpp>
#include AUDIOMANAGER_HEADER

namespace PuzzleRPG
{
    inline bool &MenuMusicActiveFlag()
    {
        static bool active = false;
        return active;
    }

    inline unsigned int &MenuMusicResumeMs()
    {
        static unsigned int milliseconds = 0;
        return milliseconds;
    }

    inline void PreserveMenuMusicForSceneChange()
    {
        if (!MenuMusicActiveFlag()) return;
        MenuMusicResumeMs() = AUD_MN.GetPosition(PUZZLERPG_MUSIC_MENU_KEY);
        MenuMusicActiveFlag() = false;
    }

    inline void StartMenuMusic()
    {
        AUD_MN.Load(PUZZLERPG_MUSIC_MENU_KEY, PUZZLERPG_MUSIC_MENU_PATH, CosmicEngine::SoundType::Music);
        if (MenuMusicResumeMs() > 0)
        {
            AUD_MN.SetPosition(PUZZLERPG_MUSIC_MENU_KEY, MenuMusicResumeMs());
        }
        AUD_MN.Play(PUZZLERPG_MUSIC_MENU_KEY, 1.0f, true, false);
        MenuMusicActiveFlag() = true;
    }

    inline void StopMenuMusic(bool resetPosition = true)
    {
        AUD_MN.Stop(PUZZLERPG_MUSIC_MENU_KEY);
        MenuMusicActiveFlag() = false;
        if (resetPosition) MenuMusicResumeMs() = 0;
    }
}

#endif
