# GTK Wrapper Implementation Guide

This file explains how the current wrapper is organized, how each part works, and how the demo app is connected.

## 1) Project Structure

- `main.c`
  - Creates `GtkApplication`
  - Connects the GTK `activate` signal to `demo_app_start`
- `include/demo_app.h`
  - Public entry function for the demo app
- `src/demo_app.c`
  - Demo UI composition (pages, callbacks, menu setup, window setup)
- `include/widgets/*.h`
  - Public wrapper APIs and config structs
- `src/widgets/*.c`
  - Wrapper implementations (actual GTK calls)

The main idea is: application code uses wrapper config structs, wrapper code handles raw GTK API calls.

## 2) Core Pattern Used Everywhere

For each widget family:

1. Define a `*_config` struct in `include/widgets/*.h`
2. Expose a `create_*` function
3. Implement all setup logic in `src/widgets/*.c`
4. Optional callbacks are passed through config structs and connected internally

This keeps calling code simple and consistent.

## 3) Shared Style Layer

- File: `include/widgets/common.h`, `src/widgets/common.c`
- Type: `widget_style_config`
- Function: `apply_widget_style(GtkWidget *widget, const widget_style_config *style)`

What it applies:
- CSS class(es)
- size request
- margins
- optional align/expand/sensitive/visible flags
- tooltip

Why it exists:
- avoid repeating the same style calls inside every wrapper
- keep widget wrappers readable

## 4) Window Wrapper

- File: `include/widgets/window.h`, `src/widgets/window.c`
- Function: `create_window(GtkApplication *app, const window_config *config)`

What it sets from `window_config`:
- title, icon, widget name
- default/min sizes
- resizable, decorated, modal, maximized, present-on-create
- style via `apply_widget_style`
- background color/image support (generated CSS scoped by window widget name)

Important detail:
- Background is applied inside the wrapper based on struct fields, not from external helper code.

## 5) Container Wrappers

- File: `include/widgets/container.h`, `src/widgets/container.c`
- Functions:
  - `create_grid`
  - `create_box`
  - `create_stack`
  - `create_stack_sidebar`
  - `stack_add_page`

What they do:
- Build common layout widgets with spacing/homogeneous/transition options
- Attach stack to sidebar
- Add titled pages to stack

## 6) Interactive Input Wrappers

- File: `include/widgets/input.h`, `src/widgets/input.c`
- Functions:
  - `create_entry`
  - `create_dropdown`
  - `create_spin_button`

Features:
- Entry: placeholder/default text/password/editable/max length/input purpose
- Dropdown: options, selected item, search enabled
- Spin button: min/max/step/page/value/digits/numeric/wrap
- Callbacks connected from struct fields

## 7) Button and Toggle Wrappers

- Buttons: `include/widgets/button.h`, `src/widgets/button.c`
  - regular button + radio button creation
  - optional icon support
  - callback wiring
  - button theme variants mapped to CSS classes: `btn-primary`, `btn-success`, `btn-danger`

- Toggles: `include/widgets/toggle.h`, `src/widgets/toggle.c`
  - checkbox wrapper
  - switch-row wrapper (label + switch in a row)
  - callback wiring

## 8) Display Wrappers

- File: `include/widgets/display.h`, `src/widgets/display.c`
- Functions:
  - `create_progress_bar`
  - `create_spinner`
  - `create_label`
  - `create_image`

Purpose:
- keep display widgets configurable through structs instead of many direct GTK calls in app code

## 9) Date and Separator Wrappers

- Date: `include/widgets/date.h`, `src/widgets/date.c`
  - calendar setup + selected-date callback

- Separator: `include/widgets/separator.h`, `src/widgets/separator.c`
  - simple orientation-based separator creation

## 10) Dialog Wrapper (Modern API)

- File: `include/widgets/dialog.h`, `src/widgets/dialog.c`
- Function: `show_alert_dialog(const alert_dialog_config *config)`

Uses:
- `GtkAlertDialog` (modern, non-deprecated)

Supports:
- message/detail/buttons/default/cancel/modal
- async response callback

Implementation detail:
- wrapper creates an async context, resolves selected response index in finish callback, then forwards to user callback.

## 11) Menu Wrapper

- File: `include/widgets/menu.h`, `src/widgets/menu.c`
- Functions:
  - `create_menubar`
  - `create_menu_panel`

How it works:
- Menu definitions are recursive (`menu_item_config` can contain children)
- Supports item kinds:
  - `MENU_ITEM_ACTION`
  - `MENU_ITEM_SUBMENU`
  - `MENU_ITEM_SECTION`
- For action items, wrapper auto-generates an internal action name
- Wrapper registers actions into provided `GActionMap`
- Callback is taken directly from each menu item struct (`on_activate`, `user_data`)

This gives button-like callback behavior for menu items while still using GTK action infrastructure internally.

## 12) Theme

- File: `include/widgets/theme.h`, `src/widgets/theme.c`
- Function: `apply_demo_theme()`

Current CSS scope:
- button theme classes only:
  - `.btn-primary`
  - `.btn-success`
  - `.btn-danger`

## 13) Demo App Flow

- Entry: `demo_app_start` in `src/demo_app.c`
- Steps:
  1. apply theme once
  2. create window from `window_config`
  3. build showcase root
  4. add stack pages (inputs, buttons, toggles, displays, dialogs)
  5. attach callbacks that update shared status label
  6. present window

Demo state:
- `showcase_state` stores shared widget references (status, progress, spinner, switch)

## 14) Build System

- File: `Makefile`
- Uses object-based build with auto dependency generation (`-MMD -MP`)
- Fixes stale build behavior by tracking header dependencies and compiling per source file into `build/.../*.o`

## 15) Quick Mental Model

- App layer (`src/demo_app.c`) should read like UI composition.
- Wrapper layer (`src/widgets/*.c`) should contain GTK complexity.
- Config structs in headers are the contract between both layers.
