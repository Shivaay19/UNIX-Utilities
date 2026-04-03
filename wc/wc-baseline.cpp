#include <iostream>
#include <vector>
#include <unistd.h>
#include <string.h>
#include <string>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>

ssize_t writeAllBytes(int fileDesc, const char* buffer, ssize_t bytesToWrite);
void logInformation(bool& showLineCount, bool& showWordCount, bool& showByteCount, ssize_t& lineCount, ssize_t& wordCount, ssize_t& byteCount, const char* filePath);
void logOpenError(const char* filePath);
void logReadError();
void logWriteError();

using namespace std;

int main(int argc, char* argv[])
{
    vector<string> filePaths {};
    bool showLineCount {false};
    bool showWordCount {false};
    bool showByteCount {false};
    
    for(int i = 1; i < argc; ++i)
    {
        if(strcmp(argv[i],"-l") == 0)
        {
            showLineCount = true;
        }
        else if(strcmp(argv[i],"-w") == 0)
        {
            showWordCount = true;
        }
        else if(strcmp(argv[i],"-c") == 0)
        {
            showByteCount = true;
        }
        else
        {
            filePaths.push_back(argv[i]);
        }
    }

    if(!showLineCount && !showWordCount && !showByteCount)
    {
        showLineCount = true;
        showWordCount = true;
        showByteCount = true;
    }

    if(filePaths.size() == 0)
    {
        filePaths.push_back("-");
    }

    ssize_t totalLinesCount {};
    ssize_t totalWordsCount {};
    ssize_t totalBytesCount {};

    char byte {};

    int fileDesc {};

    for(int i = 0; i < filePaths.size(); ++i)
    {
        if(strcmp(filePaths.at(i).c_str(), "-") == 0)
        {
            fileDesc = STDIN_FILENO;
        }
        else
        {
            fileDesc = open(filePaths.at(i).c_str(), O_RDONLY);
            if(fileDesc == -1)
            {
                logOpenError(filePaths.at(i).c_str());
                exit(1);
            }
        }

        ssize_t lineCount {};
        ssize_t wordCount {};
        ssize_t byteCount {};
        bool isAWord {false};

        ssize_t bytes_read {};

        while(true)
        {
            bytes_read = read(fileDesc, &byte, sizeof(byte));

            if(bytes_read > 0)
            {
                if(isspace(static_cast<unsigned char>(byte)))
                {
                    isAWord = false;
                }
                else
                {
                    if(!isAWord)
                    {
                        ++wordCount;
                        isAWord = true;
                    }
                }
                if(byte == '\n')
                {
                    ++lineCount;
                }
                byteCount = byteCount + bytes_read;
            }

            else if(bytes_read == 0)
            {
                break;
            }

            else
            {
                if(errno == EINTR) continue;
                logReadError();
                exit(2);
            }

        }

        if(fileDesc != STDIN_FILENO)
        {
            close(fileDesc);
        }

        totalLinesCount = totalLinesCount + lineCount;
        totalWordsCount = totalWordsCount + wordCount;
        totalBytesCount = totalBytesCount + byteCount;

        logInformation(showLineCount, showWordCount, showByteCount, lineCount, wordCount, byteCount, filePaths.at(i).c_str());
    }

    if(filePaths.size() > 1)
    {
        logInformation(showLineCount, showWordCount, showByteCount,totalLinesCount,totalWordsCount,totalBytesCount,"total");
    }

    return 0;
}

ssize_t writeAllBytes(int fileDesc, const char* buffer, ssize_t bytesToWrite)
{
    ssize_t bytes_written {};
    while(bytes_written < bytesToWrite)
    {
        ssize_t byte_Count = write(fileDesc, buffer + bytes_written, bytesToWrite - bytes_written);

        if(byte_Count == -1)
        {
            if(errno == EINTR) continue;
            return -1;
        }

        bytes_written = bytes_written + byte_Count;
    }
    return bytes_written;
}

void logInformation(bool& showLineCount, bool& showWordCount, bool& showByteCount, ssize_t& lineCount, ssize_t& wordCount, ssize_t& byteCount, const char* filePath)
{
    string numberToString {};

    const char* padding {"      "};
    ssize_t paddingLength {6};

    if(showLineCount)
    {
        if(writeAllBytes(STDOUT_FILENO, padding, paddingLength) == -1)
        {
            logWriteError();
            exit(3);
        }
        numberToString = to_string(lineCount);
        if(writeAllBytes(STDOUT_FILENO, numberToString.c_str(), strlen(numberToString.c_str())) == -1)
        {
            logWriteError();
            exit(3);
        }
    }
    if(showWordCount)
    {
        if(writeAllBytes(STDOUT_FILENO, padding, paddingLength) == -1)
        {
            logWriteError();
            exit(3);
        }
        numberToString = to_string(wordCount);
        if(writeAllBytes(STDOUT_FILENO, numberToString.c_str(), strlen(numberToString.c_str())) == -1)
        {
            logWriteError();
            exit(3);
        }
    }
    if(showByteCount)
    {
        if(writeAllBytes(STDOUT_FILENO, padding, paddingLength) == -1)
        {
            logWriteError();
            exit(3);
        }
        numberToString = to_string(byteCount);
        if(writeAllBytes(STDOUT_FILENO, numberToString.c_str(), strlen(numberToString.c_str())) == -1)
        {
            logWriteError();
            exit(3);
        }
        
    }
    if(strcmp(filePath,"-") != 0)
    {
        if(writeAllBytes(STDOUT_FILENO, padding, paddingLength) == -1)
        {
            logWriteError();
            exit(3);
        }
        if(writeAllBytes(STDOUT_FILENO, filePath, strlen(filePath)) == -1)
        {
            logWriteError();
            exit(3);
        }
        if(writeAllBytes(STDOUT_FILENO, "\n", 1) == -1)
        {
            logWriteError();
            exit(3);
        }
    }
}

void logOpenError(const char* filePath)
{
    cerr<<"wc: "<<filePath<<": "<<"open: No such file or directory\n";
}

void logReadError()
{
    cerr<<"Error Occured while Reading\n";
}

void logWriteError()
{
    cerr<<"Error Occured while Writing\n";
}