#ifndef CK_MD_H
#define CK_MD_H

#ifndef CK_MD_CACHELINE
#define CK_MD_CACHELINE (64)
#endif

#ifndef CK_MD_PAGESIZE
#define CK_MD_PAGESIZE (4096)
#endif

/*
 * Minimal CK configuration for this demo application.
 * x86_64 is the host architecture in this workspace, so TSO is the
 * appropriate memory model for the concurrency kit headers.
 */
#define CK_MD_TSO

#endif /* CK_MD_H */
