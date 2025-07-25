#include "config.h"

#include "core/dir.h"
#include "core/file.h"
#include "core/log.h"

#include <stdio.h>
#include <string.h>

#define MAX_LINE 100

static const char *INI_FILENAME = "augustus.ini";
static int needs_user_directory_setup;

// Keep this in the same order as the config_keys in config.h
static const char *ini_keys[] = {
    "enable_audio",
    "master_volume",
    "enable_music_randomise",
    "enable_audio_in_videos",
    "video_volume",
    "next_autosave_slot",
    "has_set_user_directories",
    "gameplay_fix_immigration",
    "gameplay_fix_100y_ghosts",
    "screen_display_scale",
    "screen_cursor_scale",
    "screen_color_cursors",
    "ui_sidebar_info",
    "ui_show_intro_video",
    "ui_smooth_scrolling",
    "ui_disable_mouse_edge_scrolling",
    "ui_visual_feedback_on_delete",
    "ui_allow_cycling_temples",
    "ui_show_water_structure_range",
    "ui_show_water_structure_range_houses",
    "ui_show_market_range",
    "ui_show_construction_size",
    "ui_highlight_legions",
    "ui_show_military_sidebar",
    "ui_disable_map_drag",
    "ui_show_max_prosperity",
    "ui_digit_separator",
    "ui_inverse_map_drag",
    "ui_message_alerts",
    "ui_show_grid",
    "ui_show_partial_grid_around_construction",
    "ui_always_show_rotation_buttons",
    "ui_show_roaming_path",
    "ui_draw_cloud_shadows",
    "ui_display_fps",
    "ui_ask_confirmation_on_file_overwrite",
    "gameplay_change_max_grand_temples",
    "gameplay_change_jealous_gods",
    "gameplay_change_global_labour",
    "gameplay_change_retire_at_60",
    "gameplay_change_fixed_workers",
    "gameplay_wolves_block",
    "gameplay_buyers_dont_distribute",
    "gameplay_change_getting_granaries_go_offroad",
    "gameplay_change_granaries_get_double",
    "gameplay_change_allow_exporting_from_granaries",
    "gameplay_change_tower_sentries_go_offroad",
    "gameplay_change_farms_deliver_close",
    "gameplay_change_only_deliver_to_accepting_granaries",
    "gameplay_change_all_houses_merge",
    "gameplay_change_random_mine_or_pit_collapses_take_money",
    "gameplay_change_multiple_barracks",
    "gameplay_change_warehouses_dont_accept",
    "gameplay_change_markets_dont_accept",
    "gameplay_change_warehouses_granaries_over_road_placement",
    "gameplay_change_houses_dont_expand_into_gardens",
    "gameplay_change_monuments_boost_culture_rating",
    "gameplay_change_disable_infinite_wolves_spawning",
    "gameplay_change_romers_dont_skip_corners",
    "gameplay_change_yearly_autosave",
    "gameplay_change_auto_kill_animals",
    "gameplay_change_nonmilitary_gates_allow_walkers",
    "gameplay_change_max_autosave_slots",
    "ui_show_speedrun_info",
    "ui_show_desirability_range",
    "ui_draw_asclepius",
    "ui_show_desirability_range_all",
    "ui_highlight_selected_building",
    "gameplay_change_caravans_move_off_road",
    "ui_draw_weather",
    "gameplay_change_storage_step_4",
    "ui_move_savings_to_right",
    "gameplay_patrician_devolution_fix"

};

static const char *ini_string_keys[] = {
    "ui_language_dir",
};

static int values[CONFIG_MAX_ENTRIES];
static char string_values[CONFIG_STRING_MAX_ENTRIES][CONFIG_STRING_VALUE_MAX];

static int default_values[CONFIG_MAX_ENTRIES] = {
    [CONFIG_GENERAL_ENABLE_AUDIO] = 1,
    [CONFIG_GENERAL_MASTER_VOLUME] = 100,
    [CONFIG_GENERAL_ENABLE_MUSIC_RANDOMISE] = 0,
    [CONFIG_GENERAL_ENABLE_VIDEO_SOUND] = 1,
    [CONFIG_GENERAL_VIDEO_VOLUME] = 100,
    [CONFIG_GENERAL_HAS_SET_USER_DIRECTORIES] = 1,
    [CONFIG_UI_SIDEBAR_INFO] = 1,
    [CONFIG_UI_SMOOTH_SCROLLING] = 1,
    [CONFIG_UI_SHOW_WATER_STRUCTURE_RANGE] = 1,
    [CONFIG_UI_SHOW_CONSTRUCTION_SIZE] = 1,
    [CONFIG_UI_HIGHLIGHT_LEGIONS] = 1,
    [CONFIG_UI_ASK_CONFIRMATION_ON_FILE_OVERWRITE] = 1,
    [CONFIG_SCREEN_DISPLAY_SCALE] = 100,
    [CONFIG_SCREEN_CURSOR_SCALE] = 100,
    [CONFIG_GP_CH_MAX_GRAND_TEMPLES] = 2,
    [CONFIG_UI_SHOW_DESIRABILITY_RANGE] = 0,
    [CONFIG_UI_SHOW_DESIRABILITY_RANGE_ALL] = 0,
    [CONFIG_UI_HIGHLIGHT_SELECTED_BUILDING] = 1,
    [CONFIG_GP_CH_MAX_AUTOSAVE_SLOTS] = 10,
    [CONFIG_GENERAL_NEXT_AUTOSAVE_SLOT] = 0,
    [CONFIG_GP_CARAVANS_MOVE_OFF_ROAD] = 0,
    [CONFIG_UI_DRAW_WEATHER] = 0,
    [CONFIG_GP_STORAGE_INCREMENT_4] = 0,
    [CONFIG_UI_MOVE_SAVINGS_TO_RIGHT] = 0,
    [CONFIG_GP_CH_PATRICIAN_DEVOLUTION_FIX] = 1,
};

