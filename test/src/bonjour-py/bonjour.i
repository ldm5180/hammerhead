/* This was pulled from 
http://www.mcs.anl.gov/fl/research/accessgrid/bonjour-py/bonjour-py-0.3.tar.gz 

This product includes software developed by and/or derived
from the Access Grid Project (http://www.accessgrid.org) to
which the U.S. Government retains certain rights.

*/

%module bonjour

/* rename wrapper functions to bonjour standard names */
%rename(DNSServiceProcessResult) _DNSServiceProcessResult;
/* extern DNSServiceErrorType _DNSServiceProcessResult(SRStruct *srstruct); */

%rename(DNSServiceRefSockFD) _DNSServiceRefSockFD;
/* extern int _DNSServiceRefSockFD(SRStruct *srstruct); */

%rename(DNSServiceRefDeallocate) _DNSServiceRefDeallocate;
/* extern void _DNSServiceRefDeallocate (SRStruct *srstruct); */

%{
/* Includes the header in the wrapper code */
#include <dns_sd.h> 

typedef struct {
    void *func;
    void *userdata;
    } CallbackStruct;

typedef struct {
  DNSServiceRef *ref;
  CallbackStruct *cb;
} SRStruct;

/*
DNSServiceErrorType raise_err(int err)
{
    int ret = -1;
    switch(err) {
        case kDNSServiceErr_NoError:
            ret = 0;
            break;
        case kDNSServiceErr_Unknown:
            PyErr_SetString(PyExc_Exception, "unknown bonjour error");
            break;
        case kDNSServiceErr_NoSuchName:
            PyErr_SetString(PyExc_LookupError, "No such name");
            break;
        case kDNSServiceErr_NoMemory:
            PyErr_SetString(PyExc_MemoryError, "Memory Error");
            break;
        case kDNSServiceErr_BadParam:
            PyErr_SetString(PyExc_AttributeError, "Bad Param");
            break;
        case kDNSServiceErr_BadReference:
            PyErr_SetString(PyExc_AttributeError, "Bad Reference");
            break;
        case kDNSServiceErr_BadState:
            PyErr_SetString(PyExc_StandardError, "Bad State");
            break;
        case kDNSServiceErr_BadFlags:
            PyErr_SetString(PyExc_AttributeError, "Bad Flags");
            break;
        case kDNSServiceErr_Unsupported:
            PyErr_SetString(PyExc_NotImplementedError, "Unsupported");
            break;
        case kDNSServiceErr_NotInitialized:
            PyErr_SetString(PyExc_StandardError, "Not Initialized");
            break;
        case kDNSServiceErr_AlreadyRegistered:
            PyErr_SetString(PyExc_StandardError, "Already Registered");
            break;
        case kDNSServiceErr_NameConflict:
            PyErr_SetString(PyExc_StandardError, "Name Conflict");
            break;
        case kDNSServiceErr_Invalid:
            PyErr_SetString(PyExc_StandardError, "Invalid");
            break;
        case kDNSServiceErr_Firewall:
            PyErr_SetString(PyExc_StandardError, "Firewall");
            break;
        case kDNSServiceErr_Incompatible:
            PyErr_SetString(PyExc_StandardError, "Incompatible");
            break;
        case kDNSServiceErr_BadInterfaceIndex:
            PyErr_SetString(PyExc_StandardError, "Bad Interface Index");
            break;
        case kDNSServiceErr_Refused:
            PyErr_SetString(PyExc_StandardError, "Refused");
            break;
        case kDNSServiceErr_NoSuchRecord:
            PyErr_SetString(PyExc_StandardError, "No Such Record");
            break;
        case kDNSServiceErr_NoAuth:
            PyErr_SetString(PyExc_StandardError, "No Auth");
            break;
        case kDNSServiceErr_NoSuchKey:
            PyErr_SetString(PyExc_StandardError, "No Such Key");
            break;
        case kDNSServiceErr_NATTraversal:
            PyErr_SetString(PyExc_StandardError, "NATTraversal");
            break;
        case kDNSServiceErr_DoubleNAT:
            PyErr_SetString(PyExc_StandardError, "Double NAT");
            break;
        case kDNSServiceErr_BadTime:
            PyErr_SetString(PyExc_StandardError, "Bad Time");
            break;
        default:
            PyErr_SetString(PyExc_Exception, "unknown bonjour error");
            break;
    }
    return ret;
}
*/

DNSServiceRef * __AllocateDNSServiceRef(void)
{
    DNSServiceRef * ref = (DNSServiceRef*)malloc(sizeof(DNSServiceRef));
    return ref;
}

SRStruct *
AllocateDNSServiceRef(void)
{
  /* allocate a service record (just some memory) */
  DNSServiceRef *ref = __AllocateDNSServiceRef();
  
  /* allocate one of our record structures,
     which includes slots for the callbacks */
  SRStruct *srstruct = (SRStruct *) malloc( sizeof(SRStruct) );
  
  /* save the reference */
  srstruct->ref = ref;

  /* no callbacks yet! */
  srstruct->cb = NULL;

  return srstruct;
}

void
_DNSServiceRefDeallocate(SRStruct *srstruct) 
{
  /* deallocate everything */
  DNSServiceRef *ref;
  CallbackStruct *cb;
  void *func;
  void *userdata;

  if (!srstruct)
    return;

  ref = srstruct->ref;
  cb = srstruct->cb;

  if (ref) {
    DNSServiceRefDeallocate(*ref);
    
    /* free the object; DNSServiceRefDeallocate doesn't free anything. */
    free(ref);
  }

  srstruct->ref = NULL;

  if (cb) {
    func = cb->func;
    userdata = cb->userdata;
    Py_XDECREF((PyObject *) func);
    Py_XDECREF((PyObject *) userdata);
    
    /* zero everything out for good measure */
    cb->func = NULL;
    cb->userdata = NULL;

    /* free */
    free(srstruct->cb);    
  }

  srstruct->cb = NULL;

  free(srstruct);

  srstruct = NULL;

  return;
}

DNSServiceErrorType 
_DNSServiceProcessResult(SRStruct *srstruct) {
  DNSServiceRef *sdRef;
  if (!srstruct) {
    PyErr_SetString(PyExc_AttributeError, "sdRef argument is invalid");
    return (DNSServiceErrorType) kDNSServiceErr_Invalid;
  }
  sdRef = srstruct->ref;
  return DNSServiceProcessResult(*sdRef);
}

int 
_DNSServiceRefSockFD(SRStruct *srstruct) {
  DNSServiceRef *sdRef;
  if (!srstruct) {
    PyErr_SetString(PyExc_AttributeError, "sdRef argument is invalid");
    return (DNSServiceErrorType) kDNSServiceErr_Invalid;
  }

  sdRef = srstruct->ref;
  return DNSServiceRefSockFD(*sdRef);
}


%}

