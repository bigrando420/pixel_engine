#ifndef ENGINE_FULL_H
#define ENGINE_FULL_H

#define OS_FEATURE_GFX 1

#define MD_FUNCTION exported
#define MD_DEFAULT_SPRINTF 0
#define MD_DEFAULT_ARENA 0
#define MD_IMPL_Vsnprintf ts_stbsp_vsnprintf
#define MD_IMPL_Arena M_Arena
#define MD_IMPL_ArenaAlloc M_ArenaAllocDefault
#define MD_IMPL_ArenaRelease M_ArenaRelease
#define MD_IMPL_ArenaGetPos M_ArenaGetPos
#define MD_IMPL_ArenaPush M_ArenaPush
#define MD_IMPL_ArenaPopTo M_ArenaSetPosBack
#define MD_IMPL_ArenaSetAutoAlign M_ArenaSetAutoAlign
#define MD_IMPL_ArenaMinPos sizeof(M_Arena)
#define MD_IMPL_ArenaHeaderSize 0

#include "base/base_inc.h"
#include "third_party/metadesk/md.h"
#include "base/md_helpers.h"
#include "os/os_inc.h"
#include "render/render.h"
#include "font_provider/font_provider.h"
#include "draw/draw.h"
#include "ui/ui_inc.h"
#include "app/app_gfx.h"
#include "config/config.h"
#include "client/client.h"
#include "client/commands.h"
#include "client/theme.h"

#endif // ENGINE_FULL_H
