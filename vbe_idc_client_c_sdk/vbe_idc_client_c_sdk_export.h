#ifndef VBE_IDC_CLIENT_C_SDK_EXPORT_H_328928987643289
#define VBE_IDC_CLIENT_C_SDK_EXPORT_H_328928987643289

#ifdef WIN32
#ifdef DECLARE_VBE_IDC_C_CLIENT_SDK_EXPORT
#define VBE_IDC_CLIENT_C_SDK_EXPORT __declspec(dllexport)
#else
#define VBE_IDC_CLIENT_C_SDK_EXPORT __declspec(dllimport)
#endif
#else
#ifdef __GNUC__
#define VBE_IDC_CLIENT_C_SDK_EXPORT __attribute__ ((visibility("default")))
#else
#define VBE_IDC_CLIENT_C_SDK_EXPORT
#endif
#endif

#endif
