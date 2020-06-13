

#include "TscTrz.hh"


#define TscTraceORB_dispatch_impl

unsigned char TscTraceORB_dispatch::dispatch (CORBA::Request &IT_r, 
 unsigned char IT_isTarget, void *IT_pp) {
    if (!IT_pp)
       IT_pp = m_obj;
    const char *IT_s = IT_r.getOperation ();
    if (!strcmp(IT_s,"TraceORB")) {
        CORBA::Environment IT_env (IT_r);
        CORBA::Filter* IT_f = CORBA::Orbix.getFilter ();
        if (!IT_r.tcAssert ("\
Ro~TraceORB~+trazas{0},+bToScreen{b},+bToFile{b},>{v},O{}\
"))
            return 1;
        char * trazas;

        IT_r.decodeStringOp(trazas);

        CORBA::Boolean bToScreen;

        IT_r >> bToScreen;

        CORBA::Boolean bToFile;

        IT_r >> bToFile;

        if (IT_f && !IT_r.isException (IT_env))
        	IT_f->inRequestPostM (IT_r, IT_env);
        if (!IT_r.isException (IT_env)) {
            try {
                ((TscTraceORB*)IT_pp)->TraceORB (  trazas,   bToScreen,   bToFile, IT_env);

            } catch (CORBA::SystemException &IT_exSys) {
            IT_env.propagate (&IT_exSys);
            } catch (CORBA::UserException &IT_exUsr) {
            IT_env.propagate (&IT_exUsr);
            }
        }
        CORBA::string_free (trazas);


        IT_r.replyNoResults (CORBA::Flags(CORBA::INV_NO_RESPONSE),IT_env);


        return 1;
    }

    if (IT_isTarget)
        IT_r.makeRuntimeException2 ();

    return 0;
}

#include "TscTrzC.CPP"

