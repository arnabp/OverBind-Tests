#include "unity.h"
#include "interception.h"
#include <windows.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct
{
  char *keycode;
  char *result_type;
  int result_value;
} KeyBinding;

InterceptionContext context;
InterceptionDevice device;

unsigned short key_A = 0x41;
unsigned short key_B = 0x42;
unsigned short key_left_arrow = 0x25;
unsigned short key_up_arrow = 0x26;
unsigned short key_right_arrow = 0x27;
unsigned short key_down_arrow = 0x28;

void WriteConfig(KeyBinding *bindings, size_t bindingsCount)
{
  printf("Writing config file\n");
  char *appDataPath = getenv("APPDATA");
  if (appDataPath == NULL)
  {
    printf("Failed to get APPDATA path.\n");
    return;
  }

  char configFilePath[1024];
  snprintf(configFilePath, sizeof(configFilePath), "%s\\OverBind\\OverBind_conf.json", appDataPath);

  FILE *file = fopen(configFilePath, "w");
  if (file == NULL)
  {
    printf("Failed to open config file.\n");
    return;
  }

  fprintf(file, "[\n");
  for (size_t i = 0; i < bindingsCount; ++i)
  {
    fprintf(file, "  { \"keycode\": \"%s\", \"result_type\": \"%s\", \"result_value\": %d }",
            bindings[i].keycode, bindings[i].result_type, bindings[i].result_value);
    if (i < bindingsCount - 1)
    {
      fprintf(file, ",\n");
    }
    else
    {
      fprintf(file, "\n");
    }
  }
  fprintf(file, "]\n");

  fclose(file);
}

void OpenOverbind()
{
  if (system("tasklist | findstr OverBind.exe") == 0)
  {
    printf("OverBind is running\n");
    system("taskkill /IM OverBind.exe /F");
  }
  else
  {
    printf("OverBind is not running\n");
  }
  system("start \"\" \"C:\\Program Files\\OverBind\\OverBind.exe\"");
}

void OpenNotepad()
{
  if (system("tasklist | findstr notepad.exe") == 0)
  {
    printf("Notepad is running\n");
    system("taskkill /IM notepad.exe /F");
  }
  else
  {
    printf("Notepad is not running\n");
  }
  system("start \"\" notepad.exe");
}

// When overbind is actually ready to go it gets refocused automatically
// So poll current focused windows until it's overbind again and then refocus notepad
void RefocusNotepad()
{
  char buffer[1024];
  while (1)
  {
    HWND hwnd = GetForegroundWindow();
    GetWindowText(hwnd, buffer, sizeof(buffer));
    if (strcmp(buffer, "OverBind") == 0)
    {
      printf("OverBind is focused\n");
      break;
    }
  }

  system("start \"\" notepad.exe");
}

// Function to simulate pressing and holding key A
void KeyDown(unsigned short key)
{
  printf("Pressing key %c\n", key);
  InterceptionKeyStroke keystroke;
  keystroke.code = key;
  keystroke.state = INTERCEPTION_KEY_DOWN;

  interception_send(context, device, (const InterceptionStroke *)&keystroke, 1);
}

// Function to release key A
void KeyUp(unsigned short key)
{
  printf("Releasing key %c\n", key);
  InterceptionKeyStroke keystroke;
  keystroke.code = key;
  keystroke.state = INTERCEPTION_KEY_UP;

  interception_send(context, device, (const InterceptionStroke *)&keystroke, 1);
}

bool IsKeyHeld(int vKey)
{
  // GetAsyncKeyState returns a short where:
  // - The least significant bit is set if the key was pressed after the previous call.
  // - The most significant bit is set if the key is currently down.
  SHORT state = GetAsyncKeyState(vKey);

  return (state & 0x8000) != 0; // Check if the most significant bit is set
}

// Test function
void test_KeyBinding_A_To_B()
{
  KeyBinding bindings[] = {
      {"41", "keyboard", 66} // A -> B
  };

  WriteConfig(bindings, sizeof(bindings) / sizeof(bindings[0]));
  OpenOverbind();
  OpenNotepad();
  RefocusNotepad();
  Sleep(30 * 1000); // Wait for OverBind to start
  TEST_ASSERT_FALSE(IsKeyHeld(key_B));
  KeyDown(key_A);
  TEST_ASSERT_TRUE(IsKeyHeld(key_B));
  KeyUp(key_A);
}

void setUp(void)
{
  context = interception_create_context();
  char hardware_id[500];

  for (device = INTERCEPTION_KEYBOARD(0); device <= INTERCEPTION_KEYBOARD(INTERCEPTION_MAX_KEYBOARD); device++)
  {
    if (interception_is_keyboard(device))
    {
      interception_get_hardware_id(context, device, hardware_id, sizeof(hardware_id));
      printf("Found keyboard device: %s\n", hardware_id);
      return;
    }
  }
  printf("No keyboard device found\n");
}

void tearDown(void)
{
  interception_destroy_context(context);
}

// Main function to run the test
int main()
{
  UNITY_BEGIN();
  RUN_TEST(test_KeyBinding_A_To_B);
  return UNITY_END();
}
