#ifndef PLATFORM_ANDROID_H
#define PLATFORM_ANDROID_H

#ifdef __ANDROID__

#define PLATFORM_NO_USER_DIRECTORIES

const char *android_show_c3_path_dialog(int again);
float android_get_screen_density(void);
int android_get_file_descriptor(const char *filename, const char *mode);
int android_set_base_path(const char *path);
int android_get_directory_contents(const char *dir, int type, const char *extension, int (*callback)(const char *, long));
int android_create_directory(const char *name);
int android_remove_file(const char *filename);

void *android_open_asset(const char *asset, const char *mode);

#define PLATFORM_USE_VIRTUAL_KEYBOARD
void platform_show_virtual_keyboard(void);
void platform_hide_virtual_keyboard(void);

#endif // __ANDROID__
#endif // PLATFORM_ANDROID_H
