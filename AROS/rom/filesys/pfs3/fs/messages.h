extern CONST UBYTE AFS_WARNING_MEMORY_MASK[];
extern CONST UBYTE AFS_ERROR_EXNEXT_FAIL[];
extern CONST UBYTE AFS_ERROR_DOSLIST_ADD[];
extern CONST UBYTE AFS_ERROR_READ_DELDIR[];
extern CONST UBYTE AFS_ERROR_DELDIR_INVALID[];
extern CONST UBYTE AFS_ERROR_READ_OUTSIDE[];
extern CONST UBYTE AFS_ERROR_READ_ERROR[];
extern CONST UBYTE AFS_ERROR_SEEK_OUTSIDE[];
extern CONST UBYTE AFS_ERROR_WRITE_ERROR[];
extern CONST UBYTE AFS_ERROR_WRITE_OUTSIDE[];
extern CONST UBYTE AFS_ERROR_EX_NEXT_FAIL[];
extern CONST UBYTE AFS_ERROR_NEWDIR_ADDLISTENTRY[];
extern CONST UBYTE AFS_ERROR_CACHE_INCONSISTENCY[];
extern CONST UBYTE AFS_ERROR_LOAD_DIRBLOCK_FAIL[];
extern CONST UBYTE AFS_ERROR_DNV_WRONG_DIRID[];
extern CONST UBYTE AFS_ERROR_DNV_LOAD_DIRBLOCK[];
extern CONST UBYTE AFS_ERROR_LRU_UPDATE_FAIL[];
extern CONST UBYTE AFS_ERROR_OUT_OF_BUFFERS[];
extern CONST UBYTE AFS_ERROR_DNV_WRONG_BMID[];
extern CONST UBYTE AFS_ERROR_DNV_WRONG_INDID[];
extern CONST UBYTE AFS_ERROR_UPDATE_FAIL[];
extern CONST UBYTE AFS_ERROR_DNV_ALLOC_INFO[];
extern CONST UBYTE AFS_ERROR_DNV_ALLOC_BLOCK[];
extern CONST UBYTE AFS_ERROR_DNV_WRONG_ANID[];
extern CONST UBYTE AFS_ERROR_MEMORY_POOL[];
extern CONST UBYTE AFS_ERROR_PLEASE_FREE_MEM[];
extern CONST UBYTE AFS_ERROR_LIBRARY_PROBLEM[];
extern CONST UBYTE AFS_ERROR_INIT_FAILED[];
extern CONST UBYTE AFS_ERROR_UNSLEEP[];
extern CONST UBYTE AFS_ERROR_DISK_TOO_LARGE[];
extern CONST UBYTE AFS_ERROR_ANODE_ERROR[];
extern CONST UBYTE AFS_ERROR_ANODE_INIT[];
extern CONST UBYTE AFS_ERROR_32BIT_ACCESS_ERROR[];

#if VERSION23
extern CONST UBYTE AFS_ERROR_READ_EXTENSION[];
extern CONST UBYTE AFS_ERROR_EXTENSION_INVALID[];
#endif

/* beta messages */
extern CONST UBYTE AFS_BETA_WARNING_1[];
extern CONST UBYTE AFS_BETA_WARNING_2[];
extern CONST UBYTE FORMAT_MESSAGE[];

#if DELDIR
extern CONST UBYTE deldirname[];
#endif
