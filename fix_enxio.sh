#!/bin/bash
cat << 'DIFF' > temp.patch
--- interface/mmal/mmal_types.h
+++ interface/mmal/mmal_types.h
@@ -51,6 +51,7 @@
    MMAL_EINVAL,                      /**< Argument is invalid */
    MMAL_ENOSYS,                      /**< Function not implemented */
    MMAL_ENOENT,                      /**< No such file or directory */
+   MMAL_ENXIO,                       /**< No such device or address */
    MMAL_EIO,                         /**< I/O error */
    MMAL_ESPIPE,                      /**< Illegal seek */
    MMAL_EILSEQ,                      /**< Illegal byte sequence */
DIFF
patch -p0 < temp.patch
rm temp.patch
