
// Copyright (c) 2008-2009, Regents of the University of Colorado.
// This work was supported by NASA contracts NNJ05HE10G, NNC06CB40C, and
// NNC07CB47C.

#include "cal-mdnssd-bip.h"

SSL_CTX * ssl_ctx_client = NULL;
SSL_CTX * ssl_ctx_server = NULL;

int bip_ssl_verify_callback(int ok, X509_STORE_CTX *store) {
    char data[256];
    
    if (!ok) {
	X509 *cert = X509_STORE_CTX_get_current_cert(store);
	int depth = X509_STORE_CTX_get_error_depth(store);
	int err = X509_STORE_CTX_get_error(store);

	g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
	      "Error with certificate at depth %i", depth);
	X509_NAME_oneline(X509_get_issuer_name(cert), data, 256);
	g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
	      "  issuer = %s", data);
	X509_NAME_oneline(X509_get_subject_name(cert), data, 256);
	g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
	      "  subject = %s", data);
	g_log(CAL_LOG_DOMAIN, G_LOG_LEVEL_WARNING,
	      " err %i:%s", err, X509_verify_cert_error_string(err));
    }

    return ok;
} /* verify_callback() */
