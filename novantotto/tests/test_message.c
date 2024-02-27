#include <stdio.h>
#include "../windefs.h"
#include "../handler.h"
#include "miniunit.h"

int           tests_run = 0;
HANDLE_ENTRY *hwnd;
HANDLE_ENTRY *hwnd2;

static char *test_post_messages()
{
    hwnd = AllocateHandle(Window, NULL);
    mu_assert("error, hwnd is NULL", hwnd != NULL);
    for (int i = 0; i < MESSAGE_LIMIT; i++)
    {
        printf("%d\n", i);
        BOOL t = PostMessage(hwnd, WM_USER, i, i * 2);
        mu_assert("error, queue is full", t);
    }
    BOOL t = PostMessage(hwnd, WM_USER, 0, 0);
    mu_assert("error, queue is not full", !t);
    return NULL;
}

static char *test_peek_messages()
{
    MSG  msg;
    BOOL t;
    for (int i = 0; i < MESSAGE_LIMIT; i++)
    {
        t = PeekMessage(&msg, hwnd, 0, 0, PM_REMOVE);
        mu_assert("error, message != WM_USER", msg.message == WM_USER);
        mu_assert("error, wParam != i", msg.wParam == i);
        mu_assert("error, lParam != i * 2", msg.lParam == i * 2);
    }
    t = PostMessage(hwnd, WM_USER, 9999, 0);
    mu_assert("error, queue is full", t);
    t = PeekMessage(&msg, hwnd, 0, 0, PM_REMOVE);
    mu_assert("error, queue is empty", t);
    mu_assert("error, wParam != 9999", msg.wParam == 9999);
    return NULL;
}

extern void PrintMessages();

static char *test_two_handlers()
{
    MSG  msg;
    BOOL t;
    hwnd2 = AllocateHandle(Window, NULL);
    mu_assert("error, hwnd2 is NULL", hwnd2 != NULL);
    t = PostMessage(hwnd, WM_USER, 1, 0);
    mu_assert("error, queue is full", t);
    t = PostMessage(hwnd2, WM_USER, 2, 0);
    mu_assert("error, queue is full", t);

    PrintMessages();
    t = PeekMessage(&msg, hwnd2, 0, 0, PM_REMOVE);
    mu_assert("error, msg not found - hwnd2", t);
    mu_assert("error, message != WM_USER", msg.message == WM_USER);
    mu_assert("error, wParam != 2", msg.wParam == 2);
    PrintMessages();

    t = PeekMessage(&msg, hwnd, 0, 0, PM_REMOVE);
    mu_assert("error, msg not found - hwnd", t);
    mu_assert("error, message != WM_USER", msg.message == WM_USER);
    mu_assert("error, wParam != 1", msg.wParam == 1);
    return NULL;
}

static char *all_tests()
{
    mu_run_test(test_post_messages);
    mu_run_test(test_peek_messages);
    mu_run_test(test_two_handlers);
    return 0;
}

int main(int argc, char **argv)
{
    char *result = all_tests();
    if (result != 0)
    {
        printf("%s\n", result);
    }
    else
    {
        printf("ALL TESTS PASSED\n");
    }
    printf("Tests run: %d\n", tests_run);

    return result != 0;
}
