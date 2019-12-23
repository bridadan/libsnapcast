#include "snapcast.h"

#include <cjson/cJSON.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "buffer.h"

int base_message_deserialize(base_message_t *msg, const char *data, uint32_t size) {
    read_buffer_t buffer;
    int result = 0;

    buffer_read_init(&buffer, data, size);

    result |=  buffer_read_uint16(&buffer, &(msg->type));
    result |=  buffer_read_uint16(&buffer, &(msg->id));
    result |=  buffer_read_uint16(&buffer, &(msg->refersTo));
    result |=  buffer_read_int32(&buffer, &(msg->received.sec));
    result |=  buffer_read_int32(&buffer, &(msg->received.usec));
    result |=  buffer_read_int32(&buffer, &(msg->sent.sec));
    result |=  buffer_read_int32(&buffer, &(msg->sent.usec));
    result |=  buffer_read_uint32(&buffer, &(msg->size));

    return result;
}

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

int server_settings_message_deserialize(server_settings_message_t *msg, const char *json_str) {
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

int wire_chunk_message_deserialize(wire_chunk_message_t *msg, const char *data, uint32_t size) {
    read_buffer_t buffer;
    int result = 0;

    buffer_read_init(&buffer, data, size);

    result |=  buffer_read_int32(&buffer, &(msg->timestamp.sec));
    result |=  buffer_read_int32(&buffer, &(msg->timestamp.usec));
    result |=  buffer_read_uint32(&buffer, &(msg->size));

    // If there's been an error already (especially for the size bit) return early
    if (result) {
        return result;
    }

    // TODO maybe should check to see if need to free memory?
    msg->payload = malloc(msg->size * sizeof(char));
    // Failed to allocate the memory
    if (!msg->payload) {
        return 2;
    }

    result |= buffer_read_buffer(&buffer, msg->payload, msg->size);
    return result;
}

void wire_chunk_message_free(wire_chunk_message_t *msg) {
    if (msg->payload) {
        free(msg->payload);
        msg->payload = NULL;
    }
}
