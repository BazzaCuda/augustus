#include "rotation.h"

#include "building/connectable.h"
#include "building/construction.h"
#include "building/menu.h"
#include "building/properties.h"
#include "building/variant.h"
#include "city/view.h"
#include "city/warning.h"
#include "core/config.h"
#include "core/direction.h"
#include "core/lang.h"
#include "core/string.h"
#include "core/time.h"
#include "map/grid.h"

#define MAX_ROTATION 3

static struct {
    int rotation;
    int extra_rotation;
    int road_orientation;
    uint8_t rotation_text[100];
    int warning_id;
} data = { 0, 0, 1 };

static int get_num_rotations(building_type type)
{
    int variants = building_construction_type_num_cycles(type);
    if (variants > 1) {
        return variants;
    }
    variants = building_variant_get_number_of_variants(type);
    if (variants > 0) {
        return variants;
    }
    variants = building_connectable_num_variants(type);
    if (variants > 0) {
        return variants;
    }
    if (building_properties_for_type(type)->rotation_offset) {
        return 2;
    }
    switch (type) {
        case BUILDING_MENU_SHRINES:
            return 10;
        case BUILDING_MENU_LARGE_TEMPLES:
        case BUILDING_MENU_SMALL_TEMPLES:
            return 5;
        case BUILDING_FORT_JAVELIN:
        case BUILDING_FORT_LEGIONARIES:
        case BUILDING_FORT_MOUNTED:
        case BUILDING_FORT_AUXILIA_INFANTRY:
        case BUILDING_FORT_ARCHERS:
        case BUILDING_WAREHOUSE:
            return 4;
        case BUILDING_HIPPODROME:
        case BUILDING_GATEHOUSE:
        case BUILDING_TRIUMPHAL_ARCH:
            return 2;
        default:
            return 0;
    }
}

static void update_rotation_message(void)
{
    uint8_t *cursor = data.rotation_text;
    building_type type = building_construction_type();
    cursor += string_from_int(cursor, data.extra_rotation + 1, 0);
    cursor = string_copy(string_from_ascii("/"), cursor, 100 - (int) (cursor - data.rotation_text));
    cursor += string_from_int(cursor, get_num_rotations(type), 0);
    cursor = string_copy(string_from_ascii(" "), cursor, 100 - (int) (cursor - data.rotation_text));
    string_copy(lang_get_string(28, type), cursor, 100 - (int) (cursor - data.rotation_text));

    data.warning_id = city_warning_show_custom(data.rotation_text, data.warning_id);
}

static void rotate_forward(void)
{
    building_construction_cycle_forward();
    data.rotation += 1;
    data.extra_rotation += 1;
    if (data.rotation > MAX_ROTATION) {
        data.rotation = 0;
    }
    building_type type = building_construction_type();
    if (data.extra_rotation >= get_num_rotations(type)) {
        data.extra_rotation = 0;
    }
    update_rotation_message();
}

static void rotate_backward(void)
{
    building_construction_cycle_back();
    data.rotation -= 1;
    data.extra_rotation -= 1;
    if (data.rotation < 0) {
        data.rotation = MAX_ROTATION;
    }
    if (data.extra_rotation < 0) {
        data.extra_rotation = get_num_rotations(building_construction_type()) - 1;
    }
    update_rotation_message();
}

int building_rotation_get_road_orientation(void)
{
    return data.road_orientation;
}

void building_rotation_force_two_orientations(void)
{
    // for composite buildings like hippodrome
    if (data.rotation == 1) {
        data.rotation = 3;
        return;
    }
    if (data.rotation == 2) {
        data.rotation = 0;
        return;
    }
}

int building_rotation_get_rotation(void)
{
    return data.rotation;
}

int building_rotation_get_rotation_with_limit(int limit)
{
    return data.extra_rotation % limit;
}

void building_rotation_rotate_forward(void)
{
    if (building_rotation_type_has_rotations(building_construction_type())) {
        rotate_forward();
        data.road_orientation = data.road_orientation == 1 ? 2 : 1;
    }
}

void building_rotation_rotate_backward(void)
{
    if (building_rotation_type_has_rotations(building_construction_type())) {
        rotate_backward();
        data.road_orientation = data.road_orientation == 1 ? 2 : 1;
    }
}

void building_rotation_reset_rotation(void)
{
    data.rotation = 0;
    data.extra_rotation = 0;
}

void building_rotation_setup_rotation(int variant)
{
    building_rotation_reset_rotation();
    data.warning_id = 0;

    for (int i = 0; i < variant; i++) {
        building_rotation_rotate_forward();
    }
    update_rotation_message();
}

void building_rotation_remove_rotation(void)
{
    city_warning_clear_id(data.warning_id);
    data.warning_id = 0;
}

int building_rotation_get_building_orientation(int building_rotation)
{
    return (2 * building_rotation + city_view_orientation()) % 8;
}

int building_rotation_get_delta_with_rotation(int default_delta)
{
    switch (data.rotation) {
        case 0:
            return map_grid_delta(default_delta, 0);
        case 1:
            return map_grid_delta(0, -default_delta);
        case 2:
            return map_grid_delta(-default_delta, 0);
        default:
            return map_grid_delta(0, default_delta);
    }
}

void building_rotation_get_offset_with_rotation(int offset, int rotation, int *x, int *y)
{
    switch (rotation) {
        case 0:
            *x = offset;
            *y = 0;
            return;
        case 1:
            *x = 0;
            *y = -offset;
            return;
        case 2:
            *x = -offset;
            *y = 0;
            return;
        default:
            *x = 0;
            *y = offset;
            return;
    }
}

int building_rotation_get_corner(int rotation)
{
    switch (rotation) {
        case DIR_2_RIGHT:
            return 4; // left corner
        case DIR_4_BOTTOM:
            return 8; // bottom corner
        case DIR_6_LEFT:
            return 5; // right corner
        default:
            return 0; // top corner
    }
}

static int menu_has_buildings_with_cycles(build_menu_group menu, building_type type)
{
    int num_items = building_menu_count_items(menu);
    int item_index = -1;
    for (int i = 0; i < num_items; i++) {
        item_index = building_menu_next_index(menu, item_index);
        building_type item_type = building_menu_type(menu, item_index);
        if (item_type != type && building_construction_type_can_cycle(item_type)) {
            return 1;
        }
    }
    return 0;
}

int building_rotation_type_has_rotations(building_type type)
{
    if (building_variant_has_variants(type) || building_properties_for_type(type)->rotation_offset ||
        building_construction_type_can_cycle(type)) {
        return 1;
    }
    build_menu_group menu = building_menu_get_submenu_for_type(type);
    if (menu) {
        return menu_has_buildings_with_cycles(menu, type);
    }
    switch (type) {
        case BUILDING_FORT_JAVELIN:
        case BUILDING_FORT_LEGIONARIES:
        case BUILDING_FORT_MOUNTED:
        case BUILDING_FORT_AUXILIA_INFANTRY:
        case BUILDING_FORT_ARCHERS:
        case BUILDING_WAREHOUSE:
        case BUILDING_HIPPODROME:
        case BUILDING_GATEHOUSE:
        case BUILDING_TRIUMPHAL_ARCH:
            return 1;
        default:
            return 0;
    }
}
