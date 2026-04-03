#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include <cctype>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/stat.h>

ssize_t writeAllBytes(int fileDesc, const char *buffer, ssize_t bytesToWrite);
void handleUsingMMAP(struct stat &fileMetaData, char *filePtr, bool &isAWord, ssize_t &lineCount, ssize_t &wordCount, ssize_t &byteCount, ssize_t &maxLineLength);
void handleUsingREAD(int fileDesc, ssize_t &lineCount, ssize_t &wordCount, ssize_t &byteCount, bool &isAWord, ssize_t &maxLineLength);
void logInformation(ssize_t &lineCount, ssize_t &wordCount, ssize_t &byteCount, ssize_t &maxLineLength, const uint8_t &flags, const char *filePath);
void logOpenError(const char *filePath);
void logReadError();
void logWriteError();

using namespace std;

int main(int argc, char *argv[])
{
    vector<string> filePaths{};

    uint8_t flags{0b0000'0000};

    for (int i = 1; i < argc; ++i)
    {
        if (strcmp(argv[i], "-l") == 0)
        {
            flags = flags | (1 << 0);
        }
        else if (strcmp(argv[i], "-w") == 0)
        {
            flags = flags | (1 << 1);
        }
        else if (strcmp(argv[i], "-c") == 0)
        {
            flags = flags | (1 << 2);
        }
        else if (strcmp(argv[i], "-L") == 0)
        {
            flags = flags | (1 << 3);
        }
        else
        {
            filePaths.push_back(argv[i]);
        }
    }

    if (!flags)
    {
        for (int i = 0; i <= 3; ++i)
        {
            flags = flags | (1 << i);
        }
    }
    if (filePaths.size() == 0)
    {
        filePaths.push_back("-");
    }

    ssize_t lineCount{};
    ssize_t wordCount{};
    ssize_t byteCount{};
    ssize_t maxLineLength{};
    ssize_t totalLinesCount{};
    ssize_t totalWordsCount{};
    ssize_t totalBytesCount{};
    ssize_t totalMaxLineLength{};

    char *filePtr{};
    int fileDesc{};

    struct stat fileMetaData{};

    bool isRegularFile{false};
    bool isAWord{false};

    for (int i = 0; i < filePaths.size(); ++i)
    {
        lineCount = 0;
        wordCount = 0;
        byteCount = 0;
        maxLineLength = 0;
        isAWord = false;

        if (strcmp(filePaths.at(i).c_str(), "-") == 0)
        {
            fileDesc = STDIN_FILENO;
            handleUsingREAD(fileDesc, lineCount, wordCount, byteCount, isAWord, maxLineLength);
        }
        else
        {
            fileDesc = open(filePaths.at(i).c_str(), O_RDONLY);
            if (fileDesc == -1)
            {
                logOpenError(filePaths.at(i).c_str());
                exit(2);
            }

            if (fstat(fileDesc, &fileMetaData) == -1)
            {
                cerr << "Error Occured while retrieving the meta data of the file\n";
                exit(3);
            }

            isRegularFile = S_ISREG(fileMetaData.st_mode);
            bool usedMMAP{false};

            if (isRegularFile && fileMetaData.st_size)
            {
                filePtr = static_cast<char *>(mmap(nullptr, fileMetaData.st_size, PROT_READ, MAP_PRIVATE, fileDesc, 0));
                usedMMAP = true;
            }
            else
            {
                handleUsingREAD(fileDesc, lineCount, wordCount, byteCount, isAWord, maxLineLength);
            }

            if (usedMMAP && (void *)filePtr == MAP_FAILED)
            {
                cerr << "Error Occured While mmap() System Call\n";
                exit(5);
            }

            if (usedMMAP)
            {
                handleUsingMMAP(fileMetaData, filePtr, isAWord, lineCount, wordCount, byteCount, maxLineLength);
            }
        }

        if (fileDesc != STDIN_FILENO)
        {
            close(fileDesc);
        }

        totalLinesCount = totalLinesCount + lineCount;
        totalWordsCount = totalWordsCount + wordCount;
        totalBytesCount = totalBytesCount + byteCount;
        totalMaxLineLength = max(totalMaxLineLength, maxLineLength);

        logInformation(lineCount, wordCount, byteCount, maxLineLength, flags, filePaths.at(i).c_str());
    }

    if (filePaths.size() > 1)
    {
        logInformation(totalLinesCount, totalWordsCount, totalBytesCount, totalMaxLineLength, flags, "total");
    }

    return 0;
}

void handleUsingMMAP(struct stat &fileMetaData, char *filePtr, bool &isAWord, ssize_t &lineCount, ssize_t &wordCount, ssize_t &byteCount, ssize_t &maxLineLength)
{
    ssize_t currentMaxLineLength{};
    for (ssize_t index = 0; index < fileMetaData.st_size; ++index)
    {
        if (isspace(static_cast<unsigned char>(filePtr[index])))
        {
            isAWord = false;
        }
        else
        {
            if (!isAWord)
            {
                ++wordCount;
                isAWord = true;
            }
        }
        if (filePtr[index] == '\n')
        {
            ++lineCount;
            maxLineLength = max(maxLineLength, currentMaxLineLength);
            currentMaxLineLength = 0;
        }
        else
        {
            ++currentMaxLineLength;
        }
    }

    maxLineLength = max(maxLineLength, currentMaxLineLength);
    byteCount = fileMetaData.st_size;

    munmap(filePtr, fileMetaData.st_size);
}

void handleUsingREAD(int fileDesc, ssize_t &lineCount, ssize_t &wordCount, ssize_t &byteCount, bool &isAWord, ssize_t &maxLineLength)
{
    char buffer[4096]{};

    ssize_t bytesRead{};
    ssize_t currentMaxLineLength{};

    while (true)
    {
        bytesRead = read(fileDesc, buffer, 4096);
        if (bytesRead > 0)
        {
            for (int i = 0; i < bytesRead; ++i)
            {
                char byte{buffer[i]};
                if (isspace(static_cast<unsigned char>(byte)))
                {
                    isAWord = false;
                }
                else
                {
                    if (!isAWord)
                    {
                        isAWord = true;
                        ++wordCount;
                    }
                }
                if (byte == '\n')
                {
                    ++lineCount;
                    maxLineLength = max(maxLineLength, currentMaxLineLength);
                    currentMaxLineLength = 0;
                }
                else
                {
                    ++currentMaxLineLength;
                }
            }
            byteCount = byteCount + bytesRead;
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
            exit(6);
        }
    }
    maxLineLength = max(maxLineLength, currentMaxLineLength); // if file doesn't ends with '\n'
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

void logInformation(ssize_t &lineCount, ssize_t &wordCount, ssize_t &byteCount, ssize_t &maxLineLength, const uint8_t &flags, const char *filePath)
{
    const char *padding{"      "};
    ssize_t paddingLength{6};

    string numberToString{};

    constexpr uint8_t lineMask{0b0000'0001};
    constexpr uint8_t wordMask{0b0000'0010};
    constexpr uint8_t byteMask{0b0000'0100};
    constexpr uint8_t maxLineMask{0b0000'1000};

    if (static_cast<bool>(flags & lineMask))
    {
        if (writeAllBytes(STDOUT_FILENO, padding, paddingLength) == -1)
        {
            logWriteError();
            exit(3);
        }
        numberToString = to_string(lineCount);
        if (writeAllBytes(STDOUT_FILENO, numberToString.c_str(), strlen(numberToString.c_str())) == -1)
        {
            logWriteError();
            exit(3);
        }
    }
    if (static_cast<bool>(flags & wordMask))
    {
        if (writeAllBytes(STDOUT_FILENO, padding, paddingLength) == -1)
        {
            logWriteError();
            exit(3);
        }
        numberToString = to_string(wordCount);
        if (writeAllBytes(STDOUT_FILENO, numberToString.c_str(), strlen(numberToString.c_str())) == -1)
        {
            logWriteError();
            exit(3);
        }
    }
    if (static_cast<bool>(flags & byteMask))
    {
        if (writeAllBytes(STDOUT_FILENO, padding, paddingLength) == -1)
        {
            logWriteError();
            exit(3);
        }
        numberToString = to_string(byteCount);
        if (writeAllBytes(STDOUT_FILENO, numberToString.c_str(), strlen(numberToString.c_str())) == -1)
        {
            logWriteError();
            exit(3);
        }
    }
    if (static_cast<bool>(flags & maxLineMask))
    {
        if (writeAllBytes(STDOUT_FILENO, padding, paddingLength) == -1)
        {
            logWriteError();
            exit(3);
        }
        numberToString = to_string(maxLineLength);
        if (writeAllBytes(STDOUT_FILENO, numberToString.c_str(), strlen(numberToString.c_str())) == -1)
        {
            logWriteError();
            exit(3);
        }
    }
    if (strcmp(filePath, "-") != 0)
    {
        if (writeAllBytes(STDOUT_FILENO, padding, paddingLength) == -1)
        {
            logWriteError();
            exit(3);
        }
        if (writeAllBytes(STDOUT_FILENO, filePath, strlen(filePath)) == -1)
        {
            logWriteError();
            exit(3);
        }
    }
    if (writeAllBytes(STDOUT_FILENO, "\n", 1) == -1)
    {
        logWriteError();
        exit(3);
    }
}
void logOpenError(const char *filePath)
{
    cerr << "wc: " << filePath << ": " << "open: No such file or directory\n";
}
void logReadError()
{
    cerr << "Error Occured While Reading\n";
}
void logWriteError()
{
    cerr << "Error Occured While Writing\n";
}