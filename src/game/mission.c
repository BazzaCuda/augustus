#include "mission.h"

#include "campaign/campaign.h"
#include "scenario/property.h"

static const struct {
    int peaceful;
    int military;
} MISSION_IDS[12] = {
    {0, 0},
    {1, 1},
    {2, 3},
    {4, 5},
    {6, 7},
    {8, 9},
    {10, 11},
    {12, 13},
    {14, 15},
    {16, 17},
    {18, 19},
    {20, 21},
};

static const int RANK_CHOICE[] = {
    0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
};

int game_mission_peaceful(void)
{
    return MISSION_IDS[scenario_campaign_rank()].peaceful;
}

int game_mission_military(void)
{
    return MISSION_IDS[scenario_campaign_rank()].military;
}

int game_mission_has_choice(void)
{
    if (!campaign_is_active()) {
        return RANK_CHOICE[scenario_campaign_rank()];
    } else {
        int mission = scenario_campaign_mission();
        const campaign_mission_info *info = campaign_get_current_mission(mission);
        return info && info->total_scenarios > 1;
    }
}
