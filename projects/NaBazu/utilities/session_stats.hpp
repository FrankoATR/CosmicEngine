#ifndef NABAZU_SESSION_STATS_HPP
#define NABAZU_SESSION_STATS_HPP

#include <algorithm>

namespace NaBazu
{
    // Records for the current process run, shared between the menu (shows the record
    // before you start) and the game over screen (compares your run against it).
    // In-memory only: persisting to disk via JsonManager stays a future improvement.
    struct SessionStats
    {
        int highScore = 0;
        int bestDistance = 0;
        int bestKills = 0;
        int runsPlayed = 0;
    };

    inline SessionStats &GetSessionStats()
    {
        static SessionStats stats;
        return stats;
    }

    inline int GetSessionHighScore()
    {
        return GetSessionStats().highScore;
    }

    // Returns true when this run beat the previous best score.
    inline bool RecordRun(int score, int distance, int kills)
    {
        SessionStats &stats = GetSessionStats();
        ++stats.runsPlayed;
        stats.bestDistance = std::max(stats.bestDistance, distance);
        stats.bestKills = std::max(stats.bestKills, kills);

        if (score > stats.highScore)
        {
            stats.highScore = score;
            return true;
        }
        return false;
    }
}

#endif
