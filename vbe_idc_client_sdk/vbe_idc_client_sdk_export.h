#ifndef VBE_IDC_CLIENT_SDK_EXPORT_H_43280831943276432765326576
#define VBE_IDC_CLIENT_SDK_EXPORT_H_43280831943276432765326576

#ifdef WIN32
#ifdef DECLARE_VBE_IDC_CLIENT_SDK_EXPORT
#define VBE_IDC_CLIENT_SDK_EXPORT __declspec(dllexport)
#else
#define VBE_IDC_CLIENT_SDK_EXPORT __declspec(dllimport)
#endif
#else
#ifdef __GNUC__
#define VBE_IDC_CLIENT_SDK_EXPORT __attribute__ ((visibility("default")))
#else
#define VBE_IDC_CLIENT_SDK_EXPORT
#endif
#endif

#endif
