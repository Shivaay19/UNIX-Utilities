#include <iostream>
#include <vector>
#include <unistd.h>
#include <string.h>
#include <string>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>

ssize_t writeAllBytes(int fileDesc, const char* buffer, ssize_t bytesToWrite);
void logOpenError(const char* filePath);
void logReadError();
void logWriteError();

using namespace std;

int main(int argc, char *argv[])
{
    vector<string> filePaths {};
    bool showLineNumbers {false};
    bool showNonEmptyLineNumbers {false};
    bool markEndLine {false};

    for (int i = 1; i < argc; ++i)
    {
        if (strcmp(argv[i], "-n") == 0)
        {
            showLineNumbers = true;
        }
        else if (strcmp(argv[i], "-b") == 0)
        {
            showNonEmptyLineNumbers = true;
        }
        else if (strcmp(argv[i], "-e") == 0)
        {
            markEndLine = true;
        }
        else
        {
            filePaths.push_back(argv[i]);
        }
    }
    
    if(filePaths.size() == 0)
    {
        filePaths.push_back("-");
    }

    if (showNonEmptyLineNumbers)                 
    {
        showLineNumbers = false;
    }

    const char* leftPadding {"    "};
    ssize_t leftPaddingLength {4};

    const char* paddingFromLineNumber {"  "};
    ssize_t paddingFromLineNumberLength {2};

    const char* emptyLineMarkingPadding {"       "};
    ssize_t emptyLineMarkingPaddingLength {7};

    ssize_t bytesRead {};

    char byte {};
    long lineCount {1};
    bool isNewLine {true};
    bool lineHasContent {false};
 
    int fileDesc {};

    for (int i = 0; i < filePaths.size(); ++i)
    {
        if (strcmp(filePaths.at(i).c_str(), "-") == 0)
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

        while (true)
        {
            bytesRead = read(fileDesc, &byte, sizeof(byte));
            if (bytesRead > 0)
            {
                if (showLineNumbers)
                {
                    if (isNewLine)
                    {
                        string lineNumber {to_string(lineCount)};

                        if(writeAllBytes(STDOUT_FILENO, leftPadding, leftPaddingLength) == -1)
                        {
                            logWriteError();
                            exit(2);
                        }
                        if(writeAllBytes(STDOUT_FILENO, lineNumber.data(), lineNumber.size()) == -1)
                        {
                            logWriteError();
                            exit(2);
                        }
                        if(writeAllBytes(STDOUT_FILENO, paddingFromLineNumber, paddingFromLineNumberLength) == -1)
                        {
                            logWriteError();
                            exit(2);
                        }

                        isNewLine = false;
                    }

                    if (byte != '\n')
                    {
                        lineHasContent = true;
                    }
                    else if (byte == '\n')
                    {
                        if (markEndLine)
                        {
                            if (!lineHasContent)
                            {
                                if(writeAllBytes(STDOUT_FILENO, emptyLineMarkingPadding, emptyLineMarkingPaddingLength) == -1)
                                {
                                    logWriteError();
                                    exit(2);
                                }
                            }
                            if(writeAllBytes(STDOUT_FILENO, "$", 1) == -1)
                            {
                                logWriteError();
                                exit(2);
                            }
                        }
                        isNewLine = true;
                        lineHasContent = false;
                        ++lineCount;
                    }
                }

                if (showNonEmptyLineNumbers)
                {
                    if (isNewLine && byte != '\n')
                    {
                        string lineNumber {to_string(lineCount)};

                        if(writeAllBytes(STDOUT_FILENO, leftPadding, leftPaddingLength) == -1)
                        {
                            logWriteError();
                            exit(2);
                        }
                        if(writeAllBytes(STDOUT_FILENO, lineNumber.data(), lineNumber.size()) == -1)
                        {
                            logWriteError();
                            exit(2);
                        }
                        if(writeAllBytes(STDOUT_FILENO, paddingFromLineNumber, paddingFromLineNumberLength) == -1)
                        {
                            logWriteError();
                            exit(2);
                        }

                        isNewLine = false;
                    }

                    if (byte != '\n')
                    {
                        lineHasContent = true;
                    }
                    else if (byte == '\n')
                    {
                        if (markEndLine)
                        {
                            if (!lineHasContent)
                            {
                                if(writeAllBytes(STDOUT_FILENO, emptyLineMarkingPadding, emptyLineMarkingPaddingLength) == -1)
                                {
                                    logWriteError();
                                    exit(2);
                                }
                            }
                            if(writeAllBytes(STDOUT_FILENO, "$", 1) == -1)
                            {
                                logWriteError();
                                exit(2);
                            }
                        }

                        if (lineHasContent)
                        {
                            ++lineCount;
                        }

                        isNewLine = true;
                        lineHasContent = false;
                    }
                }
                if(writeAllBytes(STDOUT_FILENO, &byte, sizeof(byte)) == -1)
                {
                    logWriteError();
                    exit(2);
                }
            }

            else if (bytesRead == 0)
            {
                break;
            }

            else
            {
                if(errno == EINTR) continue;
                logReadError();
                exit(3);
            }
        }
        if(fileDesc != STDIN_FILENO)
        {
            close(fileDesc);
        }
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
void logOpenError(const char* filePath)
{
    cerr<<"cat: "<<filePath<<": "<<"No such file or directory\n";
}
void logReadError()
{
    cerr<<"Error Occured while Reading\n";
}
void logWriteError()
{
    cerr<<"Error Occured while Writing\n";
}