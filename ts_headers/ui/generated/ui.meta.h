#define UI_StackDecls struct{\
struct { F32 active; F32 v[64]; U64 count; } fixed_x;\
struct { F32 active; F32 v[64]; U64 count; } fixed_y;\
struct { F32 active; F32 v[64]; U64 count; } fixed_width;\
struct { F32 active; F32 v[64]; U64 count; } fixed_height;\
struct { UI_Size active; UI_Size v[64]; U64 count; } pref_width;\
struct { UI_Size active; UI_Size v[64]; U64 count; } pref_height;\
struct { F32 active; F32 v[64]; U64 count; } min_width;\
struct { F32 active; F32 v[64]; U64 count; } min_height;\
struct { UI_BoxFlags active; UI_BoxFlags v[64]; U64 count; } flags;\
struct { Vec4F32 active; Vec4F32 v[64]; U64 count; } background_color;\
struct { Vec4F32 active; Vec4F32 v[64]; U64 count; } text_color;\
struct { Vec4F32 active; Vec4F32 v[64]; U64 count; } border_color;\
struct { R_Font active; R_Font v[64]; U64 count; } font;\
struct { F32 active; F32 v[64]; U64 count; } corner_radius;\
}

#define UI_ZeroAllStacks(ui_state) do{\
MemoryZeroStruct(&ui_state->fixed_x);\
MemoryZeroStruct(&ui_state->fixed_y);\
MemoryZeroStruct(&ui_state->fixed_width);\
MemoryZeroStruct(&ui_state->fixed_height);\
MemoryZeroStruct(&ui_state->pref_width);\
MemoryZeroStruct(&ui_state->pref_height);\
MemoryZeroStruct(&ui_state->min_width);\
MemoryZeroStruct(&ui_state->min_height);\
MemoryZeroStruct(&ui_state->flags);\
MemoryZeroStruct(&ui_state->background_color);\
MemoryZeroStruct(&ui_state->text_color);\
MemoryZeroStruct(&ui_state->border_color);\
MemoryZeroStruct(&ui_state->font);\
MemoryZeroStruct(&ui_state->corner_radius);\
} while(0)

