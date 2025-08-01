#include "city_figure.h"

#include "city/view.h"
#include "figure/formation.h"
#include "figure/image.h"
#include "figuretype/editor.h"
#include "graphics/image.h"
#include "graphics/text.h"

static color_t get_highlight_mask(int highlight_mask)
{
    switch (highlight_mask) {
        case FIGURE_HIGHLIGHT_NONE:
            return COLOR_MASK_NONE;
        case FIGURE_HIGHLIGHT_RED:
            return COLOR_MASK_LEGION_HIGHLIGHT;
        case FIGURE_HIGHLIGHT_GREEN:
            return COLOR_MASK_GREEN;
        default:
            return COLOR_MASK_NONE;
    }
}

static void draw_figure_with_cart(const figure *f, int x, int y, color_t color_mask, float scale)
{
    if (f->y_offset_cart >= 0) {
        image_draw(f->image_id, x, y, color_mask, scale);
        image_draw(f->cart_image_id, x + f->x_offset_cart, y + f->y_offset_cart, color_mask, scale);
    } else {
        image_draw(f->cart_image_id, x + f->x_offset_cart, y + f->y_offset_cart, color_mask, scale);
        image_draw(f->image_id, x, y, color_mask, scale);
    }
}

static void draw_hippodrome_horse(const figure *f, int x, int y, color_t color_mask, float scale)
{
    int val = f->wait_ticks_missile;
    switch (city_view_orientation()) {
        case DIR_0_TOP:
            x += 10;
            if (val <= 10) {
                y -= 2;
            } else if (val <= 11) {
                y -= 10;
            } else if (val <= 12) {
                y -= 18;
            } else if (val <= 13) {
                y -= 16;
            } else if (val <= 20) {
                y -= 14;
            } else if (val <= 21) {
                y -= 10;
            } else {
                y -= 2;
            }
            break;
        case DIR_2_RIGHT:
            x -= 10;
            if (val <= 9) {
                y -= 12;
            } else if (val <= 10) {
                y += 4;
            } else if (val <= 11) {
                x -= 5;
                y += 2;
            } else if (val <= 13) {
                x -= 5;
            } else if (val <= 20) {
                y -= 2;
            } else if (val <= 21) {
                y -= 6;
            } else {
                y -= 12;
            }
            break;
        case DIR_4_BOTTOM:
            x += 20;
            if (val <= 9) {
                y += 4;
            } else if (val <= 10) {
                x += 10;
                y += 4;
            } else if (val <= 11) {
                x += 10;
                y -= 4;
            } else if (val <= 13) {
                y -= 6;
            } else if (val <= 20) {
                y -= 12;
            } else if (val <= 21) {
                y -= 10;
            } else {
                y -= 2;
            }
            break;
        case DIR_6_LEFT:
            x -= 10;
            if (val <= 9) {
                y -= 12;
            } else if (val <= 10) {
                y += 4;
            } else if (val <= 11) {
                y += 2;
            } else if (val <= 13) {
                // no change
            } else if (val <= 20) {
                y -= 2;
            } else if (val <= 21) {
                y -= 6;
            } else {
                y -= 12;
            }
            break;
    }
    draw_figure_with_cart(f, x, y, color_mask, scale);
}

static void draw_fort_standard(const figure *f, int x, int y, float scale)
{
    if (!formation_get(f->formation_id)->in_distant_battle) {
        // base
        image_draw(f->image_id, x, y, COLOR_MASK_NONE, scale);
        // flag
        int flag_height = image_get(f->cart_image_id)->height;
        image_draw(f->cart_image_id, x, y - flag_height, COLOR_MASK_NONE, scale);
        // top icon
        int icon_image_id = formation_get(f->formation_id)->legion_flag_id;
        image_draw(icon_image_id, x, y - image_get(icon_image_id)->height - flag_height, COLOR_MASK_NONE, scale);
    }
}

static void draw_map_flag(const figure *f, int x, int y, float scale)
{
    // base
    image_draw(f->image_id, x, y, COLOR_MASK_NONE, scale);
    // flag
    image_draw(f->cart_image_id, x, y - image_get(f->cart_image_id)->height, COLOR_MASK_NONE, scale);
    // flag number
    int number = 0;
    int id = f->resource_id;
    if (id >= MAP_FLAG_INVASION_MIN && id < MAP_FLAG_INVASION_MAX) {
        number = id - MAP_FLAG_INVASION_MIN + 1;
    } else if (id >= MAP_FLAG_FISHING_MIN && id < MAP_FLAG_FISHING_MAX) {
        number = id - MAP_FLAG_FISHING_MIN + 1;
    } else if (id >= MAP_FLAG_HERD_MIN && id < MAP_FLAG_HERD_MAX) {
        number = id - MAP_FLAG_HERD_MIN + 1;
    }
    if (number > 0) {
        text_draw_number_scaled(number, '@', 0, x + 6, y + 7, FONT_NORMAL_PLAIN, COLOR_WHITE, scale);
    }
}

static void tile_cross_country_offset_to_pixel_offset(int cross_country_x, int cross_country_y,
    int *pixel_x, int *pixel_y)
{
    int dir = city_view_orientation();
    if (dir == DIR_0_TOP || dir == DIR_4_BOTTOM) {
        int base_pixel_x = 2 * cross_country_x - 2 * cross_country_y;
        int base_pixel_y = cross_country_x + cross_country_y;
        *pixel_x = dir == DIR_0_TOP ? base_pixel_x : -base_pixel_x;
        *pixel_y = dir == DIR_0_TOP ? base_pixel_y : -base_pixel_y;
    } else {
        int base_pixel_x = 2 * cross_country_x + 2 * cross_country_y;
        int base_pixel_y = cross_country_x - cross_country_y;
        *pixel_x = dir == DIR_2_RIGHT ? base_pixel_x : -base_pixel_x;
        *pixel_y = dir == DIR_6_LEFT ? base_pixel_y : -base_pixel_y;
    }
}