%include typemaps.i
typedef unsigned short int uint16_t;
typedef long int32_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;
typedef uint32_t DNSServiceFlags;
typedef int32_t DNSServiceErrorType;

%inline %{
  /* put this through the SWIG parser as well (i.e., everything
     here gets exported */

#if defined(_WIN32)
#define DNSSD_API __stdcall
#else
#define DNSSD_API
#endif


static void DNSSD_API
service_register_callback
    (
    DNSServiceRef                       sdRef,
    DNSServiceFlags                     flags,
    DNSServiceErrorType                 errorCode,
    const char                          *name,
    const char                          *regtype,
    const char                          *domain,
    void                                *context
    ) 
    {
    PyObject *func, *userdata, *arglist, *result;
    
    CallbackStruct *cbStruct = (CallbackStruct *)context;
    func = (PyObject*)cbStruct->func;
    userdata = (PyObject*)cbStruct->userdata;

    #ifdef DEBUG
    fprintf(stderr,"In service register callback %x\n", &sdRef);
    fprintf(stderr,"  callback = %x\n", func);
    fprintf(stderr,"  userdata = %x\n", userdata);
    #endif
    
    /* don't pass back sdref through Python; this will cause
       a segmentation fault at best. Let's return None instead. */
    
    Py_INCREF(Py_None);
    arglist = Py_BuildValue("(OiisssO)", Py_None, flags, errorCode, name,
			    regtype, domain, userdata);

    if (PyErr_Occurred() != NULL){
	    PyErr_Print();
	    Py_XDECREF(arglist);
	    Py_DECREF(Py_None);
	    return;
    } 		

    result = PyObject_CallObject(func, arglist);

    /* we don't care about the result nor the argument list */
    Py_DECREF(arglist);
    Py_XDECREF(result);
    Py_DECREF(Py_None);

    if (PyErr_Occurred() != NULL){
	    PyErr_Print();
    } 		

    #ifdef DEBUG
    fprintf(stderr,"Exiting service register callback\n");
    #endif
    }

DNSServiceErrorType
pyDNSServiceRegister
    (
     /*    DNSServiceRef                       *sdRef, */
    SRStruct                            *srstruct,
    DNSServiceFlags                     flags,
    uint32_t                            interfaceIndex,
    const char                          *name,         /* may be NULL */
    const char                          *regtype,
    const char                          *domain,       /* may be NULL */
    const char                          *host,         /* may be NULL */
    uint16_t                            port,
    uint16_t                            txtLen,
    const char                          *txtRecord,    /* may be NULL */
    PyObject                            *callback,     /* may be NULL */
    PyObject                            *context
    )
    {
    DNSServiceRef *sdRef;
    DNSServiceErrorType ret;
    CallbackStruct *cbStruct;

      if (!srstruct) {
	PyErr_SetString(PyExc_AttributeError, "sdRef argument is invalid");
	return (DNSServiceErrorType) kDNSServiceErr_Invalid;
      }
      
    sdRef = srstruct->ref;

    cbStruct = (CallbackStruct*)malloc(sizeof(CallbackStruct));
    Py_XINCREF(callback);
    Py_XINCREF(context);
    cbStruct->func = callback;
    cbStruct->userdata = context;

    /* check the old callback */
    if (srstruct->cb) {
      Py_XDECREF((PyObject *) (srstruct->cb)->func);
      Py_XDECREF((PyObject *) (srstruct->cb)->userdata);
      free (srstruct->cb);
    }

    /* save the cbStruct */
    srstruct->cb = cbStruct;

    ret = DNSServiceRegister( sdRef, flags, interfaceIndex,
                              name, regtype, domain,
                              host, port, txtLen, txtRecord,
                              (DNSServiceRegisterReply)service_register_callback,
                              (void *)cbStruct); 

    return ret;
    }


static void DNSSD_API
service_browse_callback
    (
    DNSServiceRef                       sdRef,
    DNSServiceFlags                     flags,
    uint32_t                            interfaceIndex,
    DNSServiceErrorType                 errorCode,
    const char                          *serviceName,
    const char                          *regtype,
    const char                          *replyDomain,
    void                                *context
    )
    {
    PyObject *func, *userdata, *arglist, *result;
    
    CallbackStruct *cbStruct = (CallbackStruct *)context;
    func = (PyObject*)cbStruct->func;
    userdata = (PyObject*)cbStruct->userdata;
        
    #ifdef DEBUG
    fprintf(stderr,"In service browse callback %x\n", &sdRef);
    #endif

    Py_INCREF(Py_None);
    arglist = Py_BuildValue("(OiiisssO)", Py_None, flags, interfaceIndex,
			    errorCode, serviceName, regtype,
			    replyDomain, userdata);

    if (PyErr_Occurred() != NULL){
	    PyErr_Print();
	    Py_XDECREF(arglist);
	    Py_DECREF(Py_None);
	    return;
    } 		

    result = PyObject_CallObject(func, arglist);

    Py_DECREF(arglist);
    Py_XDECREF(result);
    Py_DECREF(Py_None);

    if (PyErr_Occurred() != NULL){
	    PyErr_Print();
    } 		

    #ifdef DEBUG
    fprintf(stderr,"Exiting service browse callback\n");
    #endif
    }    


DNSServiceErrorType 
pyDNSServiceBrowse
    (
     /*    DNSServiceRef                       *sdRef, */
    SRStruct                            *srstruct,
    DNSServiceFlags                     flags,
    uint32_t                            interfaceIndex,
    const char                          *regtype,
    const char                          *domain,    /* may be NULL */
    PyObject                            *callback,
    PyObject                            *context
    )
    {
    DNSServiceRef *sdRef;
    DNSServiceErrorType ret;
    CallbackStruct *cbStruct;

      if (!srstruct) {
	PyErr_SetString(PyExc_AttributeError, "sdRef argument is invalid");
	return (DNSServiceErrorType) kDNSServiceErr_Invalid;
      }
      
    sdRef = srstruct->ref;
    cbStruct = (CallbackStruct*)malloc(sizeof(CallbackStruct));
    Py_XINCREF(callback);
    Py_XINCREF(context);
    cbStruct->func = callback;
    cbStruct->userdata = context;

    /* check the old callback */
    if (srstruct->cb) {
      Py_XDECREF((PyObject *) (srstruct->cb)->func);
      Py_XDECREF((PyObject *) (srstruct->cb)->userdata);
      free (srstruct->cb);
    }

    /* save the cbStruct */
    srstruct->cb = cbStruct;

    ret = DNSServiceBrowse( sdRef, flags, interfaceIndex,
                            regtype, domain,
                            (DNSServiceBrowseReply)service_browse_callback,
                            (void *)cbStruct);
    return ret;
    }


static void DNSSD_API
service_resolve_callback
    (
    DNSServiceRef                       sdRef,
    DNSServiceFlags                     flags,
    uint32_t                            interfaceIndex,
    DNSServiceErrorType                 errorCode,
    const char                          *fullname,
    const char                          *hosttarget,
    uint16_t                            port,
    uint16_t                            txtLen,
    const char                          *txtRecord,
    void                                *context
    )
    {
    PyObject *func, *userdata, *arglist, *result;
    CallbackStruct *cbStruct = (CallbackStruct *)context;
    func = (PyObject*)cbStruct->func;
    userdata = (PyObject*)cbStruct->userdata;
    
    #ifdef DEBUG
    fprintf(stderr,"In service resolve callback\n");
    #endif

    Py_INCREF(Py_None);

    arglist = Py_BuildValue("(OiiissiisO)", Py_None, flags, interfaceIndex,
			    errorCode, fullname, hosttarget,
			    port, txtLen, txtRecord, userdata);

    if (PyErr_Occurred() != NULL){
	    PyErr_Print();
	    Py_XDECREF(arglist);
	    Py_DECREF(Py_None);
	    return;
    } 		
    
    result = PyObject_CallObject(func, arglist);

    Py_DECREF(arglist);
    Py_XDECREF(result);
    Py_DECREF(Py_None);

    if (PyErr_Occurred() != NULL){
	    PyErr_Print();
    } 		

    #ifdef DEBUG
    fprintf(stderr,"Exiting service resolve callback\n");
    #endif
    }
    
DNSServiceErrorType 
pyDNSServiceResolve
    (
     /*    DNSServiceRef                       *sdRef, */
    SRStruct                            *srstruct,
    DNSServiceFlags                     flags,
    uint32_t                            interfaceIndex,
    const char                          *name,
    const char                          *regtype,
    const char                          *domain,
    PyObject                            *callback,
    PyObject                            *context
    )
    {
    DNSServiceRef *sdRef;
    DNSServiceErrorType ret;
    CallbackStruct *cbStruct;

	if (!srstruct) {
		PyErr_SetString(PyExc_AttributeError, "sdRef argument is invalid");
		return (DNSServiceErrorType) kDNSServiceErr_Invalid;
    }
      
    sdRef = srstruct->ref;
    cbStruct = (CallbackStruct*)malloc(sizeof(CallbackStruct));
    Py_XINCREF(callback);
    Py_XINCREF(context);
    cbStruct->func = callback;
    cbStruct->userdata = context;

    /* check the old callback */
    if (srstruct->cb) {
      Py_XDECREF((PyObject *) (srstruct->cb)->func);
      Py_XDECREF((PyObject *) (srstruct->cb)->userdata);
      free (srstruct->cb);
    }

    /* save the cbStruct */
    srstruct->cb = cbStruct;

    ret = DNSServiceResolve( sdRef, flags, interfaceIndex,
                             name, regtype, domain,
                             (DNSServiceResolveReply)service_resolve_callback,
                             (void *)cbStruct);
    return ret;
    }

static void DNSSD_API
service_query_callback
    (
    DNSServiceRef                       sdRef,
    DNSServiceFlags                     flags,
    uint32_t                            interfaceIndex,
    DNSServiceErrorType                 errorCode,
    const char                          *fullname,
    uint16_t                            rrtype,
    uint16_t                            rrclass,
    uint16_t                            rdlen,
    const void                          *rdata,
    uint32_t                            ttl,
    void                                *context
    )
    {
    PyObject *func, *userdata, *arglist, *result;
    CallbackStruct *cbStruct = (CallbackStruct *)context;
    func = (PyObject*)cbStruct->func;
    userdata = (PyObject*)cbStruct->userdata;
    
    #ifdef DEBUG
        fprintf(stderr,"In service resolve callback\n");
    #endif
    
    Py_INCREF(Py_None);

    arglist = Py_BuildValue("(OiiisiiisiO)", Py_None, flags, interfaceIndex,
			    errorCode, fullname, rrtype,
			    rrclass, rdlen, (char*) rdata,
			    ttl, userdata);

    if (PyErr_Occurred() != NULL){
	    PyErr_Print();
	    Py_XDECREF(arglist);
	    Py_DECREF(Py_None);
	    return;
    } 		

    result = PyObject_CallObject(func, arglist);
    
    Py_DECREF(arglist);
    Py_XDECREF(result);
    Py_DECREF(Py_None);

    if (PyErr_Occurred() != NULL){
	    PyErr_Print();
    } 		

    #ifdef DEBUG
    fprintf(stderr,"Exiting service resolve callback\n");
    #endif
    }
    
DNSServiceErrorType 
pyDNSServiceQueryRecord
    (
     /*    DNSServiceRef   *sdRef, */
    SRStruct        *srstruct,
    DNSServiceFlags flags,
    uint32_t        interfaceIndex,
    const char      *fullname,
    uint16_t        rrtype,
    uint16_t        rrclass,
    PyObject        *callback,
    PyObject        *context
    )
    {
    DNSServiceRef *sdRef;
    DNSServiceErrorType ret;
    CallbackStruct *cbStruct;
    if (!srstruct) {
	    PyErr_SetString(PyExc_AttributeError, "sdRef argument is invalid");
	    return (DNSServiceErrorType) kDNSServiceErr_Invalid;
    }
      
    sdRef = srstruct->ref;
    cbStruct = (CallbackStruct*)malloc(sizeof(CallbackStruct));
    Py_XINCREF(callback);
    Py_XINCREF(context);
    cbStruct->func = callback;
    cbStruct->userdata = context;

    /* check the old callback */
    if (srstruct->cb) {
      Py_XDECREF((PyObject *) (srstruct->cb)->func);
      Py_XDECREF((PyObject *) (srstruct->cb)->userdata);
      free (srstruct->cb);
    }

    /* save the cbStruct */
    srstruct->cb = cbStruct;

    ret = DNSServiceQueryRecord( sdRef, flags, interfaceIndex,
                                 fullname, rrtype, rrclass,
                                 (DNSServiceQueryRecordReply)service_query_callback,
                                 (void *)cbStruct);
    return ret;
    }
    
%}

