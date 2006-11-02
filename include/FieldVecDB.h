#include <FieldVec.h>
#include <db.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdexcept>
/// A persistent store for per-vector state.
template <class T>
class FieldVecDB {
 public:
   /// Instantiate a database with the given name (may include path).
   FieldVecDB(const std::string & file) {
        db = dbopen(file.c_str(), (O_RDWR | O_CREAT), 0666, DB_BTREE, NULL);
        if (!db) throw std::runtime_error(strerror(errno));
   }

   ~FieldVecDB() {
	if (db) {
		if (-1 == db->close(db)) {
        		throw std::runtime_error(strerror(errno));
		}
	}
   }

   T get(const DtsObjectVec & v) {
        DtsDigest digest(v);

	DBT key;
	key.size = sizeof(DtsDigest);
	key.data = &digest;

        DBT val;
        int ret = db->get(db, &key, &val, 0);
	if (ret == -1) {
        	throw std::runtime_error(strerror(errno));
	} else if (ret == 1) {
		// Did not exist; create
                T empty;
                return empty;
 	} else {
                assert(val.size == sizeof(T));
                T cpy(*(T *)(val.data));
                return cpy;
        }
   }

   void put(const DtsObjectVec & v, T & value) {
        DtsDigest digest(v);

	DBT key;
	key.size = sizeof(DtsDigest);
	key.data = &digest;

        DBT val;
        val.size = sizeof(T);
        val.data = &value;

        int ret = db->put(db, &key, &val, 0);
        if (ret != 0) throw std::runtime_error(strerror(errno));
   }

 private:
   DB * db;

};

