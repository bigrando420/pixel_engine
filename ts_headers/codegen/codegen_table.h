#ifndef METAPROGRAM_TABLE_H
#define METAPROGRAM_TABLE_H

typedef enum CG_TableOp
{
    CG_TableOp_Null,
    
    CG_TableOp_BeginStringOps,
    CG_TableOp_Dot,
    CG_TableOp_Bump,
    CG_TableOp_CheckIfTrue,
    CG_TableOp_Concat,
    CG_TableOp_EndStringOps,
    
    CG_TableOp_BeginNumericOps,
    CG_TableOp_Equal,
    CG_TableOp_IsNotEqual,
    CG_TableOp_BooleanAnd,
    CG_TableOp_BooleanOr,
    CG_TableOp_EndNumericOps,
    
    CG_TableOp_COUNT
}
CG_TableOp;

typedef struct CG_NodeArray CG_NodeArray;
struct CG_NodeArray
{
    MD_u64 count;
    MD_Node **v;
};

typedef struct CG_NodeGrid CG_NodeGrid;
struct CG_NodeGrid
{
    CG_NodeArray cells;
    CG_NodeArray row_parents;
};

typedef enum CG_ColumnKind
{
    CG_ColumnKind_Default,
    CG_ColumnKind_CheckForTag,
    CG_ColumnKind_COUNT
}
CG_ColumnKind;

typedef struct CG_ColumnDesc CG_ColumnDesc;
struct CG_ColumnDesc
{
    CG_ColumnKind kind;
    MD_String8 name;
    MD_String8 tag_string;
};

typedef struct CG_TableHeader CG_TableHeader;
struct CG_TableHeader
{
    MD_u64 column_count;
    CG_ColumnDesc *column_descs;
};

typedef struct CG_ExpandIter CG_ExpandIter;
struct CG_ExpandIter
{
    CG_ExpandIter *next;
    CG_NodeGrid *grid;
    CG_TableHeader *header;
    MD_String8 label;
    MD_u64 idx;
    MD_u64 count;
};

typedef struct CG_ExpandInfo CG_ExpandInfo;
struct CG_ExpandInfo
{
    MD_String8 strexpr;
    CG_ExpandIter *first_expand_iter;
    MD_ExprOprTable expr_op_table;
};

static CG_NodeArray CG_NodeArrayMake(MD_u64 count);
static CG_NodeGrid CG_GridFromNode(MD_Node *node);
static CG_TableHeader CG_TableHeaderFromTag(MD_Node *tag);
static MD_u64 CG_RowChildIndexFromColumnName(CG_TableHeader *header, MD_String8 column_name);
static MD_i64 CG_TableExprEvaluate_Numeric(CG_ExpandInfo *info, MD_Expr *expr);
static void CG_TableExprEvaluate_String(CG_ExpandInfo *info, MD_Expr *expr, MD_String8List *out);
static void CG_LoopExpansionDimension(CG_ExpandIter *it, CG_ExpandInfo *info, MD_String8List *out);
static MD_String8List CG_GenStringListFromNode(MD_ExprOprTable expr_op_table, MD_Node *gen);
static void CG_TBL_Generate(MD_Node *file_list);

#endif // METAPROGRAM_TABLE_H
