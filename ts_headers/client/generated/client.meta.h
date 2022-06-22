typedef enum C_EntityKind
{
C_EntityKind_Nil,
C_EntityKind_Root,
C_EntityKind_File,
C_EntityKind_Folder,
C_EntityKind_StableTextPoint,
C_EntityKind_COUNT
}
C_EntityKind;

engine_global String8 c_g_entity_kind_string_table[5];