static const char default_string_values[CONFIG_STRING_MAX_ENTRIES][CONFIG_STRING_VALUE_MAX] = { 0 };

int config_get(config_key key)
{
    return values[key];
}

void config_set(config_key key, int value)
{
    values[key] = value;
}

const char *config_get_string(config_string_key key)
{
    return string_values[key];
}

void config_set_string(config_string_key key, const char *value)
{
    if (!value) {
        string_values[key][0] = 0;
    } else {
        snprintf(string_values[key], CONFIG_STRING_VALUE_MAX, "%s", value);
    }
}

int config_get_default_value(config_key key)
{
    return default_values[key];
}

const char *config_get_default_string_value(config_string_key key)
{
    return default_string_values[key];
}

static void set_defaults(void)
{
    for (int i = 0; i < CONFIG_MAX_ENTRIES; ++i) {
        values[i] = default_values[i];
    }
    snprintf(string_values[CONFIG_STRING_UI_LANGUAGE_DIR], CONFIG_STRING_VALUE_MAX, "%s",
        default_string_values[CONFIG_STRING_UI_LANGUAGE_DIR]);
}

void config_load(void)
{
    set_defaults();
    needs_user_directory_setup = 1;
    const char *file_name = dir_get_file_at_location(INI_FILENAME, PATH_LOCATION_CONFIG);
    if (!file_name) {
        return;
    }
    FILE *fp = file_open(file_name, "rt");
    if (!fp) {
        return;
    }
    // Override default, if value is the same at end, then we never setup the directories
    needs_user_directory_setup = 0;
    values[CONFIG_GENERAL_HAS_SET_USER_DIRECTORIES] = -1;

    char line_buffer[MAX_LINE];
    char *line;
    while ((line = fgets(line_buffer, MAX_LINE, fp)) != 0) {
        // Remove newline from string
        size_t size = strlen(line);
        while (size > 0 && (line[size - 1] == '\n' || line[size - 1] == '\r')) {
            line[--size] = 0;
        }
        char *equals = strchr(line, '=');
        if (equals) {
            *equals = 0;
            for (int i = 0; i < CONFIG_MAX_ENTRIES; i++) {
                if (strcmp(ini_keys[i], line) == 0) {
                    int value = atoi(&equals[1]);
                    log_info("Config key", ini_keys[i], value);
                    values[i] = value;
                    break;
                }
            }
            for (int i = 0; i < CONFIG_STRING_MAX_ENTRIES; i++) {
                if (strcmp(ini_string_keys[i], line) == 0) {
                    const char *value = &equals[1];
                    log_info("Config key", ini_string_keys[i], 0);
                    log_info("Config value", value, 0);
                    snprintf(string_values[i], CONFIG_STRING_VALUE_MAX, "%s", value);
                    break;
                }
            }
        }
    }
    file_close(fp);
    if (values[CONFIG_GENERAL_HAS_SET_USER_DIRECTORIES] == -1) {
        values[CONFIG_GENERAL_HAS_SET_USER_DIRECTORIES] = default_values[CONFIG_GENERAL_HAS_SET_USER_DIRECTORIES];
        needs_user_directory_setup = 1;
    }
}

int config_must_configure_user_directory(void)
{
    return needs_user_directory_setup;
}

void config_save(void)
{
    const char *file_name = dir_append_location(INI_FILENAME, PATH_LOCATION_CONFIG);
    if (!file_name) {
        return;
    }
    FILE *fp = file_open(file_name, "wt");
    if (!fp) {
        log_error("Unable to write configuration file", INI_FILENAME, 0);
        return;
    }
    for (int i = 0; i < CONFIG_MAX_ENTRIES; i++) {
        fprintf(fp, "%s=%d\n", ini_keys[i], values[i]);
    }
    for (int i = 0; i < CONFIG_STRING_MAX_ENTRIES; i++) {
        fprintf(fp, "%s=%s\n", ini_string_keys[i], string_values[i]);
    }
    file_close(fp);
    needs_user_directory_setup = 0;
}
