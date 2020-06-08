/*
  Copyright (C) 2020 by Shohei YOSHIDA

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#define _DEFAULT_SOURCE
#include <emacs-module.h>

#include <qr.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int plugin_is_GPL_compatible;

static char *retrieve_string(emacs_env *env, emacs_value str, ptrdiff_t *size) {
    *size = 0;

    env->copy_string_contents(env, str, NULL, size);
    char *p = malloc(*size);
    if (p == NULL) {
        *size = 0;
        return NULL;
    }
    env->copy_string_contents(env, str, p, size);

    return p;
}

emacs_value Fqrcode_native_core_display_string(emacs_env *env, ptrdiff_t nargs, emacs_value args[], void *data) {
    ptrdiff_t input_size = 0;
    char *str = retrieve_string(env, args[0], &input_size);
    if (str == NULL) {
        return env->intern(env, "nil");
    }

    QRCode *qr = qrInit(10, QR_EM_8BIT, 2, -1, NULL);
    if (qr == NULL) {
        free(str);
        return env->intern(env, "nil");
    }

    qrAddData(qr, (const qr_byte_t *)str, input_size - 1);
    if (!qrFinalize(qr)) {
        free(str);
        return env->intern(env, "nil");
    }

    int size = 0;
    qr_byte_t *buffer = qrSymbolToPBM(qr, 5, 5, &size);
    free(str);
    qrDestroy(qr);
    if (buffer == NULL) {
        return env->intern(env, "nil");
    }

    emacs_value ret = env->make_string(env, (const char *)buffer, size);
    return ret;
}

static void bind_function(emacs_env *env, const char *name, emacs_value Sfun) {
    emacs_value Qfset = env->intern(env, "fset");
    emacs_value Qsym = env->intern(env, name);
    emacs_value args[] = {Qsym, Sfun};

    env->funcall(env, Qfset, 2, args);
}

static void provide(emacs_env *env, const char *feature) {
    emacs_value Qfeat = env->intern(env, feature);
    emacs_value Qprovide = env->intern(env, "provide");
    emacs_value args[] = {Qfeat};

    env->funcall(env, Qprovide, 1, args);
}

int emacs_module_init(struct emacs_runtime *ert) {
    emacs_env *env = ert->get_environment(ert);

#define DEFUN(lsym, csym, amin, amax, doc, data) bind_function(env, lsym, env->make_function(env, amin, amax, csym, doc, data))

    DEFUN("qrcode-native-core-display-string", Fqrcode_native_core_display_string, 1, 1, NULL, NULL);
#undef DEFUN

    provide(env, "qr-native-core");
    return 0;
}
