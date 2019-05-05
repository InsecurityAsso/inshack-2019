#include "config.h"
#include "macro.h"
#include "ddfs.h"
#include "path.h"
#include "log.h"

#include <string.h>
#include <errno.h>


$FUNCTION_PROTOTYPE
{
    log_in("", );
    int err;
    err=1;
end:
    if(err) {
        err=-errno;
    }
    log_out_fuse();
    return err;
}
