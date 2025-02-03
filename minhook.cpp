#include "pch.h"
#include <iostream>
#include <windows.h>
#include <MinHook.h>
#include <thread>
#include <vector>
#include <glew.h>
#include <glfw3.h>
#include <tchar.h>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/scalar_constants.hpp>

typedef BOOL(APIENTRY* twglSwapBuffers)(_In_ HDC hDc);
typedef void(APIENTRY* tglDrawElements)(GLenum mode, GLsizei count, GLenum type, const void* indices);
typedef void(APIENTRY* tglUniform4fv)(GLint location, GLsizei count, const GLfloat* value);
typedef void(APIENTRY* tglGetQueryObjectiv)(GLuint id, GLenum pname, GLint* params);
typedef void(APIENTRY* tglGetQueryObjectuiv)(GLuint id, GLenum pname, GLuint* params);
typedef void(APIENTRY* tglUniform4ui)(GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3);
typedef PROC(APIENTRY* func_wglGetProcAddress_t)(LPCSTR lpszProc);

static twglSwapBuffers owglSwapBuffers = nullptr;
static tglDrawElements oglDrawElements = nullptr;
static tglUniform4fv oglUniform4fv = nullptr;
static tglGetQueryObjectiv oglGetQueryObjectiv = nullptr;
static tglGetQueryObjectuiv oglGetQueryObjectuiv = nullptr;
static tglUniform4ui oglUniform4ui = nullptr;
static func_wglGetProcAddress_t owglGetProcAddress = nullptr;

static tglDrawElements oglDrawElementsHook = nullptr;
static tglUniform4fv oglUniform4fvHook = nullptr;
static tglGetQueryObjectiv oglGetQueryObjectivHook = nullptr;
static tglGetQueryObjectuiv oglGetQueryObjectuivHook = nullptr;
static tglUniform4ui oglUniform4uiHook = nullptr;

static bool FirstInit = false;
static GLuint greenTexture = 0;
static bool uniform4fvCalled = false;
static bool arta = false;
static bool sat = false;
static bool drawBoxFlag = false;

void drawBox();
void chaaamsd();

BOOL APIENTRY hwglSwapBuffers(_In_ HDC hDc) {
    if (!FirstInit) {
        glewExperimental = GL_TRUE;
        if (glewInit() != GLEW_OK) {
            std::cerr << "GLEW initialization failed!" << std::endl;
        }
        FirstInit = true;
    }
    return owglSwapBuffers(hDc);
}

void drawBox() {
    GLfloat depthRange[2];
    glGetFloatv(GL_DEPTH_RANGE, depthRange);

    GLint blendSrc, blendDst;
    glGetIntegerv(GL_BLEND_SRC_ALPHA, &blendSrc);
    glGetIntegerv(GL_BLEND_DST_ALPHA, &blendDst);

    GLfloat blendColor[4];
    glGetFloatv(GL_BLEND_COLOR, blendColor);

    GLfloat lineWidth;
    glGetFloatv(GL_LINE_WIDTH, &lineWidth);

    GLboolean blendEnabled = glIsEnabled(GL_BLEND);
    GLboolean depthTestEnabled = glIsEnabled(GL_DEPTH_TEST);

    glDepthRangef(1, 0.0);
    glLineWidth(2.0);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_CONSTANT_COLOR);
    glBlendColor(0.0f, 1.0f, 0.0f, 0.0f);
    glDisable(GL_DEPTH_TEST);
    glBegin(GL_LINES);
    {
        glVertex3f(0.1f, -0.1f, 0.1f);
        glVertex3f(0.1f, 0.1f, 0.1f);

        glVertex3f(-0.1f, -0.1f, 0.1f);
        glVertex3f(0.1f, -0.1f, 0.1f);

        glVertex3f(-0.1f, -0.1f, 0.1f);
        glVertex3f(-0.1f, 0.1f, 0.1f);

        glVertex3f(-0.1f, 0.1f, 0.1f);
        glVertex3f(0.1f, 0.1f, 0.1f);

        glVertex3f(0.1f, 0.1f, 0.1f);
        glVertex3f(0.1f, 0.1f, -0.1f);

        glVertex3f(0.1f, 0.1f, -0.1f);
        glVertex3f(0.1f, -0.1f, -0.1f);

        glVertex3f(0.1f, 0.1f, -0.1f);
        glVertex3f(-0.1f, 0.1f, -0.1f);

        glVertex3f(-0.1f, 0.1f, -0.1f);
        glVertex3f(-0.1f, 0.1f, 0.1f);

        glVertex3f(-0.1f, 0.1f, -0.1f);
        glVertex3f(-0.1f, -0.1f, -0.1f);

        glVertex3f(-0.1f, -0.1f, -0.1f);
        glVertex3f(0.1f, -0.1f, -0.1f);

        glVertex3f(-0.1f, -0.1f, -0.1f);
        glVertex3f(-0.1f, -0.1f, 0.1f);

        glVertex3f(0.1f, -0.1f, -0.1f);
        glVertex3f(0.1f, -0.1f, 0.1f);
    }
    glEnd();
    glDepthRangef(depthRange[0], depthRange[1]);
    glBlendFunc(blendSrc, blendDst);
    glBlendColor(blendColor[0], blendColor[1], blendColor[2], blendColor[3]);
    glLineWidth(lineWidth);

    if (blendEnabled == GL_TRUE) {
        glEnable(GL_BLEND);
    }
    else {
        glDisable(GL_BLEND);
    }

    if (depthTestEnabled == GL_TRUE) {
        glEnable(GL_DEPTH_TEST);
    }
    else {
        glDisable(GL_DEPTH_TEST);
    }
}

