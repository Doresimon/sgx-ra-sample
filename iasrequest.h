#ifndef __IASREQUEST_H
#define __IASREQUEST_H

#include <sys/types.h>
#include <inttypes.h>
#include <openssl/x509.h>
#include "agent.h"

using namespace std;

#include <string>
#include <map>
#include <vector>

/* Our arguments and data must be NULL-terminated strings */

#define IAS_F_DEFAULT		IAS_F_VERIFY_PEER
#define IAS_F_VERIFY_PEER	0x1

/* v1 API has been EOL'd */
#define IAS_MIN_VERSION	2
#define IAS_MAX_VERSION	3

#define IAS_PROXY_NONE	0
#define IAS_PROXY_AUTO	1
#define IAS_PROXY_FORCE	2

#define IAS_SERVER_DEVELOPMENT	0
#define IAS_SERVER_PRODUCTION	1

#define IAS_SERVER_DEVELOPMENT_HOST	"test-as.sgx.trustedservices.intel.com"
#define IAS_SERVER_PRODUCTION_HOST	"as.sgx.trustedservices.intel.com"
#define IAS_PORT	443

#define IAS_API_DEF_VERSION	2

/* Model these roughly after errno */

typedef uint32_t ias_error_t;

// We couldn't construct the query string, or the query attempt to
// 
#define IAS_QUERY_FAILED		0
// Model these after HTTP response codes defined by the IAS spec //
#define IAS_OK					200
#define IAS_BADREQUEST			400
#define IAS_UNAUTHORIZED		401
#define IAS_NOT_FOUND			404
#define IAS_SERVER_ERR			500
#define IAS_UNAVAILABLE			503
// Other errors
#define IAS_INTERNAL_ERROR		1000
#define IAS_BAD_CERTIFICATE		1001
#define IAS_BAD_SIGNATURE		1002

class Agent;

class IAS_Connection {
friend class Agent;
	string c_server;
	string c_cert_file;
	string c_key_file;
	string c_cert_type;
	unsigned char *c_key_passwd;	
	unsigned char *c_xor;
	size_t c_pwlen;
	string c_proxy_server;
	uint16_t c_server_port;
	uint16_t c_proxy_port;
	int c_proxy_mode;
	uint32_t c_flags;
	X509_STORE *c_store;
	Agent *c_agent;

public:
	IAS_Connection(int server, uint32_t flags);
	~IAS_Connection();

	string base_url();

	int proxy(const char *server, uint16_t port);
	void proxy_mode(int mode) { c_proxy_mode= mode; }
	int proxy_mode() { return c_proxy_mode; }
	string proxy_server() { return c_proxy_server; }
	uint16_t proxy_port() { return c_proxy_port; }
	string proxy_url();

	int client_cert(const char *file, const char *certtype);
	string client_cert_file() { return c_cert_file; }
	string client_cert_type() { return c_cert_type; }

	int client_key(const char *file, const char *passwd);
	string client_key_file() { return c_key_file; }
	int client_key_passwd(char **passwd, size_t *len);

	void cert_store(X509_STORE *store) { c_store= store; }
	X509_STORE *cert_store() { return c_store; }

	Agent* new_agent();
	Agent* agent();
};


class IAS_Request {
	IAS_Connection *r_conn;
	uint16_t r_api_version;
	string url;

public:
	IAS_Request(IAS_Connection *conn_in, uint16_t version= IAS_API_DEF_VERSION);
	~IAS_Request();

	IAS_Connection *conn() { return r_conn; }

	ias_error_t sigrl(uint32_t gid, string &sigrl);
	ias_error_t report(map<string,string> &payload, string &content,
		vector<string> &messages);
};

#endif
