TOP = ../../../../../..

AROS_HOST_CPU    ?= i386
AROS_HOST_ARCH   ?= linux
AROS_TARGET_CPU  ?= i386
AROS_TARGET_ARCH ?= linux



include $(TOP)/config/make.cfg

STARTUP=$(LIBDIR)/startup.o

LOCALINCLUDES = -I. -I../Include

CC := $(TOOLDIR)/$(AROS_TARGET_CPU)-$(AROS_TARGET_ARCH)-aros-gcc

DEFINES =
OPTIMIZE = -O2 -g3

PRELIBS := -lautoinit -lgtlayout -lamiga -larosc_shared -lamiga -larosm -lautoinit

LIBS := -L$(LIBDIR) $(PRELIBS)

#First all intermediate files MUST be created so there is a 'defines' file!
all: $(AROS_LIBS)/gtlayout.library  $(TESTDIR)/gtlayout_test includes-copy


includes-copy: $(INCDIR)/gtlayout.h $(INCDIR)/proto/gtlayout.h $(INCDIR)/defines/gtlayout.h

$(INCDIR)/gtlayout.h : aros/gtlayout.h
	@$(ECHO) Copying $^ ...
	@$(CP) $^ $@

$(INCDIR)/defines/gtlayout.h : aros/gtlayout_defines.h
	@$(ECHO) Copying $^ ...
	@$(CP) $^ $@

$(INCDIR)/proto/gtlayout.h : aros/gtlayout_proto.h
	@$(ECHO) Copying $^ ...
	@$(CP) $^ $@

aros/%_aros.c : %.c
	@$(ECHO) Generating $@...
	@$(ARCHTOOL) -R aros/gtlayout_pragmas_aros.h  $^ $@ aros/gtlayout_defines_aros.h >/dev/null

%.o : aros/%_aros.c
	@$(ECHO) Compiling $@...
	@$(CC) $(CFLAGS) $(OPTIMIZE) $(DEFINES) $(LOCALINCLUDES) -include aros/arosspecific.h -c $^ -o $@ 

LT_DeleteHandle.ol : aros/LT_DeleteHandle_aros.c
	@$(ECHO) Compiling $@...
	@$(CC) $(CFLAGS) $(OPTIMIZE) $(DEFINES) -DLINK_LIB $(LOCALINCLUDES) -include aros/arosspecific.h -c $^ -o $@ 
	@$(OBJCOPY) $@ --redefine-sym GTLayout_LT_DeleteHandle=LT_DeleteHandle

LT_HandleInput.ol : aros/LT_HandleInput_aros.c
	@$(ECHO) Compiling $@...
	@$(CC) $(CFLAGS) $(OPTIMIZE) $(DEFINES) -DLINK_LIB $(LOCALINCLUDES) -include aros/arosspecific.h -c $^ -o $@ 
	@$(OBJCOPY) $@ --redefine-sym GTLayout_LT_HandleInput=LT_HandleInput

%.ol : aros/%_aros.c
	@$(ECHO) Compiling $@...
	@$(CC) $(CFLAGS) $(OPTIMIZE) $(DEFINES) -DLINK_LIB $(LOCALINCLUDES) -include aros/arosspecific.h -c $^ -o $@ 


test/%.o : test/%.c
	@$(ECHO) Compiling $@...
	@$(CC) $(CFLAGS) $(OPTIMIZE) $(DEFINES) $(LOCALINCLUDES) -c $^ -o $@


API_SOURCE = \
	LT_Activate \
	LT_Build \
	LT_CreateHandle \
	LT_DeleteHandle \
	LT_DisposeMenu \
	LT_FindMenuCommand \
	LT_GetAttributes \
	LT_GetCode \
	LT_GetMenuItem \
	LT_HandleInput \
	LT_IMsg \
	LT_InitExit \
	LT_LabelWidth \
	LT_LayoutMenus \
	LT_LevelWidth \
	LT_LockWindow \
	LT_MenuControlTagList \
	LT_New \
	LT_NewMenuTagList \
	LT_NewMenuTemplate \
	LT_PressButton \
	LT_Rebuild \
	LT_Refresh \
	LT_SetAttributes \
	LT_ShowWindow \
	LT_UpdateStrings


