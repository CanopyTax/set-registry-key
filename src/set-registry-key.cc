#include <node.h>
#include <windows.h>
#include <shlobj.h>
#include <Shlwapi.h>
#include <iostream>
#include <string>

std::wstring ConvertUtf8ToWide(const std::string& utf8) {
    if (utf8.empty()) {
        return std::wstring();
    }

    int len = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, NULL, 0);
    std::wstring wide(len - 1, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, &wide[0], len);

    return wide;
}

bool EnsureKeyExists(LPCWSTR subkey) {
    HKEY hKey;

    LONG openRes = RegOpenKeyExW(HKEY_CURRENT_USER, subkey, 0, KEY_ALL_ACCESS, &hKey);
    if (openRes == ERROR_SUCCESS) {
        RegCloseKey(hKey);
        return true;
    }

    if (openRes == ERROR_FILE_NOT_FOUND) {
        LONG createRes = RegCreateKeyExW(HKEY_CURRENT_USER, subkey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, NULL);
        RegCloseKey(hKey);
        return createRes == ERROR_SUCCESS;
    }

    return false;
}

void SetRegistryKey(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();

    if (args.Length() < 3) {
        isolate->ThrowException(v8::Exception::TypeError(
            v8::String::NewFromUtf8(isolate, "Wrong number of arguments").ToLocalChecked()));
        return;
    }

    if (!args[0]->IsString() || !args[1]->IsString() || !args[2]->IsString()) {
        isolate->ThrowException(v8::Exception::TypeError(
            v8::String::NewFromUtf8(isolate, "Wrong arguments").ToLocalChecked()));
        return;
    }

    v8::String::Utf8Value subkeyUtf8(isolate, args[0]);
    v8::String::Utf8Value valueNameUtf8(isolate, args[1]);
    v8::String::Utf8Value dataUtf8(isolate, args[2]);

    std::wstring subkey = ConvertUtf8ToWide(std::string(*subkeyUtf8, subkeyUtf8.length()));
    std::wstring valueName = ConvertUtf8ToWide(std::string(*valueNameUtf8, valueNameUtf8.length()));
    std::wstring data = ConvertUtf8ToWide(std::string(*dataUtf8, dataUtf8.length()));

    if (!EnsureKeyExists(subkey.c_str())) {
        args.GetReturnValue().Set(v8::Boolean::New(isolate, false));
        return;
    }

    HKEY hKey;
    LONG openRes = RegOpenKeyExW(HKEY_CURRENT_USER, subkey.c_str(), 0, KEY_ALL_ACCESS, &hKey);

    if (openRes != ERROR_SUCCESS) {
        args.GetReturnValue().Set(v8::Boolean::New(isolate, false));
        return;
    }

    LONG setRes = RegSetValueExW(hKey, valueName.c_str(), 0, REG_SZ, (const BYTE*)data.c_str(), (data.size() + 1) * sizeof(wchar_t));

    RegCloseKey(hKey);

    args.GetReturnValue().Set(v8::Boolean::New(isolate, setRes == ERROR_SUCCESS));
}

void Initialize(v8::Local<v8::Object> exports) {
    NODE_SET_METHOD(exports, "setRegistryKey", SetRegistryKey);
}

NODE_MODULE(NODE_GYP_MODULE_NAME, Initialize)
