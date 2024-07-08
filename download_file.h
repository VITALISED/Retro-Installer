#pragma once

#include <curl/curl.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <string>

void DownloadFile(const std::string &url, const std::string &outputFilePath);