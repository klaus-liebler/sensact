#include <stdio.h>
#include <string.h>
#include <sys/param.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <array>

#include "esp_err.h"
#include "esp_log.h"

#include "esp_vfs.h"
#include "esp_spiffs.h"
#include "esp_http_server.h"
#include "http_handlers.hh"
#include "manager.hh"
#include "paths_and_files.hh"

static constexpr char *TAG = "HTTP_handler";

constexpr size_t SCRATCH_BUFSIZE = 8192;
char scratch[SCRATCH_BUFSIZE];

constexpr size_t MAX_FILE_SIZE  = (200*1024); // 200 KB
constexpr const char* MAX_FILE_SIZE_STR="File size must be less than 200KB!";

class BinaryWriterImpl : public BinaryWriter
{
public:
    std::array<uint32_t, 128> buf{0};
    size_t statusBufferPtr{0};
    uint32_t currentApplicationId{0};
    size_t nextBufferPtr{0};

    ErrorCode StartWriting(uint32_t applicationId, size_t sizeIn4byteUnits) override
    {
        //check, if previous has been finished
        if (applicationId != 0)
            return ErrorCode::INVALID_STATE;
        if (statusBufferPtr + 2 + sizeIn4byteUnits >= buf.size())
            return ErrorCode::INDEX_OUT_OF_BOUNDS;
        currentApplicationId = applicationId;
        nextBufferPtr = statusBufferPtr + 2 + sizeIn4byteUnits;
        buf[statusBufferPtr++] = (uint32_t)applicationId;
        buf[statusBufferPtr++] = sizeIn4byteUnits;
        return ErrorCode::OK;
    }
    ErrorCode WriteU32(uint32_t value) override
    {
        if (statusBufferPtr >= nextBufferPtr)
            return ErrorCode::NONE_AVAILABLE;
        buf[statusBufferPtr++] = value;
        return ErrorCode::OK;
    }
    ErrorCode WriteF32(float value) override
    {
        if (statusBufferPtr >= nextBufferPtr)
            return ErrorCode::NONE_AVAILABLE;
        uint32_t value1 = *(uint32_t *)&value;
        buf[statusBufferPtr++] = value1;
        return ErrorCode::OK;
    }
    ErrorCode EndWriting(uint32_t applicationId) override
    {
        if (currentApplicationId != applicationId)
            return ErrorCode::INVALID_STATE;
        currentApplicationId = 0;
        statusBufferPtr = nextBufferPtr;
        return ErrorCode::OK;
    }
};


esp_err_t http_404_error_handler(httpd_req_t *req, httpd_err_code_t err)
{
    if (strcmp("/hello", req->uri) == 0)
    {
        httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "/hello URI is not available");
        /* Return ESP_OK to keep underlying socket open */
        return ESP_OK;
    }
    else if (strcmp("/echo", req->uri) == 0)
    {
        httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "/echo URI is not available");
        /* Return ESP_FAIL to close underlying socket */
        return ESP_FAIL;
    }
    /* For any other URI send 404 and close socket */
    httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "Some 404 error message");
    return ESP_FAIL;
}

extern const char index_html_gz_start[] asm("_binary_index_html_gz_start");
extern const char index_html_gz_end[] asm("_binary_index_html_gz_end");
extern const size_t index_html_gz_size asm("index_html_gz_length");

esp_err_t handle_get_root(httpd_req_t *req)
{
    httpd_resp_set_type(req, "text/html");
    httpd_resp_set_hdr(req, "Content-Encoding", "gzip");
    httpd_resp_send(req, index_html_gz_start, index_html_gz_size); // -1 = use strlen()
    return ESP_OK;
}
esp_err_t handle_get_webui(httpd_req_t *req)
{
    Manager *manager = (Manager *)(req->user_ctx);
    BinaryWriterImpl statusBuffer;
    manager->WebUIData(&statusBuffer);
    httpd_resp_set_type(req, "application/octet-stream");
    httpd_resp_send(req, (const char *)statusBuffer.buf.data(), statusBuffer.nextBufferPtr * 4);
    return ESP_OK;
}


esp_err_t handle_put_webui(httpd_req_t *req)
{
    Manager *manager = (Manager *)(req->user_ctx);
    int ret = 0;
    int remaining = req->content_len;
    char buf[req->content_len+1];
    while (remaining > 0)
    {
        // Read the data for the request
        if ((ret = httpd_req_recv(req, (char *)buf, MIN(remaining, sizeof(buf)))) <= 0)
        {
            if (ret == HTTPD_SOCK_ERR_TIMEOUT)
            {
                // Retry receiving if timeout occurred
                continue;
            }
            return ESP_FAIL;
        }
        remaining -= ret;
    }
    // End response
    buf[req->content_len]=0;
    cJSON *root=cJSON_Parse(buf);
    manager->HandleWebUICommand(root);
    uint32_t retbuf[1];
    retbuf[0] = 0;
    httpd_resp_set_type(req, "application/octet-stream");
    httpd_resp_send(req, (const char *)retbuf, 1 * 4);
    return ESP_OK;
}



