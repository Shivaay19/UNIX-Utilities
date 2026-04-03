#include <algorithm>
#include <cstdlib>
#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>

ssize_t writeAllBytes(int fileDesc, const char *buffer, ssize_t bytesToWrite);
void handleUsingREAD(int fileDescriptor);
void handleUsingMMAP(const char *filePtr, const struct stat &fileMetaData);
void logOpenError(const char *filePath);
void logWriteError();
void logReadError();

using namespace std;

int main(int argc, char *argv[])
{
    vector<string> filePaths{};

    for (int i = 1; i < argc; ++i)
    {
        filePaths.emplace_back(argv[i]);
    }

    if (filePaths.size() == 0)
    {
        filePaths.emplace_back("-");
    }

    int fileDescriptor{};
    char *filePtr{nullptr};
    struct stat fileMetaData;

    for (int i = 0; i < filePaths.size(); ++i)
    {
        if (strcmp(filePaths.at(i).c_str(), "-") == 0)
        {
            fileDescriptor = STDIN_FILENO;
            handleUsingREAD(fileDescriptor);
        }
        else
        {
            fileDescriptor = open(filePaths.at(i).c_str(), O_RDONLY);

            if (fileDescriptor == -1)
            {
                logOpenError(filePaths.at(i).c_str());
                exit(1);
            }

            if (fstat(fileDescriptor, &fileMetaData) == -1)
            {
                cerr << "Error while Opening the file\n";
                exit(2);
            }

            bool isRegularFile = S_ISREG(fileMetaData.st_mode);
            bool usedMMAP{false};

            if (isRegularFile && fileMetaData.st_size)
            {
                filePtr = static_cast<char *>(mmap(nullptr, fileMetaData.st_size, PROT_READ, MAP_PRIVATE, fileDescriptor, 0));
                if ((void *)filePtr == MAP_FAILED)
                {
                    cerr << "Error Occured While mmap() System Call\n";
                    exit(3);
                }
                else
                {
                    usedMMAP = true;
                }
            }

            if (usedMMAP)
            {
                handleUsingMMAP(filePtr, fileMetaData);
                munmap(filePtr, fileMetaData.st_size);
                filePtr = nullptr;
            }
            else
            {
                handleUsingREAD(fileDescriptor);
            }
        }

        if (fileDescriptor != STDIN_FILENO)
        {
            close(fileDescriptor);
        }
    }
    return 0;
}
ssize_t writeAllBytes(int fileDesc, const char *buffer, ssize_t bytesToWrite)
{
    ssize_t bytesWritten{};
    while (bytesWritten < bytesToWrite)
    {
        ssize_t writtenByteCount = write(fileDesc, buffer + bytesWritten, bytesToWrite - bytesWritten);

        if (writtenByteCount == -1)
        {
            if (errno == EINTR)
                continue;
            return -1;
        }

        bytesWritten = bytesWritten + writtenByteCount;
    }
    return bytesWritten;
}
void handleUsingREAD(int fileDescriptor)
{
    ssize_t bytesRead{};
    char readByte{};
    char readBuffer[4096];
    string lineContent{};

    while (true)
    {
        bytesRead = read(fileDescriptor, readBuffer, 4096);
        if (bytesRead > 0)
        {
            for (ssize_t index = 0; index < bytesRead; ++index)
            {
                readByte = readBuffer[index];
                if (readByte != '\n')
                {
                    lineContent.push_back(readByte);
                }
                else
                {
                    reverse(lineContent.begin(), lineContent.end());
                    if (writeAllBytes(STDOUT_FILENO, lineContent.data(), lineContent.size()) == -1)
                    {
                        logWriteError();
                        exit(4);
                    }
                    if (writeAllBytes(STDOUT_FILENO, "\n", 1) == -1)
                    {
                        logWriteError();
                        exit(4);
                    }
                    lineContent.clear();
                }
            }
        }
        else if (bytesRead == 0)
        {
            break;
        }
        else
        {
            if (errno == EINTR)
                continue;
            logReadError();
            exit(5);
        }
    }
    if (!lineContent.empty())
    {
        reverse(lineContent.begin(), lineContent.end());
        if (writeAllBytes(STDOUT_FILENO, lineContent.data(), lineContent.size()) == -1)
        {
            logWriteError();
            exit(4);
        }
        if (writeAllBytes(STDOUT_FILENO, "\n", 1) == -1)
        {
            logWriteError();
            exit(4);
        }
    }
}
void handleUsingMMAP(const char *filePtr, const struct stat &fileMetaData)
{
    string lineContent{};
    for (ssize_t index = 0; index < fileMetaData.st_size; ++index)
    {
        if (filePtr[index] != '\n')
        {
            lineContent.push_back(filePtr[index]);
        }
        else
        {
            reverse(lineContent.begin(), lineContent.end());
            if (writeAllBytes(STDOUT_FILENO, lineContent.data(), lineContent.size()) == -1)
            {
                logWriteError();
                exit(4);
            }
            if (writeAllBytes(STDOUT_FILENO, "\n", 1) == -1)
            {
                logWriteError();
                exit(4);
            }
            lineContent.clear();
        }
    }
    if (!lineContent.empty())
    {
        reverse(lineContent.begin(), lineContent.end());
        if (writeAllBytes(STDOUT_FILENO, lineContent.data(), lineContent.size()) == -1)
        {
            logWriteError();
            exit(4);
        }
        if (writeAllBytes(STDOUT_FILENO, "\n", 1) == -1)
        {
            logWriteError();
            exit(4);
        }
    }
}
void logOpenError(const char *filePath)
{
    cerr << "rev: " << filePath << ": " << "No such file or directory\n";
}
void logReadError()
{
    cerr << "Error Occured While Reading\n";
}
void logWriteError()
{
    cerr << "Error Occured While Writing\n";
}