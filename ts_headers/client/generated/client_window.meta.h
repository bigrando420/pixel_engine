typedef enum CW_ViewKind
{
CW_ViewKind_Null,
CW_ViewKind_FileSystem,
CW_ViewKind_COUNT
}
CW_ViewKind;

typedef enum CW_CmdFastPath
{
CW_CmdFastPath_Null,
CW_CmdFastPath_Run,
CW_CmdFastPath_PutName,
CW_CmdFastPath_PutNameSpace,
CW_CmdFastPath_PutNamePath,
CW_CmdFastPath_PutNameViewCommand,
CW_CmdFastPath_COUNT
}
CW_CmdFastPath;

typedef enum CW_CmdKind
{
CW_CmdKind_Null,
CW_CmdKind_Window,
CW_CmdKind_NewPanelRight,
CW_CmdKind_NewPanelDown,
CW_CmdKind_NextPanel,
CW_CmdKind_PrevPanel,
CW_CmdKind_ClosePanel,
CW_CmdKind_NextView,
CW_CmdKind_PrevView,
CW_CmdKind_CloseView,
CW_CmdKind_EditView,
CW_CmdKind_OpenProject,
CW_CmdKind_CloseProject,
CW_CmdKind_SystemCommand,
CW_CmdKind_COUNT
}
CW_CmdKind;

extern String8 cw_g_cmd_kind_string_table[14];

extern String8 cw_g_cmd_kind_desc_table[14];

extern CW_CmdFastPath cw_g_cmd_kind_fast_path_table[14];

extern CW_ViewKind cw_g_cmd_kind_view_kind_table[14];

extern B8 cw_g_cmd_kind_keep_input_table[14];

extern B8 cw_g_view_kind_text_focus_table[2];

extern CW_ViewFunction* cw_g_view_kind_function_table[2];

CW_VIEW_FUNCTION_DEF(Null);
CW_VIEW_FUNCTION_DEF(FileSystem);