extern enum
    {
    kDNSServiceFlagsMoreComing         ,
    kDNSServiceFlagsAdd                ,
    kDNSServiceFlagsDefault            ,
    kDNSServiceFlagsNoAutoRename       ,
    kDNSServiceFlagsShared             ,
    kDNSServiceFlagsUnique             ,
    kDNSServiceFlagsBrowseDomains      ,
    kDNSServiceFlagsRegistrationDomains,
    };

extern enum
    {
    kDNSServiceErr_NoError                            ,
    kDNSServiceErr_Unknown                            ,
    kDNSServiceErr_NoSuchName                         ,
    kDNSServiceErr_NoMemory                           ,
    kDNSServiceErr_BadParam                           ,
    kDNSServiceErr_BadReference                       ,
    kDNSServiceErr_BadState                           ,
    kDNSServiceErr_BadFlags                           ,
    kDNSServiceErr_Unsupported                        ,
    kDNSServiceErr_NotInitialized                     ,
    kDNSServiceErr_AlreadyRegistered                  ,
    kDNSServiceErr_NameConflict                       ,
    kDNSServiceErr_Invalid                            ,
    kDNSServiceErr_Incompatible                       ,
    /* mDNS Error codes are in the range
     * FFFE FF00 (-65792) to FFFE FFFF (-65537) */
    };


