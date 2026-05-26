#include <nuttx/config.h>
#include <syslog.h>
#include <nuttx/fs/fs.h>

#include "e3650.h"

/****************************************************************************
 * Name: e3650_bringup
 *
 * Description:
 *   Bring up board features
 *
 ****************************************************************************/

int e3650_bringup(void)
{
#if defined(CONFIG_FS_PROCFS) || defined(CONFIG_FS_TMPFS)
  int ret;
#endif

#ifdef CONFIG_FS_PROCFS
  /* Mount the procfs file system */

  ret = nx_mount(NULL, "/proc", "procfs", 0, NULL);
  if (ret < 0)
    {
      syslog(LOG_ERR, "ERROR: Failed to mount procfs at /proc: %d\n", ret);
    }
#endif

#ifdef CONFIG_FS_TMPFS
  /* Mount the tmpfs file system */

  ret = nx_mount(NULL, CONFIG_LIBC_TMPDIR, "tmpfs", 0, NULL);
  if (ret < 0)
    {
      syslog(LOG_ERR, "ERROR: Failed to mount tmpfs at %s: %d\n",
             CONFIG_LIBC_TMPDIR, ret);
    }
#endif

	return OK;
}
