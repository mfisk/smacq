#include <FieldVec.h>
#include <db_cxx.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdexcept>
/// A persistent store for per-vector state.
template <class T>
class FieldVecDB {
 public:
   /// Copy Constructor.
   FieldVecDB(const FieldVecDB<T> & d) : db(NULL, 0), filename(d.filename) {
        db.open(NULL, filename.c_str(), NULL, DB_BTREE, (O_RDWR | O_CREAT), 0666);
   }

   /// Instantiate a database with the given name (may include path).
   FieldVecDB(const std::string & file) : db(NULL, 0), filename(file) {
        db.open(NULL, filename.c_str(), NULL, DB_BTREE, (O_RDWR | O_CREAT), 0666);
   }

   ~FieldVecDB() {
	db.close(0);
   }

   void operator = (const FieldVecDB& d) {
	db.close(0);
	filename = d.filename;
        db.open(NULL, filename.c_str(), NULL, DB_BTREE, (O_RDWR | O_CREAT), 0666);
   }

   T get(const DtsObjectVec & v) {
        DtsDigest digest(v);

	Dbt key(&digest, sizeof(DtsDigest));
        Dbt val;
        int ret = db.get(NULL, &key, &val, 0);
	if (ret == -1) {
        	throw std::runtime_error("db.get returned -1");
	} else if (0 /*ret == DB::DB_NOTFOUND*/) {
		// Did not exist; create
                T empty;
                return empty;
 	} else {
                assert(val.get_size() == sizeof(T));
                T cpy(*(T *)(val.get_data()));
                return cpy;
        }
   }

   void put(const DtsObjectVec & v, T & value) {
        DtsDigest digest(v);

	Dbt key(&digest, sizeof(DtsDigest));
        Dbt val(&value, sizeof(T));

        int ret = db.put(NULL, &key, &val, 0);
        if (ret != 0) throw std::runtime_error(strerror(errno));
   }

 private:
   Db db;
   std::string filename;
};