static int tile_progress_to_pixel_offset_x(int direction, int progress)
{
    if (progress >= 15) {
        return 0;
    }
    switch (direction) {
        case DIR_0_TOP:
        case DIR_2_RIGHT:
            return 2 * progress - 28;
        case DIR_1_TOP_RIGHT:
            return 4 * progress - 56;
        case DIR_4_BOTTOM:
        case DIR_6_LEFT:
            return 28 - 2 * progress;
        case DIR_5_BOTTOM_LEFT:
            return 56 - 4 * progress;
        default:
            return 0;
    }
}

static int tile_progress_to_pixel_offset_y(int direction, int progress)
{
    if (progress >= 15) {
        return 0;
    }
    switch (direction) {
        case DIR_0_TOP:
        case DIR_6_LEFT:
            return 14 - progress;
        case DIR_2_RIGHT:
        case DIR_4_BOTTOM:
            return progress - 14;
        case DIR_3_BOTTOM_RIGHT:
            return 2 * progress - 28;
        case DIR_7_TOP_LEFT:
            return 28 - 2 * progress;
        default:
            return 0;
    }
}

static void tile_progress_to_pixel_offset(int direction, int progress, int *pixel_x, int *pixel_y)
{
    *pixel_x = tile_progress_to_pixel_offset_x(direction, progress);
    *pixel_y = tile_progress_to_pixel_offset_y(direction, progress);
}

static void adjust_pixel_offset(const figure *f, int *pixel_x, int *pixel_y)
{
    // determining x/y offset on tile
    int x_offset = 0;
    int y_offset = 0;
    if (f->use_cross_country) {
        tile_cross_country_offset_to_pixel_offset(
            f->cross_country_x % 15, f->cross_country_y % 15, &x_offset, &y_offset);
        y_offset -= f->missile_height;
    } else {
        int direction = figure_image_normalize_direction(f->direction);
        tile_progress_to_pixel_offset(direction, f->progress_on_tile, &x_offset, &y_offset);
        y_offset -= f->current_height;
        if (f->figures_on_same_tile_index && f->type != FIGURE_BALLISTA) {
            // an attempt to not let people walk through each other
            static const int BUSY_ROAD_X_OFFSETS[] = {
                0, 8, 8, -8, -8, 0, 16, 0, -16, 8, -8, 16, -16, 16, -16, 8, -8, 0, 24, 0, -24
            };
            static const int BUSY_ROAD_Y_OFFSETS[] = {
                0, 0, 8, 8, -8, -16, 0, 16, 0, -16, 16, 8, -8, -8, 8, 16, -16, -24, 0, 24, 0
            };
            static const int BUSY_ROAD_OFFSET_LEN = 21;
            x_offset += BUSY_ROAD_X_OFFSETS[f->figures_on_same_tile_index % BUSY_ROAD_OFFSET_LEN];
            y_offset += BUSY_ROAD_Y_OFFSETS[f->figures_on_same_tile_index % BUSY_ROAD_OFFSET_LEN];
        }
    }

    x_offset += 29;
    y_offset += 15;

    if (f->image_id >= 10000) {
        // TODO
        // Ugly hack, remove
        // Draws new walkers at their proper spots
        x_offset -= 26;
        y_offset -= 29;
    }


    const image *img = f->is_enemy_image ? image_get_enemy(f->image_id) : image_get(f->image_id);
    *pixel_x += x_offset - (img->animation ? img->animation->sprite_offset_x : 0);
    *pixel_y += y_offset - (img->animation ? img->animation->sprite_offset_y : 0);
}

static void draw_figure(const figure *f, int x, int y, float scale, int highlight)
{
    color_t color_mask = get_highlight_mask(highlight);
    if (f->cart_image_id) {
        switch (f->type) {
            case FIGURE_CART_PUSHER:
            case FIGURE_DEPOT_CART_PUSHER:
            case FIGURE_WAREHOUSEMAN:
            case FIGURE_LION_TAMER:
            case FIGURE_DOCKER:
            case FIGURE_NATIVE_TRADER:
            case FIGURE_IMMIGRANT:
            case FIGURE_EMIGRANT:
            case FIGURE_LIGHTHOUSE_SUPPLIER:
                draw_figure_with_cart(f, x, y, color_mask, scale);
                break;
            case FIGURE_HIPPODROME_HORSES:
                draw_hippodrome_horse(f, x, y, color_mask, scale);
                break;
            case FIGURE_FORT_STANDARD:
                draw_fort_standard(f, x, y, scale);
                break;
            case FIGURE_MAP_FLAG:
                draw_map_flag(f, x, y, scale);
                break;
            default:
                image_draw(f->image_id, x, y, color_mask, scale);
                break;
        }
    } else {
        if (f->is_enemy_image) {
            image_draw_enemy(f->image_id, x, y, scale);
        } else {

            image_draw(f->image_id, x, y, color_mask, scale);
        }
    }
}

void city_draw_figure(const figure *f, int x, int y, float scale, int highlight)
{
    adjust_pixel_offset(f, &x, &y);
    draw_figure(f, x, y, scale, highlight);
}

void city_draw_selected_figure(const figure *f, int x, int y, float scale, pixel_coordinate *coord)
{
    adjust_pixel_offset(f, &x, &y);
    draw_figure(f, x, y, scale, 0);
    coord->x = x;
    coord->y = y;
}
