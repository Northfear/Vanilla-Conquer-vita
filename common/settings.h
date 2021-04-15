#ifndef SETTINGS_H
#define SETTINGS_H

#include <string>
class INIClass;

class SettingsClass
{
public:
    SettingsClass();

    void Load(INIClass& ini);
    void Save(INIClass& ini);

    struct
    {
        bool RawInput;
        int Sensitivity;
    } Mouse;

    struct
    {
        int WindowWidth;
        int WindowHeight;
        bool Windowed;
        bool Boxing;
        int Width;
        int Height;
        int FrameLimit;
        int InterpolationMode;
        bool HardwareCursor;
        std::string Scaler;
        std::string Driver;
        std::string PixelFormat;
    } Video;

#ifdef VITA
    struct
    {
        bool ScaleGameSurface;
        int ControllerPointerSpeed;
    } Vita;
#endif
};

extern SettingsClass Settings;

#endif
