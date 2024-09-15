#include <stdio.h>

#include <string.h>
#include <sys/param.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include <dirent.h>

#include "esp_err.h"
#include "esp_log.h"
#include "esp_http_server.h"


#define TAG "HTTP_handler"
#include "httpd_helper.hh"

constexpr size_t MAX_FILE_SIZE  = (200*1024); // 200 KB
constexpr const char* MAX_FILE_SIZE_STR="File size must be less than 200KB!";

esp_err_t helper_get_binary_file(httpd_req_t *req, const char *filepath) // helper
{
    FILE *fd = NULL;
    struct stat file_stat;
    ESP_LOGI(TAG, "helper_get_binary_file : %s", filepath);
    if (stat(filepath, &file_stat) == -1)
    {
        ESP_LOGE(TAG, "Failed to stat file : %s", filepath);
        httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "File does not exist");
        return ESP_FAIL;
    }
    fd = fopen(filepath, "r");
    if (!fd)
    {
        ESP_LOGE(TAG, "Failed to read existing file : %s", filepath);
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to read existing file");
        return ESP_FAIL;
    }
    httpd_resp_set_type(req, "application/octet-stream");
    /* Retrieve the pointer to scratch buffer for temporary storage */
    char *chunk = static_cast<char *>(httpd_get_global_user_ctx(req->handle));
    size_t chunksize;
    do
    {
        /* Read file in chunks into the scratch buffer */
        chunksize = fread(chunk, 1, CONFIG_HTTP_SCRATCHPAD_SIZE, fd);
        if (chunksize > 0)
        {
            /* Send the buffer contents as HTTP response chunk */
            if (httpd_resp_send_chunk(req, chunk, chunksize) != ESP_OK)
            {
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

esp_err_t RegisterHandler(httpd_handle_t server, const char *uri, http_method method, esp_err_t (*handler)(httpd_req_t *r), void *userCtx)
{
    httpd_uri_t x = {};
    x.uri = uri;
    x.method = method;
    x.handler = handler;
    x.user_ctx = userCtx;
    return httpd_register_uri_handler(server, &x);
}

esp_err_t helper_put_binary_file(httpd_req_t *req, const char *filepath, bool overwrite)
{
    FILE *fd = NULL;
    struct stat file_stat;
    ESP_LOGI(TAG, "Trying to store : %s", filepath);
    if (stat(filepath, &file_stat) == 0)
    {
        if (overwrite)
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
    if (req->content_len > MAX_FILE_SIZE)
    {
        ESP_LOGE(TAG, "File too large : %d bytes", req->content_len);
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, MAX_FILE_SIZE_STR);
        return ESP_FAIL;
    }

    fd = fopen(filepath, "w");
    if (!fd)
    {
        ESP_LOGE(TAG, "Failed to create file : %s", filepath);
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to create file");
        return ESP_FAIL;
    }
    ESP_LOGI(TAG, "Receiving file : %s...", filepath);

    /* Retrieve the pointer to scratch buffer for temporary storage */
    char *buf = static_cast<char *>(httpd_get_global_user_ctx(req->handle));
    int received;

    /* Content length of the request gives the size of the file being uploaded */
    int remaining = req->content_len;

    while (remaining > 0)
    {
        if ((received = httpd_req_recv(req, buf, MIN(remaining, CONFIG_HTTP_SCRATCHPAD_SIZE))) <= 0)
        {
            if (received == HTTPD_SOCK_ERR_TIMEOUT)
            {
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
        if (received && (received != fwrite(buf, 1, received, fd)))
        {
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
    ESP_LOGI(TAG, "File stored successfully: %s", filepath);
    return ESP_OK;
}
