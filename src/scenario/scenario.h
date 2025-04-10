#ifndef SCENARIO_SCENARIO_H
#define SCENARIO_SCENARIO_H

#include "core/buffer.h"
#include "scenario/data.h"

void scenario_settings_init(void);

void scenario_settings_init_mission(void);
void scenario_settings_init_favor(void);

void scenario_unlock_all_buildings(void);

int scenario_get_state_buffer_size_by_savegame_version(int savegame_version_t);
int scenario_get_state_buffer_size_by_scenario_version(int scenario_version_t);
void scenario_save_state(buffer *buf);
void scenario_load_state(buffer *buf, int version);

void scenario_description_from_buffer(buffer *buf, uint8_t *description, int version);
int scenario_climate_from_buffer(buffer *buf, int version);
int scenario_image_id_from_buffer(buffer *buf, int version);
int scenario_invasions_from_buffer(buffer *buf, int version);
int scenario_rank_from_buffer(buffer *buf, int version);
int scenario_start_year_from_buffer(buffer *buf, int version);
void scenario_open_play_info_from_buffer(buffer *buf, int version, int *is_open_play, int *open_play_id);
void scenario_objectives_from_buffer(buffer *buf, int version, scenario_win_criteria *win_criteria);
void scenario_map_data_from_buffer(buffer *buf, int *width, int *height, int *grid_start, int *grid_border_size, int version);

void scenario_settings_save_state(
    buffer *part1, buffer *part2, buffer *part3, buffer *player_name, buffer *scenario_name, buffer *campaign_name);

void scenario_settings_load_state(
    buffer *part1, buffer *part2, buffer *part3, buffer *player_name, buffer *scenario_name, buffer *campaign_name);


#endif // SCENARIO_SCENARIO_H
