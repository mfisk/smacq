#include <fields.h>
#include <glib.h>
#include <smacq.h>
#include <sys/uio.h>
#include <stdio.h>
#include <dts-internal.h>

int iovec_has_undefined(struct iovec * iov, int nvecs) {
  int i;
  for(i=0; i<nvecs; i++) {
    if (iov[i].iov_len == 0) return 1;
  }

  return 0;
}

void fieldset_destroy(struct fieldset * fieldset) {
  int i;
  for (i=0; i < fieldset->num; i++) {
  	dts_field_free(fieldset->fields[i].num);
	if (fieldset->currentdata[i]) {
		fieldset->currentdata[i]->decref();
	}
  }
  free(fieldset->fields);
  free(fieldset->currentdata);
  /* Doesn't free self */
}

void DTS::fields_init(struct fieldset * fieldset, int argc, char ** argv) {
  fieldset->fields = g_new(struct field, argc);

  while (argc-- > 0) {
    if (argv[0][0] == '-') {
      fprintf(stderr, "Unknown argument: %s\n", argv[0]);
      argv++;
      continue;
    } else {
      fieldset->fields[fieldset->num].name = argv[0];
      fieldset->fields[fieldset->num++].num = requirefield(argv[0]);
      argv++;
    }
  }

  fieldset->currentdata = g_new0(DtsObject*, fieldset->num);
}

DtsObject ** DtsObject::fields2data(struct fieldset * fieldset) {
  DtsObject ** datalist = fieldset->currentdata;
  int i;

  for (i = 0; i < fieldset->num; i++) {
    struct field * f = &fieldset->fields[i];

    // Free old field data
    if (datalist[i]) 
     datalist[i]->decref();

    datalist[i] = getfield(f->num);

    //fprintf(stderr, "Field %s len %d\n", f->name, f->len);
  }

  return datalist;
}

struct iovec * DtsObject::fields2vec(struct fieldset * fieldset) {
  struct iovec * exlist;
  int i;

  /* Initialize fieldset->currentdata[] */
  if (!fields2data(fieldset)) 
	  return NULL;

  if (!fieldset->currentvecs) 
  	fieldset->currentvecs = g_new(struct iovec, fieldset->num);

  exlist = fieldset->currentvecs;

  for (i = 0; i < fieldset->num; i++) {
    if (fieldset->currentdata[i]) {
    	exlist[i].iov_len = fieldset->currentdata[i]->getsize();
    	exlist[i].iov_base = (char*)fieldset->currentdata[i]->getdata();
    } else {
	exlist[i].iov_len = 0;
    }
  }

  return exlist;
}

