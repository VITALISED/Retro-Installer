#include "download_file.h"

size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
    size_t written = fwrite(ptr, size, nmemb, stream);
    return written;
}

void DownloadFile(const std::string &url, const std::string &outputFilePath)
{
    CURL *curl;
    CURLcode res;
    FILE *fp;

    curl = curl_easy_init();
    if (curl)
    {
        fp = fopen(outputFilePath.c_str(), "wb");
        if (!fp)
        {
            std::cerr << "Failed to open file: " << outputFilePath << std::endl;
            curl_easy_cleanup(curl);
            return;
        }

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

        res = curl_easy_perform(curl);
        if (res != CURLE_OK)
        {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        }
        else
        {
            std::cout << "Download successful." << std::endl;
        }

        fclose(fp);
        curl_easy_cleanup(curl);
    }
    else
    {
        std::cerr << "Failed to initialize curl." << std::endl;
    }
}