esp_err_t helper_get_fbd(httpd_req_t *req, const char *filepath){
    FILE *fd = NULL;
    struct stat file_stat;
    ESP_LOGI(TAG, "helper_get_fbd : %s", filepath);
    if (stat(filepath, &file_stat) == -1){
        ESP_LOGE(TAG, "Failed to stat file : %s", filepath);
        httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "File does not exist");
        return ESP_FAIL;
    }
    fd = fopen(filepath, "r");
    if (!fd){
        ESP_LOGE(TAG, "Failed to read existing file : %s", filepath);
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to read existing file");
        return ESP_FAIL;
    }
    httpd_resp_set_type(req, "application/json");
    /* Retrieve the pointer to scratch buffer for temporary storage */
    char *chunk = scratch;
    size_t chunksize;
    do{
        /* Read file in chunks into the scratch buffer */
        chunksize = fread(chunk, 1, SCRATCH_BUFSIZE, fd);
        if (chunksize > 0){
            /* Send the buffer contents as HTTP response chunk */
            if (httpd_resp_send_chunk(req, chunk, chunksize) != ESP_OK){
                fclose(fd);
                ESP_LOGE(TAG, "File %s sending failed!", filepath);
                httpd_resp_sendstr_chunk(req, NULL);
                httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to send file");
                return ESP_FAIL;
            }
        }
    } while (chunksize != 0);
    fclose(fd);
    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}

esp_err_t helper_post_fbd(httpd_req_t *req, const char *filepath, bool overwrite){
    FILE *fd = NULL;
    struct stat file_stat;
    ESP_LOGI(TAG, "Trying to store : %s", filepath);
    if (stat(filepath, &file_stat) == 0) {
        if(overwrite)
        {
            unlink(filepath);
        }
        else
        {
            ESP_LOGE(TAG, "File already exists : %s", filepath);
            httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "File already exists");
            return ESP_FAIL;
        }
    }

    /* File cannot be larger than a limit */
    if (req->content_len > MAX_FILE_SIZE) {
        ESP_LOGE(TAG, "File too large : %d bytes", req->content_len);
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, MAX_FILE_SIZE_STR);
        /* Return failure to close underlying connection else the
         * incoming file content will keep the socket busy */
        return ESP_FAIL;
    }

    fd = fopen(filepath, "w");
    if (!fd) {
        ESP_LOGE(TAG, "Failed to create file : %s", filepath);
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to create file");
        return ESP_FAIL;
    }
    char *buf = scratch;
    int received;

    /* Content length of the request gives the size of the file being uploaded */
    int remaining = req->content_len;

    while (remaining > 0) {
        if ((received = httpd_req_recv(req, buf, MIN(remaining, SCRATCH_BUFSIZE))) <= 0) {
            if (received == HTTPD_SOCK_ERR_TIMEOUT) {
                continue;
            }
            /* In case of unrecoverable error,
             * close and delete the unfinished file*/
            fclose(fd);
            unlink(filepath);

            ESP_LOGE(TAG, "File %s reception failed!", filepath);
            httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to receive file");
            return ESP_FAIL;
        }

        /* Write buffer content to file on storage */
        if (received && (received != fwrite(buf, 1, received, fd))) {
            /* Couldn't write everything to file! Storage may be full? */
            fclose(fd);
            unlink(filepath);
            ESP_LOGE(TAG, "File %s write failed!", filepath);
            httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to write file to storage");
            return ESP_FAIL;
        }
        /* Keep track of remaining size of the file left to be uploaded */
        remaining -= received;
    }

    /* Close file upon upload completion */
    fclose(fd);
    httpd_resp_sendstr(req, "File uploaded successfully");
    return ESP_OK;
}

esp_err_t handle_get_cfgstorejson(httpd_req_t *req)
{
    char filepath[Paths::FILE_PATH_MAX];
    const char *filename = strrchr(req->uri, '/') + 1;
    if (!filename)
    {
        ESP_LOGE(TAG, "Filename is not defined correctly");
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Filename is not defined correctly");
        return ESP_FAIL;
    }

    strcpy(filepath, Paths::CFGSTORE_BASE);
    strcpy(filepath + strlen(filepath), filename);
    strcpy(filepath + strlen(filepath), ".json");
    return helper_get_fbd(req, filepath);
}

esp_err_t handle_post_fbdstorejson(httpd_req_t *req){
    char filepath[Paths::FILE_PATH_MAX];
    const char *filename = strrchr(req->uri, '/')+1;
    if (!filename) {
        ESP_LOGE(TAG, "Filename is not defined correctly");
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Filename is not defined correctly");
        return ESP_FAIL;
    }
    strcpy(filepath, Paths::CFGSTORE_BASE);
    strcpy(filepath+strlen(filepath), filename);
    strcpy(filepath+strlen(filepath), ".json");
    return helper_post_fbd(req, filepath, false);
}
