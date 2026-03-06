# AGENTS.md

## Purpose

This document guides coding agents working in this repository.
It explains build/run commands, coding style, architecture boundaries,
and practical rules for making safe, readable changes.

---

## Repository Overview

- Language: C
- UI toolkit: GTK4
- Build tool: Makefile
- Binary output: `build/gtk-app`
- Entry point: `main.c`
- Demo entry callback: `demo_app_start(...)`
- Public headers: `include/`
- Implementations: `src/` and `src/widgets/`

Current architecture separates app composition from low-level GTK calls.
Use wrapper modules instead of scattering raw GTK calls across app code.



## Build, Run, and Debug Commands

Primary commands:

- Build: `make`
- Clean build: `make clean && make`
- Debug build: `make debug`
- Run: `make run`
- Run directly: `./build/gtk-app`




## Focused Build Commands (Single Module)

The Makefile supports object-level builds.
Build only one changed module when iterating:

- `make build/src/widgets/menu.o`
- `make build/src/widgets/window.o`
- `make build/src/widgets/button.o`
- `make build/src/demo_app.o`

Then relink app:

- `make build/gtk-app`



## Test Commands

There is currently no formal automated test suite.

Use this validation sequence:

1. `make clean && make`
2. `./build/gtk-app --help`
3. `make run` and manually validate changed UI flows

### Single-Test Equivalent (Current Project)

Since no test runner exists, a “single test” means:

1. Build the changed object only (example: `make build/src/widgets/input.o`)
2. Relink app (`make build/gtk-app`)
3. Launch and verify only that feature path

When automated tests are added, extend this section with exact single-test syntax.

---

## File Organization Rules

Keep module pairing consistent:

- Header in `include/widgets/<name>.h`
- Source in `src/widgets/<name>.c`

App composition belongs in `src/demo_app.c`.
Application bootstrap belongs in `main.c`.

Do not reintroduce legacy flat files in repo root (`button.c`, `input.c`, etc.).

---

## Include and Dependency Style

In `.c` files:

1. Include the module header first
2. Include other project headers
3. Include system headers (`<string.h>`, etc.)

Rules:

- No unused includes
- Prefer forward declarations only where they improve clarity
- Keep includes local to what is used

---

## Naming Conventions

Follow existing conventions:

- Functions: `snake_case`
- Constructors: `create_*`
- Callback functions: `on_*`
- Struct config types: `*_config`
- Enum values: `UPPER_SNAKE_CASE`

Examples:

- `button_config`
- `create_menubar`
- `on_spin_value_changed`
- `MENU_ITEM_SUBMENU`

---

## Config Struct Design Guidelines

Wrapper APIs should be configuration-driven.

Do:

- Add meaningful fields only
- Keep defaults safe when `config == NULL`
- Use designated initializers at call sites
- Keep callback pointers in config for interactive widgets

Avoid:

- long positional argument constructors
- redundant fields that mirror each other
- pushing GTK complexity to app code

---

## Error Handling and Safety

Apply defensive checks consistently:

- Validate pointers (`config`, `widget`, child arrays)
- Return a sensible default widget when config is missing
- Avoid null dereference paths
- Gracefully skip optional features when fields are absent

When using GLib allocations:

- `g_free` for duplicated strings
- `g_object_unref` for owned objects
- keep ownership clear and local

---

## Formatting and Readability

This repo currently has mixed indentation across files.
Do not reformat unrelated code.

Rules:

- Match existing style in touched file
- Keep functions linear and readable
- Extract helpers when one function gets too dense
- Prefer explicit code over clever shortcuts
- Keep comments for non-obvious logic only


---

## Menu and Action Wiring Guidelines

Menu items are callback-driven via config structs, similar to button callbacks.

Keep menu behavior in wrapper layer:

- nested submenu traversal
- action creation/registration
- menu model generation

App code should mostly provide menu tree config and callbacks,
not manual per-item GTK action plumbing.

---

## Window and Theme Guidelines

Window-level behavior should come from `window_config` fields.
Do not rely on ad-hoc post-creation hacks unless necessary.

Theme scope is intentionally narrow right now:

- CSS classes only for themed buttons (`btn-primary`, `btn-success`, `btn-danger`)

Do not broaden global CSS usage unless explicitly requested.

---

## Build Hygiene and Process Hygiene

- Rebuild after interface/header changes
- Verify app launch after major UI edits
- If output appears stale, check for old running process before debugging code

Useful process check:

- `pgrep -af "gtk-app|org.gtk.demo"`

---

## Commit Guidance for Agents

- Make small, logical commits by feature/module
- Write messages in English
- Prefer explicit messages like:
  - `Add button, input, and toggle widgets`
  - `Add menu bar and nested submenu support`
- Avoid vague messages (`fix`, `update`, `misc`)

Do not commit unless the user explicitly asks.

---

## Agent Checklist

Before coding:

1. Read `Makefile`
2. Read target module header and source
3. Confirm no new policy files were added

After coding:

1. Build (`make` or `make clean && make`)
2. Smoke test (`./build/gtk-app --help`)
3. Report changed files and behavior clearly
