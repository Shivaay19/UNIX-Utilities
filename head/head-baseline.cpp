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
void handleUsingMMAP(const struct stat &fileMetaData, const char *filePtr, bool readNLines, ssize_t linesToRead, bool readNBytes, ssize_t bytesToRead, bool showHeaders, const char *filePath, bool &isFirstFile);
void handleUsingREAD(int fileDescriptor, bool readNLines, ssize_t linesToRead, bool readNBytes, ssize_t bytesToRead, bool showHeaders, const char *filePath, bool &isFirstFile);
void writeHeader(bool &isFirstFile, const char *filePath);
void logOpenError(const char *filePath);
void logReadError();
void logWriteError();
void logArgumentError();
void logLineInvalidArgumentError(const char *endPtr);
void logByteInvalidArgumentError(const char *endPtr);

using namespace std;

int main(int argc, char *argv[])
{
    vector<string> filePath{};

    bool readNLines{false};
    ssize_t linesToRead{10};

    bool readNBytes{false};
    ssize_t bytesToRead{};

    bool showHeaders{false};
    bool isFirstFile{true};

    for (int i = 1; i < argc; ++i)
    {
        if (strcmp(argv[i], "-n") == 0)
        {
            readNLines = true;
            if (i + 1 >= argc)
            {
                logArgumentError();
                exit(1);
            }
            else
            {
                errno = 0;
                char *endPtr{};
                linesToRead = strtol(argv[++i], &endPtr, 10);
                if ((errno == ERANGE && (linesToRead == LONG_MAX || linesToRead == LONG_MIN)) || (*endPtr != '\0') || (endPtr == argv[i]) || (linesToRead <= 0))
                {
                    logLineInvalidArgumentError(argv[i]);
                    exit(2);
                }
            }
        }
        else if (strcmp(argv[i], "-c") == 0)
        {
            readNBytes = true;
            if (i + 1 >= argc)
            {
                logArgumentError();
                exit(1);
            }
            else
            {
                errno = 0;
                char *endPtr{};
                bytesToRead = strtol(argv[++i], &endPtr, 10);
                if ((errno == ERANGE && (bytesToRead == LONG_MAX || bytesToRead == LONG_MIN)) || (*endPtr != '\0') || (endPtr == argv[i]) || (bytesToRead <= 0))
                {
                    logByteInvalidArgumentError(argv[i]);
                    exit(2);
                }
            }
        }
        else
        {
            filePath.emplace_back(argv[i]);
        }
    }

    if (filePath.size() > 1)
    {
        showHeaders = true;
    }

    if (filePath.size() == 0)
    {
        filePath.emplace_back("-");
    }

    if (!readNLines && !readNBytes)
    {
        readNLines = true;
    }

    if (readNLines && readNBytes)
    {
        readNLines = false;
    }

    int fileDescriptor{};
    char *filePtr{};
    struct stat fileMetaData{};

    for (int i = 0; i < filePath.size(); ++i)
    {
        if (strcmp(filePath.at(i).c_str(), "-") == 0)
        {
            fileDescriptor = STDIN_FILENO;
            handleUsingREAD(fileDescriptor, readNLines, linesToRead, readNBytes, bytesToRead, showHeaders, filePath.at(i).c_str(), isFirstFile);
        }
        else
        {
            fileDescriptor = open(filePath.at(i).c_str(), O_RDONLY);

            if (fileDescriptor == -1)
            {
                logOpenError(filePath.at(i).c_str());
                exit(3);
            }

            if (fstat(fileDescriptor, &fileMetaData) == -1)
            {
                cerr << "Error Occured while retrieving the meta data of the file\n";
                exit(4);
            }

            bool usedMMAP{false};
            bool isRegularFile = S_ISREG(fileMetaData.st_mode);

            if (isRegularFile && fileMetaData.st_size)
            {
                filePtr = static_cast<char *>(mmap(nullptr, fileMetaData.st_size, PROT_READ, MAP_PRIVATE, fileDescriptor, 0));
                if ((void *)filePtr == MAP_FAILED)
                {
                    cerr << "Error Occured While mmap() System Call\n";
                    exit(5);
                }
                else
                {
                    usedMMAP = true;
                }
            }

            if (usedMMAP)
            {
                handleUsingMMAP(fileMetaData, filePtr, readNLines, linesToRead, readNBytes, bytesToRead, showHeaders, filePath.at(i).c_str(), isFirstFile);
                munmap(filePtr, fileMetaData.st_size);
                filePtr = nullptr;
            }
            else
            {
                handleUsingREAD(fileDescriptor, readNLines, linesToRead, readNBytes, bytesToRead, showHeaders, filePath.at(i).c_str(), isFirstFile);
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
void handleUsingREAD(int fileDescriptor, bool readNLines, ssize_t linesToRead, bool readNBytes, ssize_t bytesToRead, bool showHeaders, const char *filePath, bool &isFirstFile)
{
    ssize_t linesRead{};
    ssize_t bytesRead{};
    char byte{};

    if (showHeaders)
    {
        writeHeader(isFirstFile, filePath);
    }

    errno = 0;
    while (true)
    {
        bytesRead = read(fileDescriptor, &byte, sizeof(byte));
        if (bytesRead > 0)
        {
            if (writeAllBytes(STDOUT_FILENO, &byte, sizeof(byte)) == -1)
            {
                logWriteError();
                exit(6);
            }

            if (readNLines)
            {
                if (byte == '\n')
                {
                    ++linesRead;
                    if (linesRead == linesToRead)
                    {
                        break;
                    }
                }
            }
            else if (readNBytes)
            {
                bytesToRead = bytesToRead - bytesRead;
                if (bytesToRead == 0)
                {
                    break;
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
            exit(7);
        }
    }
}
void handleUsingMMAP(const struct stat &fileMetaData, const char *filePtr, bool readNLines, ssize_t linesToRead, bool readNBytes, ssize_t bytesToRead, bool showHeaders, const char *filePath, bool &isFirstFile)
{
    ssize_t linesRead{};
    ssize_t bytesRead{};
    char byte{};

    if (showHeaders)
    {
        writeHeader(isFirstFile, filePath);
    }

    for (ssize_t index = 0; index < fileMetaData.st_size; ++index)
    {
        if (readNLines)
        {
            byte = filePtr[index];
            if (writeAllBytes(STDOUT_FILENO, &byte, sizeof(byte)) == -1)
            {
                logWriteError();
                exit(6);
            }

            if (byte == '\n')
            {
                ++linesRead;
                if (linesRead == linesToRead)
                {
                    break;
                }
            }
        }
        else if (readNBytes)
        {
            byte = filePtr[index];
            if (writeAllBytes(STDOUT_FILENO, &byte, sizeof(byte)) == -1)
            {
                logWriteError();
                exit(6);
            }

            ++bytesRead;
            if (bytesRead == bytesToRead)
            {
                break;
            }
        }
    }
}
void writeHeader(bool &isFirstFile, const char *filePath)
{
    string header {};
    if(isFirstFile)
    {
        isFirstFile = false;
    }
    else
    {
        header = header + "\n";
    }
    
    header = header + "==> ";
    header = header + filePath;
    header = header + " <==\n";
    if (writeAllBytes(STDOUT_FILENO, header.c_str(), strlen(header.c_str())) == -1)
    {
        logWriteError();
        exit(6);
    }
}
void logOpenError(const char *filePath)
{
    cerr << "head: " << filePath << ": " << "open: No such file or directory\n";
}
void logReadError()
{
    cerr << "Error Occured While Reading\n";
}
void logWriteError()
{
    cerr << "Error Occured While Writing\n";
}
void logArgumentError()
{
    cerr << "head: option requires an argument -- n\n";
}
void logLineInvalidArgumentError(const char *argValue)
{
    cerr << "head: illegal line count -- " << argValue << '\n';
}
void logByteInvalidArgumentError(const char *argValue)
{
    cerr << "head: illegal byte count -- " << argValue << '\n';
}