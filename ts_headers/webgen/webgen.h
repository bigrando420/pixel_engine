#ifndef WEBGEN_H
#define WEBGEN_H

typedef enum WG_ContentKind
{
    WG_ContentKind_Null,
    WG_ContentKind_Text,
    WG_ContentKind_Title,
    WG_ContentKind_Subtitle,
    WG_ContentKind_Code,
    WG_ContentKind_Link,
    WG_ContentKind_COUNT
}
WG_ContentKind;

typedef struct WG_SiteInfo WG_SiteInfo;
struct WG_SiteInfo
{
    MD_String8 title;
    MD_String8 desc;
    MD_String8 canonical_url;
    MD_String8 twitter_handle;
    MD_Map link_dictionary;
    MD_String8 header;
    MD_String8 footer;
    MD_String8 style;
};

typedef struct WG_Page WG_Page;
struct WG_Page
{
    WG_Page *next;
    MD_String8 name;
    MD_String8 title;
    MD_String8 author;
    MD_u64 year;
    MD_u64 month;
    MD_u64 day;
    MD_b32 publish;
    MD_b32 disable_toc;
    MD_Node *content_list;
};

typedef struct WG_PageList WG_PageList;
struct WG_PageList
{
    WG_Page *first;
    WG_Page *last;
    MD_u64 count;
};

typedef struct WG_AssetHash WG_AssetHash;
struct WG_AssetHash
{
    MD_u64 u64[2];
};

typedef struct WG_AssetNode WG_AssetNode;
struct WG_AssetNode
{
    WG_AssetNode *next;
    MD_String8 path;
    WG_AssetHash hash;
};

////////////////////////////////
//~ rjf: Assets

static WG_AssetHash WG_AssetHashFromData(MD_String8 data);
static WG_AssetNode *WG_AssetNodeFromPath(MD_String8 string);
static WG_AssetHash WG_AssetHashFromPath(MD_String8 path);

////////////////////////////////
//~ rjf: Parsing

static WG_SiteInfo WG_SiteInfoFromNode(MD_Node *node);
static WG_Page *WG_PageFromNode(MD_Node *node);
static void WG_PageListPush(WG_PageList *list, WG_Page *page);

////////////////////////////////
//~ rjf: Generation

static void WG_PushTextContentString_HTML(MD_String8 string, MD_String8List *out);
static void WG_PushNodeStrings_HTML(MD_Node *node, MD_String8List *out);
static void WG_PushPageStrings_HTML(WG_Page *page, MD_String8List *out);
static MD_String8 WG_HTMLStringFromPage(WG_SiteInfo *site_info, WG_Page *page);

#endif // WEBGEN_H
