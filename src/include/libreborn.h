/*
 * libreborn.h - Helpers and utilities extracted from the old libreborn
 * 
 * Copyright (c) 2021 TheBrokenRail
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * 
 */

#ifndef LIBREBORN_H
#define LIBREBORN_H

/* Hook Library Function */
#define HOOK(name, return_type, args) \
	typedef return_type (*name##_t)args; \
	static name##_t real_##name = NULL; \
	static void ensure_##name() \
	{ \
		if (!real_##name) \
		{ \
			dlerror(); \
			real_##name = (name##_t)dlsym(RTLD_NEXT, #name); \
			if (!real_##name) \
			{ \
				fprintf("ERROR: Resolving Symbol: "#name", %s", dlerror()); \
			} \
		} \
	}; \
	return_type name args

#endif /* LIBREBORN_H */
