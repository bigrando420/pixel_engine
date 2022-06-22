#ifndef MD_HELPERS_H
#define MD_HELPERS_H

engine_function String8 Str8FromMD(MD_String8 str);
engine_function MD_String8 MDFromStr8(String8 str);
engine_function SP_Pattern SP_PatternFromMDNode(M_Arena *arena, MD_Node *node);

#endif // MD_HELPERS_H
