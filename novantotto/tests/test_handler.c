/*
 * Novantotto
 * Copyright (c) 2024 Andrea Bonomi
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <stdio.h>
#include "../windefs.h"
#include "../handler.h"
#include "miniunit.h"

int           tests_run = 0;
HANDLE_ENTRY *handle;
HANDLE_ENTRY *handle2;
HANDLE_ENTRY *handle3;

static char *test_allocate_handler()
{
    handle = AllocateHandle(Window, NULL);
    mu_assert("error, handle is NULL", handle != NULL);
    return NULL;
}

static char *test_allocate_handler2()
{
    handle2 = AllocateHandle(Window, NULL);
    mu_assert("error, handle2 is NULL", handle2 != NULL);
    return NULL;
}

static char *test_allocate_handler3()
{
    handle3 = AllocateHandle(DlgItem, handle2);
    mu_assert("error, handle3 is NULL", handle3 != NULL);
    return NULL;
}

static char *test_not_found()
{
    HANDLE t = GetProperty(handle->window.props, "test");
    mu_assert("error, GetProperty is not NULL", t == NULL);
    return NULL;
}

static char *test_test_len1()
{
    DelProperty(handle->window.props, "test");
    SetProperty(handle->window.props, "test", (HANDLE)1);
    mu_assert("error, len is not 1", handle->window.props->len == 1);
    return NULL;
}

static char *test_get_property()
{
    HANDLE *t = GetProperty(handle->window.props, "test");
    mu_assert("error, GetProperty test is not 1", t == (HANDLE)1);
    return NULL;
}

static char *test_get_property2()
{
    SetProperty(handle->window.props, "test2", (HANDLE)2);
    mu_assert("error, len is not 2", handle->window.props->len == 2);
    HANDLE *t = GetProperty(handle->window.props, "test");
    mu_assert("error, GetProperty test is not 1", t == (HANDLE)1);
    t = GetProperty(handle->window.props, "test2");
    mu_assert("error, GetProperty test2 is not 2", t == (HANDLE)2);
    return NULL;
}

static char *test_del_properties()
{
    DelProperty(handle->window.props, "not-existing");
    DelProperty(handle->window.props, "test");
    mu_assert("error, len is not 1", handle->window.props->len == 1);
    HANDLE *t = GetProperty(handle->window.props, "test");
    mu_assert("error, GetProperty is not NULL", t == NULL);
    DelProperty(handle->window.props, "test2");
    mu_assert("error, len is not 0", handle->window.props->len == 0);
    DelProperty(handle->window.props, "test2");
    mu_assert("error, len is not 0", handle->window.props->len == 0);
    return NULL;
}

static char *test_set_many()
{
    int  i;
    int  c = handle2->window.props->capacity * 2;
    char buf[20];
    for (i = 0; i < c; i++)
    {
        snprintf(buf, sizeof(buf), "p%d", i);
        SetProperty(handle2->window.props, buf, (HANDLE)i);
    }
    mu_assert("error, len is not c", handle2->window.props->len == c);
    mu_assert("error, len is not <= capacity", handle2->window.props->len <= handle2->window.props->capacity);
    for (i = 0; i < c; i++)
    {
        snprintf(buf, sizeof(buf), "p%d", i);
        HANDLE *t = GetProperty(handle2->window.props, buf);
        mu_assert("error, wrong property value", (int)t == i);
    }
    return NULL;
}

static char *test_release_handle()
{
    ReleaseHandle((HANDLE)handle);
    SetProperty(handle2->window.props, "aaaa", (HANDLE)1111);
    SetProperty(handle2->window.props, "bbbb", (HANDLE)2222);
    SetProperty(handle2->window.props, "cccc", (HANDLE)3333);
    ReleaseHandle((HANDLE)handle2);
    ReleaseHandle((HANDLE)handle3);
    return NULL;
}

static char *test_release_empty()
{
    HANDLE_ENTRY *handle = AllocateHandle(Window, NULL);
    ReleaseHandle((HANDLE)handle);
    return NULL;
}

static char *test_get_dlg_item()
{
    HANDLE_ENTRY *handle = AllocateHandle(Window, NULL);
    HANDLE_ENTRY *i1 = AllocateHandle(DlgItem, (HWND)handle);
    i1->dlgItem.nIDDlgItem = 1001;
    HANDLE_ENTRY *i2 = AllocateHandle(DlgItem, (HWND)handle);
    i2->dlgItem.nIDDlgItem = 1002;
    HANDLE_ENTRY *i3 = AllocateHandle(DlgItem, (HWND)handle2);
    i3->dlgItem.nIDDlgItem = 1001;
    mu_assert("error, GetDlgItem 1001 != i1", GetDlgItem((HWND)handle, 1001) == i1);
    mu_assert("error, GetDlgItem 1001 parent != handle", ((HANDLE_ENTRY *)GetDlgItem((HWND)handle, 1001))->hwndParent == (HWND)handle);
    mu_assert("error, GetDlgItem 1002 != i2", GetDlgItem((HWND)handle, 1002) == i2);
    mu_assert("error, GetDlgItem 1001 != i1", GetDlgItem((HWND)handle2, 1001) == i3);
    mu_assert("error, GetDlgItem 1111 != NULL", GetDlgItem((HWND)handle, 1111) == NULL);
    ReleaseHandle((HANDLE)handle);
    mu_assert("error, GetDlgItem 1001 != NULL", GetDlgItem((HWND)handle, 1001) == NULL);
    return NULL;
}

static char *all_tests()
{
    mu_run_test(test_allocate_handler);
    mu_run_test(test_allocate_handler2);
    mu_run_test(test_allocate_handler3);
    mu_run_test(test_not_found);
    mu_run_test(test_test_len1);
    mu_run_test(test_get_property);
    mu_run_test(test_get_property2);
    mu_run_test(test_del_properties);
    mu_run_test(test_set_many);
    mu_run_test(test_release_handle);
    mu_run_test(test_release_empty);
    mu_run_test(test_get_dlg_item);
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
