#ifndef __RT_NET_SOCKET_H__
#define __RT_NET_SOCKET_H__
#include <stdint.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

typedef void * Server;
typedef void * Client;
typedef void * Piple;

#define PKT_MAGIC 0xABCDEF12

#define MAX_PKT_SIZE 1472

enum RT_PIPLE_TYPE
{
    RELIABLE_PIPLE = 1,
    RT_IMPORTANT_PIPLE,
    RT_UNIMPORTANT_PIPLE,
};

#pragma pack(4)
struct pkt_buf
{
    struct PKT_HEAD{
        uint32_t magic;
        uint32_t idx;
        uint32_t data_len;
    } head;
    uint8_t data[MAX_PKT_SIZE - sizeof(struct PKT_HEAD)];
};
#pragma pack()

enum RT_PIPLE_CTRL
{
    OPEN_PIPLE,
    OPEN_REP_ERROR,
    OPEN_REP_SUCCESS,
    BIND_PIPLE,
    BIND_REP_ERROR,
    BIND_REP_SUCCESS,
    PIPLE_DATA,
    CLOSE_PIPLE,
    CLOSE_PIPLE_REP_ERROR,
    CLOSE_PIPLE_REP_SUCCESS,
};

#define PIPLE_CHANNEL(piple_id)                             (piple_id & 0xFF)
#define PIPLE_CLIENT(piple_id)                              (piple_id >> 8 & 0xFFFF)
#define PIPLE_SERVER(piple_id)                              (piple_id >> 24 & 0xFF)
#define GET_PIPLE_ID(server_id, client_id, piple_channel)   (server_id << 24 | (client_id & 0xFFFF) << 8 | (piple_channel & 0xFF))
#pragma pack(1)
struct piple_pkt
{
    struct PIPLE_PKT_HEAD {
        enum RT_PIPLE_CTRL cmd;
        uint32_t piple_id;
        enum RT_PIPLE_TYPE piple_type;
        uint32_t idx;
        uint16_t data_len;
    }head;
    uint8_t data[MAX_PKT_SIZE - sizeof(struct PKT_HEAD) - sizeof(struct PIPLE_PKT_HEAD)];
};
#pragma pack()
#define PIPLE_DATA_LEN_MAX (MAX_PKT_SIZE - sizeof(struct PKT_HEAD) - sizeof(struct PIPLE_PKT_HEAD))

enum PIPLE_STATE
{
    PIPLE_OPENING = 1,
    PIPLE_OPENED,
    PIPLE_ERROR,
    PIPLE_CLOSED,
};

struct rt_net_piple
{
    uint32_t id;
    Client client;
    enum PIPLE_STATE state;
    enum RT_PIPLE_TYPE piple_type;

    uint32_t send_idx;
    uint32_t recv_idx;

    void *(* recv_callback)(Piple piple_fd, uint8_t *buf, uint16_t len);
    struct piple_pkt send_pkt;
    pthread_mutex_t send_lock;
};

#define PIPLE_MAX 64
struct rt_net_client
{
    int id;
    Server server;
    struct sockaddr_in tcp_client_addr;
    struct sockaddr_in udp_client_addr;

    struct sockaddr_in udp_recv_addr;

    struct pkt_buf tcp_send_buf;
    struct pkt_buf udp_send_buf;

    uint32_t tcp_send_idx;
    uint32_t tcp_recv_idx;
    uint32_t udp_send_idx;
    uint32_t udp_recv_idx;

    int tcp_fd;
    int udp_fd;

    bool udp_state;
    
	pthread_t udp_recv_thread;
	pthread_t tcp_recv_thread;

    pthread_cond_t msg_signal;
    uint8_t msg_timeout;
    pthread_mutex_t piple_open_lock;
	pthread_mutex_t accept_piple_lock;
	struct rt_net_piple *piples[PIPLE_MAX];
};

#define CLIENT_MAX 1024
struct rt_net_server
{
    int id;
    pthread_mutex_t accept_lock;

    struct sockaddr_in tcp_server_addr;
    struct rt_net_client udp_client;

    int tcp_listen_fd;

	uint32_t client_idx;
	struct rt_net_client *clients[CLIENT_MAX];
};

int udp_send_pkt(struct rt_net_client *client, const uint8_t *buf, uint16_t len);
int tcp_send_pkt(struct rt_net_client *client, const uint8_t *buf, uint16_t len);
int tcp_accept(struct rt_net_server *server, struct rt_net_client *client);
int net_server_init(struct rt_net_server *server, char *addr, uint16_t port);
int net_server_release(struct rt_net_server *server);
int net_client_init(struct rt_net_client *client, char *addr, uint16_t port);
int net_client_release(struct rt_net_client *client);

void _show_protocol(struct rt_net_client *client, const uint8_t *buf, uint16_t len, bool is_send);

#endif