#include "snort.h"

#ifndef __SPO_EXPORT_PKT_H__
#define __SPO_EXPORT_PKT_H__

typedef struct _list_item {
  Packet * packet;
  struct _list_item * next;
} list_item;

typedef struct _ExportPktData {

  list_item ** new_item;

} ExportPktData;

/* list of function prototypes for this preprocessor */
void SetupExportPkt();
void ExportPktInit(u_char *);
void ExportPkt(Packet *);
void ExportPktCleanExit(int, void *);
void ExportPktRestart(int, void *);

#endif  /* __SPO_EXPORT_PKT_H__ */