void mglGetQueryObjectivHook(GLuint id, GLenum pname, GLint* params) {
    oglGetQueryObjectiv(id, pname, params);
}

void mglGetQueryObjectuivHook(GLuint id, GLenum pname, GLuint* params) {
    oglGetQueryObjectuiv(id, pname, params);
    if (pname == GL_QUERY_RESULT_AVAILABLE)
        *params = GL_TRUE;
    else if (pname == GL_QUERY_RESULT)
        *params = 1;
}
//
//void chaaamsd() {
//    glDisable(GL_DEPTH_TEST);
//    arta = true;
//    if (greenTexture == 0) {
//        glGenTextures(1, &greenTexture);
//        glBindTexture(GL_TEXTURE_2D, greenTexture);
//        unsigned char greenPixel[] = { 0, 255, 0, 255 };
//        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, greenPixel);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//    }
//    glBindTexture(GL_TEXTURE_2D, greenTexture);
//}

void mglUniform4fv(GLint location, GLsizei count, const GLfloat* value) {
    if (location == 20)
    {
        if (count == 8 || count == 6 || count == 4) {
            arta = true;
        }
    }
    if (count > 24)
    {
        if (location == 20 || location == 19 || location == 26 || location == 22 || location == 21)
        {
            uniform4fvCalled = true;
        }
    }
    if (location == 20)
    {
        if (count == 1) {
            sat = true;
        }
    }
    oglUniform4fv(location, count, value);
}

void mglUniform4ui(GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3) {
    drawBoxFlag = true;
    oglUniform4ui(location, v0, v1, v2, v3);
}

