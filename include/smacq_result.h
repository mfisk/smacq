#ifndef SMACQ_RESULT_H
#define SMACQ_RESULT_H
template<class N, typename T = int>
class flags {
        private:
                typedef flags this_type;
                T val;
        public:
                flags<N,T>() : val(0) {}
                flags<N,T>(const T x) : val(x) {}

                flags<N,T>      operator |  (const flags<N,T> &x)       const { return val | x.val; }
                flags<N,T>      operator &  (const flags<N,T> &x)       const { return val & x.val; }
                flags<N,T>      operator |= (const flags<N,T> &x)             { return val |= x.val; }
                //int           operator () ()                          const { return val; }
                bool            operator !  ()                          const { return val == 0; }
                bool            operator == (const flags<N,T> &x)       const { return val == x.val; }
                bool            operator != (const flags<N,T> &x)       const { return val != x.val; }

                // This is crazy:
                typedef T this_type::*unspecified_bool_type;
                operator unspecified_bool_type () const { return (val == 0? 0: &this_type::val); }

};

enum _smacq_result {};

/// A smacq_result is like an enum, except that you can OR and AND them
/// like flags.
typedef flags<enum _smacq_result> smacq_result;

extern smacq_result SMACQ_NONE;
extern smacq_result SMACQ_FREE;
extern smacq_result SMACQ_PASS;
extern smacq_result SMACQ_ERROR;
extern smacq_result SMACQ_END;
extern smacq_result SMACQ_CANPRODUCE;
extern smacq_result SMACQ_PRODUCE;

/*
enum _smacq_result { SMACQ_NONE=0, SMACQ_FREE=1, SMACQ_PASS=2, SMACQ_ERROR=4, SMACQ_END=8, SMACQ_CANPRODUCE=256, SMACQ_PRODUCE=512};
typedef enum _smacq_result smacq_result;
*/
#endif
