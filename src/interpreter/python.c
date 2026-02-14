/*
 *  python.c
 *
 *  Copyright 2025 Eyassu Mongalo <3yassu@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <Python.h>
#include <dlfcn.h>
#include "python.h"

typedef struct Kernel{
	PyObject *io;
	PyObject *ker;
} Kernel;

static Kernel ipython_kernel = {NULL, NULL};

int ipython_init(){
	dlopen("libpython3.so", RTLD_NOW | RTLD_GLOBAL);
	PyStatus status;
	PyConfig config;
	PyConfig_InitPythonConfig(&config);

	status = Py_InitializeFromConfig(&config);
	if (PyStatus_Exception(status)) {
    Py_ExitStatusException(status);
	}
	PyObject *path = PySys_GetObject("path");
	PyObject *path_name = PyUnicode_FromString("/home/emerald/Projects/Geany/Geany-nb/tmp");
	PyList_Append(path, path_name);
	PyObject *sys = PyImport_ImportModule("sys");
	PyObject *io = PyImport_ImportModule("io");
	ipython_kernel.io = PyObject_CallMethod(io, "StringIO", NULL);

	PyObject_SetAttrString(sys, "stdout", ipython_kernel.io);
	PyObject_SetAttrString(sys, "stderr", ipython_kernel.io);
	PyObject *core = PyImport_ImportModule("IPython.core.interactiveshell");
	PyObject *shell = PyObject_GetAttrString(core, "InteractiveShell");
	ipython_kernel.ker = PyObject_CallMethod(shell, "instance", NULL);
	Py_DECREF(io);
	Py_DECREF(path_name);
	Py_DECREF(sys);
	Py_DECREF(core);
	Py_DECREF(shell);
	return 0;
}

int ipython_run_keep_io(const char *str){
	PyObject *state = PyObject_CallMethod(ipython_kernel.ker, "run_cell", "si", str, 0);
	PyObject *strval = PyObject_GetAttrString(state, "success");
	int ret = PyObject_IsTrue(strval);
	Py_DECREF(state);
	Py_DECREF(strval);
	return ret;
}

char *ipython_io(){
	PyObject *output = PyObject_CallMethod(ipython_kernel.io, "getvalue", NULL);
	const char *tmp_res = PyUnicode_AsUTF8(output);
	char *res = (char *)malloc(strlen(tmp_res) + 1);
	strcpy(res, tmp_res);
	Py_DECREF(output);
	PyObject_CallMethod(ipython_kernel.io, "seek", "i", 0);
	PyObject_CallMethod(ipython_kernel.io, "truncate", "i", 0);
	return res;
}

char *ipython_run_string(const char *str){
	PyObject_CallMethod(ipython_kernel.ker, "run_cell", "si", str, 0);
	PyObject *output = PyObject_CallMethod(ipython_kernel.io, "getvalue", NULL);
	const char *tmp_res = PyUnicode_AsUTF8(output);
	char *res = (char *)malloc(strlen(tmp_res) + 1);
	strcpy(res, tmp_res);
	Py_DECREF(output);
	PyObject_CallMethod(ipython_kernel.io, "seek", "i", 0);
	PyObject_CallMethod(ipython_kernel.io, "truncate", "i", 0);
	return res;
}

void ipython_end(){
	Py_DECREF(ipython_kernel.ker);
	Py_DECREF(ipython_kernel.io);
	Py_Finalize();
}