void mglDrawElements(GLenum mode, GLsizei count, GLenum type, const void* indices) {
    if (drawBoxFlag && count == 3918) {
        drawBox();
        drawBoxFlag = false;
    }

    if (sat) {
        if (count == 5580) {
            GLfloat depthRange[2];
            glGetFloatv(GL_DEPTH_RANGE, depthRange);

            GLint blendSrc, blendDst;
            glGetIntegerv(GL_BLEND_SRC_ALPHA, &blendSrc);
            glGetIntegerv(GL_BLEND_DST_ALPHA, &blendDst);

            GLfloat blendColor[4];
            glGetFloatv(GL_BLEND_COLOR, blendColor);

            GLfloat lineWidth;
            glGetFloatv(GL_LINE_WIDTH, &lineWidth);


            GLboolean blendEnabled = glIsEnabled(GL_BLEND);
            GLboolean depthTestEnabled = glIsEnabled(GL_DEPTH_TEST);

            oglDrawElements(mode, count, type, indices);


            glDepthRangef(1, 0);
            glLineWidth(3.8);
            glEnable(GL_BLEND);
            glBlendFunc(GL_CONSTANT_COLOR, GL_CONSTANT_ALPHA);
            glBlendColor(0, 0, 0, 1);

            oglDrawElements(GL_TRIANGLES, count, type, indices);


            glDepthRangef(1, 0.5);
            glBlendColor(1, 0, 0, 1);
            oglDrawElements(GL_LINES, count, type, indices);

            glDepthRangef(depthRange[0], depthRange[1]);
            glBlendFunc(blendSrc, blendDst);
            glBlendColor(blendColor[0], blendColor[1], blendColor[2], blendColor[3]);
            glLineWidth(lineWidth);

            if (blendEnabled == GL_TRUE) {
                glEnable(GL_BLEND);
            }
            else {
                glDisable(GL_BLEND);
            }

            if (depthTestEnabled == GL_TRUE) {
                glEnable(GL_DEPTH_TEST);
            }
            else {
                glDisable(GL_DEPTH_TEST);
            }
        }
        sat = false;
    }

    if (arta) {
        if (count == 5580 || count == 1632 || count == 3318 || count == 5364) {
            GLfloat depthRange[2];
            glGetFloatv(GL_DEPTH_RANGE, depthRange);

            GLint blendSrc, blendDst;
            glGetIntegerv(GL_BLEND_SRC_ALPHA, &blendSrc);
            glGetIntegerv(GL_BLEND_DST_ALPHA, &blendDst);

            GLfloat blendColor[4];
            glGetFloatv(GL_BLEND_COLOR, blendColor);

            GLfloat lineWidth;
            glGetFloatv(GL_LINE_WIDTH, &lineWidth);


            GLboolean blendEnabled = glIsEnabled(GL_BLEND);
            GLboolean depthTestEnabled = glIsEnabled(GL_DEPTH_TEST);

            oglDrawElements(mode, count, type, indices);

            glDepthRangef(1, 0);
            glLineWidth(3.8);
            glEnable(GL_BLEND);
            glBlendFunc(GL_CONSTANT_COLOR, GL_CONSTANT_ALPHA);
            glBlendColor(0, 0, 0, 1);

            oglDrawElements(GL_TRIANGLES, count, type, indices);

            glDepthRangef(1, 0.5);
            glBlendColor(1, 0, 0, 1);
            oglDrawElements(GL_LINES, count, type, indices);

            glDepthRangef(depthRange[0], depthRange[1]);
            glBlendFunc(blendSrc, blendDst);
            glBlendColor(blendColor[0], blendColor[1], blendColor[2], blendColor[3]);
            glLineWidth(lineWidth);

            if (blendEnabled == GL_TRUE) {
                glEnable(GL_BLEND);
            }
            else {
                glDisable(GL_BLEND);
            }

            if (depthTestEnabled == GL_TRUE) {
                glEnable(GL_DEPTH_TEST);
            }
            else {
                glDisable(GL_DEPTH_TEST);
            }
        }
        arta = false;
    }

    if (uniform4fvCalled && count != 1242 && count != 1308 && count != 402 && count != 13608 && count != 20280 && count != 37410) {
        GLfloat depthRange[2];
        glGetFloatv(GL_DEPTH_RANGE, depthRange);

        GLint blendSrc, blendDst;
        glGetIntegerv(GL_BLEND_SRC_ALPHA, &blendSrc);
        glGetIntegerv(GL_BLEND_DST_ALPHA, &blendDst);

        GLfloat blendColor[4];
        glGetFloatv(GL_BLEND_COLOR, blendColor);

        GLfloat lineWidth;
        glGetFloatv(GL_LINE_WIDTH, &lineWidth);

        GLboolean blendEnabled = glIsEnabled(GL_BLEND);
        GLboolean depthTestEnabled = glIsEnabled(GL_DEPTH_TEST);

        oglDrawElements(mode, count, type, indices);

        glDepthRangef(1, 0);
        glLineWidth(3.8);
        glEnable(GL_BLEND);
        glBlendFunc(GL_CONSTANT_COLOR, GL_CONSTANT_ALPHA);
        glBlendColor(0, 0, 0, 1);

        oglDrawElements(GL_TRIANGLES, count, type, indices);

        glDepthRangef(1, 0.5);
        glBlendColor(1, 0, 0, 1);
        oglDrawElements(GL_LINES, count, type, indices);

        glDepthRangef(depthRange[0], depthRange[1]);
        glBlendFunc(blendSrc, blendDst);
        glBlendColor(blendColor[0], blendColor[1], blendColor[2], blendColor[3]);
        glLineWidth(lineWidth);

        if (blendEnabled == GL_TRUE) {
            glEnable(GL_BLEND);
        }
        else {
            glDisable(GL_BLEND);
        }

        if (depthTestEnabled == GL_TRUE) {
            glEnable(GL_DEPTH_TEST);
        }
        else {
            glDisable(GL_DEPTH_TEST);
        }
        uniform4fvCalled = false;
    }
    else {
        oglDrawElements(mode, count, type, indices);
    }
}

