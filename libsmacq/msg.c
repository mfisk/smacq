#include <smacq-internal.h>
#include <stdio.h>

void msg_send(dts_environment * tenv, dts_field_element fieldnum, dts_object * data, dts_comparison * comparisons) {
  dts_message * msg = g_new(dts_message, 1);
  dts_message * mlist = darray_get(&tenv->messages_byfield, fieldnum);
  msg->field_data = *data;
  msg->criteria = comparisons;

  dts_incref(&msg->field_data, 1);

  if (!mlist) {
    msg->next = NULL;
    darray_set(&tenv->messages_byfield, fieldnum, msg);
  } else {
    msg->next = mlist->next;
    mlist->next = msg;
  }
}

const dts_object * msg_check(dts_environment * tenv, const dts_object * d, 
			 dts_field_element field, dts_object * data) {
  dts_message * m;
  //fprintf(stderr, "Looking for possible messages\n");
 
  // Look for an unbound message that applies to this datum
  for (m = darray_get(&tenv->messages_byfield, field); m; m = m->next) {
      if (type_match(tenv, d, m->criteria, 0)) {
	dts_attach_field_single(d, field, &m->field_data);

	// If message was ANYCAST, free it now.

	return &m->field_data;
      }
  }

  return NULL;
}
