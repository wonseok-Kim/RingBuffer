#pragma once

typedef void(*profile_error_handler)(const wchar_t* errMsg);

void ProfileBegin(const wchar_t* tag);
void ProfileEnd(const wchar_t* tag);
void ProfileDataOutText(const wchar_t* fileName);
void ProfileReset(void);
void ProfileSetErrorHandler(profile_error_handler handler);

#define PROFILE

#ifdef PROFILE
#define PRO_BEGIN(TagName)          ProfileBegin(TagName)
#define PRO_END(TagName)            ProfileEnd(TagName)
#define PRO_DATAOUTTEXT(FileName)   ProfileDataOutText(FileName)
#define PRO_RESET()                 ProfileReset()
#else
#define PRO_BEGIN(TagName)          (void)(0)
#define PRO_END(TagName)            (void)(0)
#define PRO_DATAOUTTEXT(FileName)   (void)(0)
#define PRO_RESET()                 (void)(0)
#endif // PROFILE

class Profile
{
public:
    Profile(const wchar_t* tag)
        : m_Tag{ tag }
    {
        PRO_BEGIN(tag);
    }

    ~Profile()
    {
        PRO_END(m_Tag);
    }

private:
    const wchar_t* m_Tag;
};
