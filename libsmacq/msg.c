#include <smacq-internal.h>
#include <stdio.h>

static inline const dts_object * smacq_datum_fieldobj(const dts_object * d, int field) {
  if (field >= d->numfields) {
    return NULL;
  }
  return d->fields[field];
}

void msg_send(dts_environment * tenv, int fieldnum, dts_object * data, dts_comparison * comparisons) {
  dts_message * msg = g_new(dts_message, 1);
  dts_message * mlist = g_hash_table_lookup(tenv->messages_byfield, (void*)fieldnum);
  msg->field_data = *data;
  msg->criteria = comparisons;

  dts_incref(&msg->field_data, 1);

  if (!mlist) {
    msg->next = NULL;
    g_hash_table_insert(tenv->messages_byfield, (void*)fieldnum, msg);
  } else {
    msg->next = mlist->next;
    mlist->next = msg;
  }
}

const dts_object * msg_check(dts_environment * tenv, const dts_object * d, 
			 int field, dts_object * data) {
  dts_message * m;
  const dts_object * field_data = smacq_datum_fieldobj(d, field);

  if (field_data) {
    return field_data;
  } else {
    //fprintf(stderr, "Looking for possible messages\n");
 
    // Look for an unbound message that applies to this datum
    for (m = g_hash_table_lookup(tenv->messages_byfield, (void*)field); m; m = m->next) {
      if (type_match(tenv, d, m->criteria, 0)) {
	dts_attach_field(d, field, &m->field_data);

	// If message was ANYCAST, free it now.

	return &m->field_data;
      }
    }
  }

  return NULL;
}
