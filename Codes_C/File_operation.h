#ifndef __FILE_OPERATION_H
#define __FILE_OPERATION_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "DataType.h"

#define FILEPATHTYPE_LENGTH (80)
#define FILEPATH_LENGTH (19)

typedef char FilePathType[FILEPATHTYPE_LENGTH];

FilePathType FilePath[FILEPATH_LENGTH];

MemoryRequestType* read_csv_to_get_memory_trace(FilePathType* _filepath);

#endif