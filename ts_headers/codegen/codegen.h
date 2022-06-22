#ifndef METAPROGRAM_H
#define METAPROGRAM_H

typedef struct CG_FilePair CG_FilePair;
struct CG_FilePair
{
    MD_String8 src_filename;
    FILE *h;
    FILE *c;
};

////////////////////////////////
//~ rjf: Helpers

static CG_FilePair CG_FilePairFromNode(MD_Node *node);
static void CG_CloseAllFiles(void);
static void CG_GenerateMultilineStringAsCLiteral(FILE *file, MD_String8 string);
static MD_String8 CG_EscapedFromString(MD_Arena *arena, MD_String8 string);

////////////////////////////////
//~ rjf: Globals

static MD_Arena *cg_arena = 0;
static int cg_file_pair_count = 0;
static CG_FilePair cg_file_pairs[4096] = {0};

#endif // METAPROGRAM_H
