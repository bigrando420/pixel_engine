#ifndef UI_WIDGETS_H
#define UI_WIDGETS_H

////////////////////////////////
//~ rjf: Widget Calls

//- rjf: basic widgets
engine_function UI_InteractResult UI_Spacer(UI_Size size);
engine_function UI_InteractResult UI_Label(String8 string);
engine_function UI_InteractResult UI_LabelF(char *fmt, ...);
engine_function UI_InteractResult UI_Button(String8 string);
engine_function UI_InteractResult UI_ButtonF(char *fmt, ...);
#if 0
engine_function UI_InteractResult UI_LineEdit(B32 keyboard_focused, TE_Point *cursor, TE_Point *mark, U64 string_max, String8 *out_string, String8 string);
engine_function UI_InteractResult UI_LineEditF(B32 keyboard_focused, TE_Point *cursor, TE_Point *mark, U64 string_max, String8 *out_string, char *fmt, ...);
#endif

//- rjf: layout widgets
engine_function UI_InteractResult UI_RowBegin(void);
engine_function void UI_RowEnd(void);
engine_function UI_InteractResult UI_ColumnBegin(void);
engine_function void UI_ColumnEnd(void);
engine_function UI_InteractResult UI_NamedRowBegin(String8 string);
engine_function UI_InteractResult UI_NamedRowBeginF(char *fmt, ...);
engine_function void UI_NamedRowEnd(void);
engine_function UI_InteractResult UI_NamedColumnBegin(String8 string);
engine_function UI_InteractResult UI_NamedColumnBeginF(char *fmt, ...);
engine_function void UI_NamedColumnEnd(void);
engine_function UI_InteractResult UI_ScrollableParentBegin(B32 x, B32 y, String8 string);
engine_function UI_InteractResult UI_ScrollableParentBeginF(B32 x, B32 y, char *fmt, ...);
engine_function void UI_ScrollableParentEnd(void);

//- rjf: containers
engine_function UI_Box *UI_WindowBegin(Rng2F32 rect, String8 string);
engine_function UI_Box *UI_WindowBeginF(Rng2F32 rect, char *fmt, ...);
engine_function void UI_WindowEnd(void);
engine_function UI_InteractResult UI_TitleBar(String8 string);
engine_function void UI_ScrollBar(Axis2 axis, UI_Box *scrolled_box);

//- rjf: special buttons
engine_function UI_InteractResult UI_CloseButton(String8 string);
engine_function UI_InteractResult UI_CloseButtonF(char *fmt, ...);
engine_function UI_InteractResult UI_IconButton(R_Handle texture, Rng2F32 src, String8 string);
engine_function UI_InteractResult UI_IconButtonF(R_Handle texture, Rng2F32 src, char *fmt, ...);

//- rjf: miscellaneous widgets
engine_function UI_InteractResult UI_SaturationValuePicker(F32 hue, F32 *saturation, F32 *value, String8 string);
engine_function UI_InteractResult UI_HuePicker(F32 *hue, String8 string);

////////////////////////////////
//~ rjf: Macro Loop Wrappers

#define UI_Row DeferLoop(UI_RowBegin(), UI_RowEnd())
#define UI_Column DeferLoop(UI_ColumnBegin(), UI_ColumnEnd())
#define UI_NamedRow(s) DeferLoop(UI_NamedRowBegin(s), UI_NamedRowEnd())
#define UI_NamedColumn(s) DeferLoop(UI_NamedColumnBegin(s), UI_NamedColumnEnd())
#define UI_NamedRowF(...) DeferLoop(UI_NamedRowBeginF(__VA_ARGS__), UI_NamedRowEnd())
#define UI_NamedColumnF(...) DeferLoop(UI_NamedColumnBeginF(__VA_ARGS__), UI_NamedColumnEnd())

#endif // UI_WIDGETS_H
