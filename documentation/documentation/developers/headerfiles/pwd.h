#ifndef _PWD_H_
#define	_PWD_H_

/*
    Copyright � 2003-2010, The AROS Development Team. All rights reserved.
    $Id: pwd.h 32359 2010-01-16 22:25:24Z verhaegs $
*/

#include <sys/_types.h>
#include <sys/cdefs.h>

/*
    Implementation Note:
    You do not need to define size_t in this file. (POSIX)
*/

#include <sys/types/gid_t.h>
#include <sys/types/uid_t.h>

struct passwd
{
  char  *pw_name;    /* Username */
  char  *pw_passwd;  /* Password */
  uid_t  pw_uid;     /* User ID */
  gid_t  pw_gid;     /* Group ID */
  char  *pw_gecos;   /* Real name */
  char  *pw_dir;     /* Home directory  */
  char  *pw_shell;   /* Shell */
};

__BEGIN_DECLS

struct passwd *getpwuid(uid_t uid);
struct passwd *getpwnam(const char *name);

#if __POSIX_VISIBLE >= 200112 || __XSI_VISIBLE >= 500
/* NOTIMPL int getpwnam_r(const char *name, struct passwd *pwd, char *buffer,
        size_t bufsize, struct passwd **result); */
/* NOTIMPL int getpwuid_r(uid_t uid, struct passwd *pwd, char *buffer, size_t bufsize,
        struct passwd **result); */
#endif

#if __XSI_VISIBLE
void endpwent(void);
struct passwd *getpwent(void);
void setpwent(void);
#endif

__END_DECLS

#endif /* _PWD_H_ */
