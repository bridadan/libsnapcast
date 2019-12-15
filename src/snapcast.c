#include "snapcast.h"
#include <cjson/cJSON.h>

static cJSON* hello_message_to_json(hello_message_t *msg) {
    cJSON *mac;
    cJSON *hostname;
    cJSON *version;
    cJSON *client_name;
    cJSON *os;
    cJSON *arch;
    cJSON *instance;
    cJSON *id;
    cJSON *protocol_version;
    cJSON *json = NULL;

    json = cJSON_CreateObject();
    if (!json) {
        goto error;
    }

    mac = cJSON_CreateString(msg->mac);
    if (!mac) {
        goto error;
    }
    cJSON_AddItemToObject(json, "MAC", mac);

    hostname = cJSON_CreateString(msg->hostname);
    if (!hostname) {
        goto error;
    }
    cJSON_AddItemToObject(json, "HostName", hostname);

    version = cJSON_CreateString(msg->version);
    if (!version) {
        goto error;
    }
    cJSON_AddItemToObject(json, "Version", version);

    client_name = cJSON_CreateString(msg->client_name);
    if (!client_name) {
        goto error;
    }
    cJSON_AddItemToObject(json, "ClientName", client_name);

    os = cJSON_CreateString(msg->os);
    if (!os) {
        goto error;
    }
    cJSON_AddItemToObject(json, "OS", os);

    arch = cJSON_CreateString(msg->arch);
    if (!arch) {
        goto error;
    }
    cJSON_AddItemToObject(json, "Arch", arch);

    instance = cJSON_CreateNumber(msg->instance);
    if (!instance) {
        goto error;
    }
    cJSON_AddItemToObject(json, "Instance", instance);

    id = cJSON_CreateString(msg->id);
    if (!id) {
        goto error;
    }
    cJSON_AddItemToObject(json, "ID", id);

    protocol_version = cJSON_CreateNumber(msg->protocol_version);
    if (!protocol_version) {
        goto error;
    }
    cJSON_AddItemToObject(json, "SnapStreamProtocolVersion", protocol_version);

    goto end;
error:
    cJSON_Delete(json);

end:
    return json;
}

char* hello_message_serialize(hello_message_t* msg) {
    int result;
    cJSON *json;
    char *str = NULL;

    json = hello_message_to_json(msg);
    if (!json) {
        return NULL;
    }

    str = cJSON_PrintUnformatted(json);
    cJSON_Delete(json);

    return str;
}
