#include "Main.h"

#include "RingBuffer.h"

const char* testString = "1234567890 abcdefghijklmnopqrstuvwxyz 1234567890 abcdefghijklmnopqrstuvwxyz 12345"
                         "1234567890 abcdefghijklmnopqrstuvwxyz 1234567890 abcdefghijklmnopqrstuvwxyz 12345"
                         "1234567890 abcdefghijklmnopqrstuvwxyz 1234567890 abcdefghijklmnopqrstuvwxyz 12345"
                         "1234567890 abcdefghijklmnopqrstuvwxyz 1234567890 abcdefghijklmnopqrstuvwxyz 12345"
                         "1234567890 abcdefghijklmnopqrstuvwxyz 1234567890 abcdefghijklmnopqrstuvwxyz 12345"
                         "1234567890 abcdefghijklmnopqrstuvwxyz 1234567890 abcdefghijklmnopqrstuvwxyz 12345"
                         "1234567890 abcdefghijklmnopqrstuvwxyz 1234567890 abcdefghijklmnopqrstuvwxyz 12345"
                         "1234567890 abcdefghijklmnopqrstuvwxyz 1234567890 abcdefghijklmnopqrstuvwxyz 12345"
                         "1234567890 abcdefghijklmnopqrstuvwxyz 1234567890 abcdefghijklmnopqrstuvwxyz 12345"
                         "1234567890 abcdefghijklmnopqrstuvwxyz 1234567890 abcdefghijklmnopqrstuvwxyz 12345"
                         "1234567890 abcdefghijklmnopqrstuvwxyz 1234567890 abcdefghijklmnopqrstuvwxyz 12345"
                         "1234567890 abcdefghijklmnopqrstuvwxyz 1234567890 abcdefghijklmnopqrstuvwxyz 12345"
                         "1234567890 abcdefghijklmnopqrstuvwxyz 1234567890 abcdefghijklmnopqrstuvwxyz 12345"
                         "1234567890 abcdefghijklmnopqrstuvwxyz 1234567890 abcdefghijklmnopqrstuvwxyz 12345"
                         "1234567890 abcdefghijklmnopqrstuvwxyz 1234567890 abcdefghijklmnopqrstuvwxyz 12345"
                         "1234567890 abcdefghijklmnopqrstuvwxyz 1234567890 abcdefghijklmnopqrstuvwxyz 12345";

const int strLen = (int)strlen(testString);
const char* pCur = testString;
const char* pEnd = testString + strLen;
const char* pCompare = testString;

constexpr int seed = 100;

int wmain(int argc, const wchar_t** argv)
{
    srand(seed);
    printf(testString);
    int ignore = _getch();

    constexpr int ringBufSize = 1000;
    RingBuffer ringBuf(ringBufSize);

    char* buf = new char[strLen + 1];
    char* peekBuf = new char[strLen + 1];

    int sizeToEnqueue;
    int enqueueSize;
    int sizeToDequeue;
    int dequeueSize;
    int peekSize;
    int compareSize;
    char* tempBuf;

    int useSize = 0;
    int freeSize = ringBufSize - 1;

    DWORD prevTick = timeGetTime();
    DWORD nowTick;

    uint64_t loop;
    uint64_t cnt;
    for (loop = 0; loop < UINT64_MAX; ++loop)
    {
        for (cnt = 0; cnt < UINT64_MAX; ++cnt)
        {
            sizeToEnqueue = (rand() % strLen) + 1;
            if (sizeToEnqueue > pEnd - pCur)
                sizeToEnqueue = (int)(pEnd - pCur);
            enqueueSize = ringBuf.Enqueue(pCur, sizeToEnqueue);

            useSize += enqueueSize;
            freeSize -= enqueueSize;

            if (useSize != ringBuf.GetUseSize())
                DebugBreak();
            if (freeSize != ringBuf.GetFreeSize())
                DebugBreak();

            pCur += enqueueSize;
            if (pCur >= pEnd)
                pCur = testString;

            sizeToDequeue = (int)(((rand() % strLen) + 1) * 0.75);
            peekSize = ringBuf.Peek(peekBuf, sizeToDequeue);
            dequeueSize = ringBuf.Dequeue(buf, sizeToDequeue);

            if (peekSize != dequeueSize)
                DebugBreak();

            useSize -= dequeueSize;
            freeSize += dequeueSize;

            if (useSize != ringBuf.GetUseSize())
                DebugBreak();
            if (freeSize != ringBuf.GetFreeSize())
                DebugBreak();

            if (memcmp(peekBuf, buf, dequeueSize) != 0)
                DebugBreak();

            buf[dequeueSize] = '\0';
            printf(buf);

            {
                compareSize;
                tempBuf = buf;

                if (dequeueSize > pEnd - pCompare)
                {
                    compareSize = (int)(pEnd - pCompare);

                    if (memcmp(pCompare, tempBuf, compareSize) != 0)
                        DebugBreak();

                    tempBuf += compareSize;
                    compareSize = dequeueSize - compareSize;
                    pCompare = testString;

                    if (memcmp(pCompare, tempBuf, compareSize) != 0)
                        DebugBreak();

                    pCompare += compareSize;
                }
                else
                {
                    compareSize = dequeueSize;

                    if (memcmp(pCompare, buf, compareSize) != 0)
                        DebugBreak();

                    pCompare += compareSize;

                    if (pCompare == pEnd)
                        pCompare = testString;
                }
            }

            DWORD nowTick = timeGetTime();
            if (nowTick - prevTick >= 1000)
            {
                char title[64];
                StringCchPrintfA(title, _countof(title), "%d, %d, %d", enqueueSize, dequeueSize, ringBuf.GetUseSize());
                SetConsoleTitleA(title);
                prevTick = nowTick;
            }
        }
    }

    return 0;
}