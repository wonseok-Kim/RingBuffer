#include "Main.h"

#include "RingBuffer.h"
#include "my_profile.h"

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

static void TestEnqueDeque();
static void TestMoveFrontRear();
static void TestPerformance();
static void TestPerformance1(int _size);
static void TestPerformance2(int _size);

int wmain(int argc, const wchar_t** argv)
{
    srand(seed);
    printf(testString);
    int ignore = _getch();

    // TestEnqueDeque();
    // TestMoveFrontRear();
    TestPerformance();

    return 0;
}

void TestEnqueDeque()
{
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

            sizeToDequeue = (int)(((rand() % strLen) + 1) * 0.65);
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

            nowTick = timeGetTime();
            if (nowTick - prevTick >= 1000)
            {
                char title[64];
                StringCchPrintfA(title, _countof(title), "%d, %d, %d", enqueueSize, dequeueSize, ringBuf.GetUseSize());
                SetConsoleTitleA(title);
                prevTick = nowTick;
            }
        }
    }
}

void TestMoveFrontRear()
{
    constexpr int ringBufSize = 100;
    RingBuffer ringBuf(ringBufSize);

    char* buf = new char[strLen + 1];
    char* tempBuf;

    int freeSize;
    int useSize;

    int sizeToEnque;
    int sizeToDeque;
    int directEnqueSize;
    int directDequeSize;
    int moveRearSize;
    int moveFrontSize;

    int remainSize;

    uint64_t loop1;
    uint64_t loop2;
    for (loop1 = 0; loop1 < UINT64_MAX; ++loop1)
    {
        for (loop2 = 0; loop2 < UINT64_MAX; ++loop2)
        {
            sizeToEnque = (rand() % strLen) + 1;
            if (sizeToEnque > pEnd - pCur)
                sizeToEnque = (int)(pEnd - pCur);

            freeSize = ringBuf.GetFreeSize();
            if (sizeToEnque > freeSize)
                sizeToEnque = freeSize;

            directEnqueSize = ringBuf.DirectEnqueueSize();
            if (sizeToEnque > directEnqueSize)
            {
                remainSize = sizeToEnque;

                memcpy(ringBuf.GetRearPtr(), pCur, directEnqueSize);

                moveRearSize = ringBuf.MoveRear(directEnqueSize);
                if (moveRearSize != directEnqueSize)
                    DebugBreak();

                pCur += directEnqueSize;
                remainSize -= directEnqueSize;

                memcpy(ringBuf.GetRearPtr(), pCur, remainSize);

                moveRearSize = ringBuf.MoveRear(remainSize);
                if (moveRearSize != remainSize)
                    DebugBreak();

                pCur += remainSize;
                if (pCur == pEnd)
                    pCur = testString;
                else if (pCur > pEnd)
                    DebugBreak();
            }
            else
            {
                memcpy(ringBuf.GetRearPtr(), pCur, sizeToEnque);

                moveRearSize = ringBuf.MoveRear(sizeToEnque);
                if (moveRearSize != sizeToEnque)
                    DebugBreak();

                pCur += sizeToEnque;
                if (pCur == pEnd)
                    pCur = testString;
                else if (pCur > pEnd)
                    DebugBreak();
            }

            sizeToDeque = (int)(((rand() % strLen) + 1) * 0.65);

            useSize = ringBuf.GetUseSize();
            if (sizeToDeque > useSize)
                sizeToDeque = useSize;

            directDequeSize = ringBuf.DirectDequeueSize();
            if (sizeToDeque > directDequeSize)
            {
                remainSize = sizeToDeque;
                tempBuf = buf;

                memcpy(tempBuf, ringBuf.GetFrontPtr(), directDequeSize);

                moveFrontSize = ringBuf.MoveFront(directDequeSize);
                if (moveFrontSize != directDequeSize)
                    DebugBreak();

                tempBuf += directDequeSize;
                remainSize -= directDequeSize;

                memcpy(tempBuf, ringBuf.GetFrontPtr(), remainSize);

                moveFrontSize = ringBuf.MoveFront(remainSize);
                if (moveFrontSize != remainSize)
                    DebugBreak();

                tempBuf[remainSize] = '\0';

                printf(buf);
            }
            else
            {
                memcpy(buf, ringBuf.GetFrontPtr(), sizeToDeque);

                moveFrontSize = ringBuf.MoveFront(sizeToDeque);
                if (moveFrontSize != sizeToDeque)
                    DebugBreak();

                buf[sizeToDeque] = '\0';
                printf(buf);
            }
        }
    }
}