UFC_SOURCE = \
	LTP_ImageClass \
	LTP_PopupClass \
	LTP_HexEditRoutine \
	LTP_DefaultHistoryHook \
	LTP_DefaultEditRoutine \
	LTP_BackFillRoutine \
	LTP_PasswordEditRoutine

OTHERS = gtlayout_lib

INTERMEDIATES = $(foreach f,$(API_SOURCE) $(UFC_SOURCE) $(OTHERS),aros/$(f)_aros.c)

FILES = \
	LTP_AddGadgets \
	LTP_AddHistory \
	LTP_AdjustItemPosition \
	LTP_AdjustMenuPosition \
	LTP_Atol \
	LTP_BitMap \
	LTP_BlinkButton \
	LTP_CheckGlyph \
	LTP_Clone \
	LTP_ConvertNum \
	LTP_CorrectItemList \
	LTP_CreateExtraObject \
	LTP_CreateGadgets \
	LTP_CreateMenuTagList \
	LTP_CreateMenuTemplate \
	LTP_CreateObjectNode \
	LTP_Delay \
	LTP_DeleteObjectNode \
	LTP_DetermineSize \
	LTP_Draw \
	LTP_DrawBackFore \
	LTP_DrawBox \
	LTP_DrawGauge \
	LTP_DrawGroup \
	LTP_DrawIncrementer \
	LTP_DrawPalette \
	LTP_DrawPicker \
	LTP_DrawPrevNext \
	LTP_DrawTapeButton \
	LTP_FillMenu \
	LTP_Find \
	LTP_FixExtraLabel \
	LTP_GetCommandWidth \
	LTP_GetDisplayClip \
	LTP_GlyphSetup \
	LTP_HandleHistory \
	LTP_InitIText \
	LTP_LayoutGadgets \
	LTP_LayoutGroup \
	LTP_LayoutMenu \
	LTP_LevelClass \
	LTP_MakeItem \
	LTP_MakeMenu \
	LTP_Memory \
	LTP_MoveToWindow \
	LTP_NewMenu \
	LTP_PlaceGroups \
	LTP_PrintBoxLine \
	LTP_PrintLabel \
	LTP_RenderArrow \
	LTP_RenderCircle \
	LTP_Rescale \
	LTP_ResetGroups \
	LTP_RPortAttrs \
	LTP_SearchKeys \
	LTP_SelectKeys \
	LTP_ShrinkMenu \
	LTP_SizeDimensions \
	LTP_Spread \
	LTP_SPrintf \
	LTP_Storage \
	LTP_TabClass \
	min_max \
	asmpoolfunctions

LINKLIB_FILES = \
	varargfuncs

TESTFILES = test/gui test/Tools

OBJS         = $(foreach f,$(API_SOURCE) $(API_SOURCE_EXTRA) $(UFC_SOURCE) $(FILES) $(OTHERS),$(f).o)
LINKLIB_OBJS = $(foreach f,$(LINKLIB_FILES)                                                  ,$(f).ol)
TESTOBJS = test/gui.o test/Tools.o


$(AROS_LIBS)/gtlayout.library : $(INTERMEDIATES) $(OBJS) gtlayout_data.o $(AROS_LIB)/libgtlayout.a 
	@$(ECHO) Compiling $@...
	@$(CC) gtlayout_data.o $(OBJS) -static -lamiga -lgtlayout -o $@ -nostartfiles

$(AROS_LIB)/libgtlayout.a : $(LINKLIB_OBJS) gtlayout_data.ol
	@$(ECHO) Generating $@...
	@$(AR) $@ $^

$(TESTDIR)/gtlayout_test: $(TESTOBJS)
	@$(ECHO) Linking $^ to $@
	$(STATIC_LD) $(LDFLAGS) $(STARTUP) $^ -o $@ $(LIBS) $(COMPILER_LIBS)
 
clean:
	@$(RM) $(INTERMEDIATES) $(OBJS) $(LINKLIB_OBJS) $(TESTOBJS) $(AROS_LIBS)/gtlayout.library aros/gtlayout_defines_aros.h gtlayout_data.o gtlayout_data.ol $(AROS_LIB)/libgtlayout.a
