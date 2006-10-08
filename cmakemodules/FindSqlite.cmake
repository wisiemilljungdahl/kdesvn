# try find sqlite
# found defines
# SQLITE_INCLUDE_DIR
# SQLITE_LIBRARIES
# SQLITE_FOUND

IF (SQLITE_INCLUDE_DIR AND SQLITE_LIBRARIES)
    SET(SQLITE_FOUND TRUE)
ELSE (SQLITE_INCLUDE_DIR AND SQLITE_LIBRARIES)
 INCLUDE(UsePkgConfig)
 PKGCONFIG(sqlite3 _sqliteincdir _sqlitelibdir _sqlitelinkflags _sqlitecflags)

 FIND_PATH(SQLITE_INCLUDE_DIR sqlite3.h
   ${_sqliteincdir}
   /usr/include
   /usr/local/include
  )
 FIND_LIBRARY(SQLITE_LIBRARIES NAMES sqlite3
   PATHS
   ${_sqlitelibdir}
   /usr/lib
   /usr/local/lib
 )

 IF (SQLITE_INCLUDE_DIR AND SQLITE_LIBRARIES)
   SET(SQLITE_FOUND TRUE)
 ENDIF (SQLITE_INCLUDE_DIR AND SQLITE_LIBRARIES)

 IF (SQLITE_FOUND)
   MESSAGE(STATUS "Found sqlite3: ${SQLITE_LIBRARIES}")
 ELSE (SQLITE_FOUND)
   MESSAGE(STATUS "Could not find sqlite3")
 ENDIF (SQLITE_FOUND)

 MARK_AS_ADVANCED(SQLITE_LIBRARIES SQLITE_INCLUDE_DIR)

ENDIF (SQLITE_INCLUDE_DIR AND SQLITE_LIBRARIES)