void TestPerformance()
{
    {
        PRO_RESET();
        TestPerformance1(300);
        printf("\n\n");
        TestPerformance2(300);
        printf("\n\n");

        PRO_DATAOUTTEXT(L"300.txt");
    }

    {
        PRO_RESET();
        TestPerformance1(500);
        printf("\n\n");
        TestPerformance2(500);
        printf("\n\n");

        PRO_DATAOUTTEXT(L"500.txt");
    }

    {
        PRO_RESET();
        TestPerformance1(1000);
        printf("\n\n");
        TestPerformance2(1000);
        printf("\n\n");

        PRO_DATAOUTTEXT(L"1000.txt");
    }
}

void TestPerformance1(int _size)
{
    pCur = testString;

    constexpr int BUF_SIZE = 2000;
    RingBuffer ringBuf(BUF_SIZE);

    char* buf = new char[strLen + 1];

    int useSize;
    int freeSize;

    int enqueSize;
    int dequeSize;

    for (int i = 0; i < 10000; ++i)
    {
        int sizeToEnque = _size;
        int sizeToDeque = _size;

        if (sizeToEnque > pEnd - pCur)
        {
            sizeToEnque = (int)(pEnd - pCur);
            sizeToDeque = sizeToEnque;
        }

        enqueSize = ringBuf.Enqueue(pCur, sizeToEnque);
        if (enqueSize != sizeToEnque)
            DebugBreak();

        pCur += sizeToEnque;
        if (pCur == pEnd)
            pCur = testString;

        PRO_BEGIN(L"Dequeue");
        dequeSize = ringBuf.Dequeue(buf, sizeToDeque);
        if (dequeSize != sizeToDeque)
            DebugBreak();
        PRO_END(L"Dequeue");

        buf[dequeSize] = '\0';

        printf(buf);
    }

    delete[] buf;
}

void TestPerformance2(int _size)
{
    pCur = testString;

    constexpr int BUF_SIZE = 2000;
    RingBuffer ringBuf(BUF_SIZE);

    char* buf = new char[strLen + 1];
    char* tempBuf;

    int useSize;
    int freeSize;

    int enqueSize;
    int dequeSize;

    int moveFrontSize;

    int directDequeSize;

    for (int i = 0; i < 10000; ++i)
    {
        int sizeToEnque = _size;
        int sizeToDeque = _size;

        if (sizeToEnque > pEnd - pCur)
        {
            sizeToEnque = (int)(pEnd - pCur);
            sizeToDeque = sizeToEnque;
        }

        enqueSize = ringBuf.Enqueue(pCur, sizeToEnque);
        if (enqueSize != sizeToEnque)
            DebugBreak();

        pCur += sizeToEnque;
        if (pCur == pEnd)
            pCur = testString;

        PRO_BEGIN(L"MoveFront");
        directDequeSize = ringBuf.DirectDequeueSize();

        if (sizeToDeque > directDequeSize)
        {
            memcpy(buf, ringBuf.GetFrontPtr(), directDequeSize);
            moveFrontSize = ringBuf.MoveFront(directDequeSize);
            if (moveFrontSize != directDequeSize)
                DebugBreak();

            dequeSize = sizeToDeque - directDequeSize;

            memcpy(buf + directDequeSize, ringBuf.GetFrontPtr(), dequeSize);
            moveFrontSize = ringBuf.MoveFront(dequeSize);
            if (moveFrontSize != dequeSize)
                DebugBreak();
        }
        else
        {
            memcpy(buf, ringBuf.GetFrontPtr(), sizeToDeque);
            moveFrontSize = ringBuf.MoveFront(sizeToDeque);
            if (moveFrontSize != sizeToDeque)
                DebugBreak();
        }
        PRO_END(L"MoveFront");

        buf[sizeToDeque] = '\0';

        printf(buf);
    }

    delete[] buf;
}
