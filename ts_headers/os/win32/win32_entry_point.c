#if !defined(OS_NO_ENTRY_POINT)
int WinMain(HINSTANCE instance, HINSTANCE prev_instance, LPSTR lp_cmd_line, int n_show_cmd)
{
    w32_hinstance = instance;
    APP_EntryPoint();
    return 0;
}
#endif