PROC APIENTRY hwglGetProcAddress(LPCSTR ProcName) {
    HMODULE hMod = GetModuleHandle(L"opengl32.dll");

    if (!strcmp(ProcName, "glDrawElements")) {
        if (!oglDrawElementsHook) {
            MH_Initialize();
            oglDrawElementsHook = (tglDrawElements)owglGetProcAddress(ProcName);
            if (MH_CreateHook(oglDrawElementsHook, mglDrawElements, (void**)&oglDrawElements) != MH_OK) {
                std::cerr << "Failed to create hook for glDrawElements" << std::endl;
            }
            MH_EnableHook(oglDrawElementsHook);
        }
    }
    else if (!strcmp(ProcName, "glUniform4fv")) {
        if (!oglUniform4fvHook) {
            MH_Initialize();
            oglUniform4fvHook = (tglUniform4fv)owglGetProcAddress(ProcName);
            if (MH_CreateHook(oglUniform4fvHook, mglUniform4fv, (void**)&oglUniform4fv) != MH_OK) {
                std::cerr << "Failed to create hook for glUniform4fv" << std::endl;
            }
            MH_EnableHook(oglUniform4fvHook);
        }
    }
    else if (!strcmp(ProcName, "glUniform4ui")) {
        if (!oglUniform4uiHook) {
            MH_Initialize();
            oglUniform4uiHook = (tglUniform4ui)owglGetProcAddress(ProcName);
            if (MH_CreateHook(oglUniform4uiHook, mglUniform4ui, (void**)&oglUniform4ui) != MH_OK) {
                std::cerr << "Failed to create hook for glUniform4ui" << std::endl;
            }
            MH_EnableHook(oglUniform4uiHook);
        }
    }
    else if (!strcmp(ProcName, "glGetQueryObjectiv")) {
        if (!oglGetQueryObjectivHook) {
            MH_Initialize();
            oglGetQueryObjectivHook = (tglGetQueryObjectiv)owglGetProcAddress(ProcName);
            if (MH_CreateHook(oglGetQueryObjectivHook, mglGetQueryObjectivHook, (void**)&oglGetQueryObjectiv) != MH_OK) {
                std::cerr << "Failed to create hook for glGetQueryObjectiv" << std::endl;
            }
            MH_EnableHook(oglGetQueryObjectivHook);
        }
    }
    else if (!strcmp(ProcName, "glGetQueryObjectuiv")) {
        if (!oglGetQueryObjectuivHook) {
            MH_Initialize();
            oglGetQueryObjectuivHook = (tglGetQueryObjectuiv)owglGetProcAddress(ProcName);
            if (MH_CreateHook(oglGetQueryObjectuivHook, mglGetQueryObjectuivHook, (void**)&oglGetQueryObjectuiv) != MH_OK) {
                std::cerr << "Failed to create hook for glGetQueryObjectuiv" << std::endl;
            }
            MH_EnableHook(oglGetQueryObjectuivHook);
        }
    }

    return owglGetProcAddress(ProcName);
}

DWORD WINAPI OpenglInit(__in  LPVOID lpParameter) {
    while (GetModuleHandle(L"opengl32.dll") == 0) {
        Sleep(100);
    }

    HMODULE hMod = GetModuleHandle(L"opengl32.dll");
    if (hMod) {
        void* ptr = GetProcAddress(hMod, "wglSwapBuffers");
        if (ptr) {
            MH_Initialize();
            if (MH_CreateHook(ptr, hwglSwapBuffers, reinterpret_cast<void**>(&owglSwapBuffers)) != MH_OK) {
                std::cerr << "Failed to create hook for wglSwapBuffers" << std::endl;
            }
            MH_EnableHook(ptr);
        }
        else {
            std::cerr << "Failed to get wglSwapBuffers address" << std::endl;
            return 1;
        }

        auto wglGetProcAddressFunc = (func_wglGetProcAddress_t)GetProcAddress(hMod, "wglGetProcAddress");
        if (wglGetProcAddressFunc)
        {
            if (MH_CreateHook(wglGetProcAddressFunc, hwglGetProcAddress, (void**)&owglGetProcAddress) != MH_OK)
            {
                std::cerr << "Failed to create hook for wglGetProcAddress" << std::endl;
            }
            MH_EnableHook(wglGetProcAddressFunc);
        }
        else {
            std::cerr << "Failed to get wglGetProcAddress address" << std::endl;
            return 1;
        }
    }
    return 1;
}



BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        CreateThread(0, 0, LPTHREAD_START_ROUTINE(OpenglInit), hModule, 0, 0);
        break;
    }
    return TRUE;
}
