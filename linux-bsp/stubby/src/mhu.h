#ifdef MB86S70_SHM_FROM_SCB

#define SHM_OFFSET 0x800

#define cmd_from_scb ((void *)MB86S70_SHM_FROM_SCB + SHM_OFFSET)
#define rsp_from_scb ((void *)(MB86S70_SHM_FROM_SCB + SHM_OFFSET + 0x100))
#define cmd_to_scb ((void *)(MB86S70_SHM_FROM_SCB + SHM_OFFSET + 0x200))
#define rsp_to_scb ((void *)(MB86S70_SHM_FROM_SCB + SHM_OFFSET + 0x300))

#define INTR_STAT_OFS  0
#define INTR_SET_OFS   8
#define INTR_CLR_OFS   0x10

#define MHU_SCFG       0x400

extern int mhu_send(u32 cmd);
extern void mhu_show_version(void);

#else

static inline void mhu_show_version(void) { }

#endif

