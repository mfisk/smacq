#include <fields.h>
#include <glib.h>
#include <smacq.h>
#include <sys/uio.h>
#include <stdio.h>

void fields_init(smacq_environment * env, struct fieldset * fieldset, int argc, char ** argv) {
  fieldset->fields = g_new(struct field, argc);

  while (argc-- > 0) {
    if (argv[0][0] == '-') {
      fprintf(stderr, "Unknown argument: %s\n", argv[0]);
      argv++;
      continue;
    } else {
      fieldset->fields[fieldset->num].name = argv[0];
      fieldset->fields[fieldset->num++].num = flow_requirefield(env, argv[0]);
      argv++;
    }
  }

  fieldset->currentdata = g_new(dts_object, fieldset->num);
}

dts_object * fields2data(smacq_environment * env,
			  const dts_object * datum, 
			  struct fieldset * fieldset) {
  dts_object * datalist = fieldset->currentdata;
  int i;

  for (i = 0; i < fieldset->num; i++) {
    struct field * f = &fieldset->fields[i];

    if (! flow_getfield(env, datum, f->num, &datalist[i])) {
      //fprintf(stderr, "Debug: uniq: no %s field in this datum of type %s\n", f->name, env->typename_bynum(datum->type));
      return 0;
    }

    //fprintf(stderr, "Field %s len %d\n", f->name, f->len);
  }

  return datalist;
}

struct iovec * fields2vec(smacq_environment * env,
			  const dts_object * datum, 
			  struct fieldset * fieldset) {
  struct iovec * exlist;
  int i;

  if (!fields2data(env, datum, fieldset)) 
	  return 0;

  if (!fieldset->currentvecs) 
  	fieldset->currentvecs = g_new(struct iovec, fieldset->num);

  exlist = fieldset->currentvecs;

  for (i = 0; i < fieldset->num; i++) {
    exlist[i].iov_len = fieldset->currentdata[i].len;
    exlist[i].iov_base = fieldset->currentdata[i].data;
  }

  return exlist;
}

