#include "snapcast.h"

#include <cjson/cJSON.h>
#include <stdio.h>

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

int server_settings_message_parse(const char *json_str, server_settings_message_t *msg) {
    int status = 1;
    cJSON *value = NULL;
    cJSON *json = cJSON_Parse(json_str);
    if (!json) {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr) {
            // TODO change to a macro that can be diabled
            fprintf(stderr, "Error before: %s\n", error_ptr);
            goto end;
        }
    }

    if (msg == NULL) {
        status = 2;
        goto end;
    }

    value = cJSON_GetObjectItemCaseSensitive(json, "bufferMs");
    if (cJSON_IsNumber(value)) {
        msg->buffer_ms = value->valueint;
    }

    value = cJSON_GetObjectItemCaseSensitive(json, "latency");
    if (cJSON_IsNumber(value)) {
        msg->latency = value->valueint;
    }

    value = cJSON_GetObjectItemCaseSensitive(json, "volume");
    if (cJSON_IsNumber(value)) {
        msg->volume = value->valueint;
    }

    value = cJSON_GetObjectItemCaseSensitive(json, "muted");
    msg->muted = cJSON_IsTrue(value);
    status = 0;
end:
    cJSON_Delete(json);
    return status;
}
