/*
 * uhub - A tiny ADC p2p connection hub
 * Copyright (C) 2007-2012, Jan Vidar Krey
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef HAVE_UHUB_NETWORK_TLS_H
#define HAVE_UHUB_NETWORK_TLS_H

#include "uhub.h"

#ifdef SSL_SUPPORT


enum ssl_state
{
	tls_st_none,
	tls_st_error,
	tls_st_accepting,
	tls_st_connecting,
	tls_st_connected,
	tls_st_disconnecting,
};

enum net_con_ssl_mode
{
	net_con_ssl_mode_server,
	net_con_ssl_mode_client,
};


/**
 * Start SSL_accept()
 */
extern ssize_t net_con_ssl_accept(struct net_connection*);

/**
 * Start SSL_connect()
 */
extern ssize_t net_con_ssl_connect(struct net_connection*);

extern ssize_t net_ssl_send(struct net_connection* con, const void* buf, size_t len);
extern ssize_t net_ssl_recv(struct net_connection* con, void* buf, size_t len);

extern void net_ssl_shutdown(struct net_connection* con);
extern void net_ssl_destroy(struct net_connection* con);
extern void net_ssl_callback(struct net_connection* con, int events);


#ifdef SSL_USE_OPENSSL
extern ssize_t net_con_ssl_handshake(struct net_connection* con, enum net_con_ssl_mode, SSL_CTX* ssl_ctx);
extern SSL* net_con_get_ssl(struct net_connection* con);
extern void net_con_set_ssl(struct net_connection* con, SSL*);
#endif // SSL_USE_OPENSSL
extern int   net_con_is_ssl(struct net_connection* con);

#endif /* SSL_SUPPORT */
#endif /* HAVE_UHUB_NETWORK_TLS_H */