extern enum
    {
    kDNSServiceClass_IN
    };

extern enum
    {
    kDNSServiceType_A,      /* Host address. */
    kDNSServiceType_NS,      /* Authoritative server. */
    kDNSServiceType_MD,      /* Mail destination. */
    kDNSServiceType_MF,      /* Mail forwarder. */
    kDNSServiceType_CNAME,      /* Canonical name. */
    kDNSServiceType_SOA,      /* Start of authority zone. */
    kDNSServiceType_MB,      /* Mailbox domain name. */
    kDNSServiceType_MG,      /* Mail group member. */
    kDNSServiceType_MR,      /* Mail rename name. */
    kDNSServiceType_NULL,     /* Null resource record. */
    kDNSServiceType_WKS,     /* Well known service. */
    kDNSServiceType_PTR,     /* Domain name pointer. */
    kDNSServiceType_HINFO,     /* Host information. */
    kDNSServiceType_MINFO,     /* Mailbox information. */
    kDNSServiceType_MX,     /* Mail routing information. */
    kDNSServiceType_TXT,     /* One or more text strings. */
    kDNSServiceType_RP,     /* Responsible person. */
    kDNSServiceType_AFSDB,     /* AFS cell database. */
    kDNSServiceType_X25,     /* X_25 calling address. */
    kDNSServiceType_ISDN,     /* ISDN calling address. */
    kDNSServiceType_RT,     /* Router. */
    kDNSServiceType_NSAP,     /* NSAP address. */
    kDNSServiceType_NSAP_PTR,     /* Reverse NSAP lookup (deprecated). */
    kDNSServiceType_SIG,     /* Security signature. */
    kDNSServiceType_KEY,     /* Security key. */
    kDNSServiceType_PX,     /* X.400 mail mapping. */
    kDNSServiceType_GPOS,     /* Geographical position (withdrawn). */
    kDNSServiceType_AAAA,     /* Ip6 Address. */
    kDNSServiceType_LOC,     /* Location Information. */
    kDNSServiceType_NXT,     /* Next domain (security). */
    kDNSServiceType_EID,     /* Endpoint identifier. */
    kDNSServiceType_NIMLOC,     /* Nimrod Locator. */
    kDNSServiceType_SRV,     /* Server Selection. */
    kDNSServiceType_ATMA,     /* ATM Address */
    kDNSServiceType_NAPTR,     /* Naming Authority PoinTeR */
    kDNSServiceType_KX,     /* Key Exchange */
    kDNSServiceType_CERT,     /* Certification record */
    kDNSServiceType_A6,     /* IPv6 address (deprecates AAAA) */
    kDNSServiceType_DNAME,     /* Non-terminal DNAME (for IPv6) */
    kDNSServiceType_SINK,     /* Kitchen sink (experimentatl) */
    kDNSServiceType_OPT,     /* EDNS0 option (meta-RR) */
    kDNSServiceType_TKEY,    /* Transaction key */
    kDNSServiceType_TSIG,    /* Transaction signature. */
    kDNSServiceType_IXFR,    /* Incremental zone transfer. */
    kDNSServiceType_AXFR,    /* Transfer zone of authority. */
    kDNSServiceType_MAILB,    /* Transfer mailbox records. */
    kDNSServiceType_MAILA,    /* Transfer mail agent records. */
    kDNSServiceType_ANY     /* Wildcard match. */
    };


/* DNSServiceErrorType DNSServiceProcessResult(DNSServiceRef sdRef); */
/* int DNSServiceRefSockFD(DNSServiceRef sdRef); */

extern DNSServiceErrorType _DNSServiceProcessResult(SRStruct *srstruct);
extern int _DNSServiceRefSockFD(SRStruct *srstruct);

/* DNSServiceRef * AllocateDNSServiceRef(); */
/* void DNSServiceRefDeallocate(DNSServiceRef sdRef); */
SRStruct * AllocateDNSServiceRef(void);
extern void _DNSServiceRefDeallocate(SRStruct *srstruct);


void 
DNSServiceReconfirmRecord(DNSServiceFlags flags,
			  uint32_t interfaceIndex,
			  const char *fullname,
			  uint16_t rrtype,
			  uint16_t rrclass,
			  uint16_t rdlen,
			  const char *rdata);
