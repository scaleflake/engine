#ifndef WINDOWS_H
#define WINDOWS_H

void InitializeWindow();
bool WindowShouldClose();
void SwapBuffers();

bool GetKey(int key);
bool GetMouseButton(int button);
void GetCursorPos(double* xpos, double* ypos);

#endif // WINDOWS_H
