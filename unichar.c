/**
 * Unicode Character Utility Extension for PHP
 *
 * Copyright (c) 2007-2010 Ryusuke SEKIYAMA. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 * @package     php-unichar
 * @author      Ryusuke SEKIYAMA <rsky0711@gmail.com>
 * @copyright   2007-2010 Ryusuke SEKIYAMA
 * @license     http://www.opensource.org/licenses/mit-license.php  MIT License
 */

#include "php_unichar.h"
#include "zval_zval.h"
#include "unichar_entity.h"
#ifdef WITH_UNICHAR_ICU
#include "unichar_icu.h"
#endif
#include <ext/standard/php_smart_str.h>

#define raise_error(code, ...) php_error_docref(NULL TSRMLS_CC, code, __VA_ARGS__)
#define parse_parameters(type_spec, ...) \
	zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, type_spec, __VA_ARGS__)

/* {{{ module globals */

static ZEND_DECLARE_MODULE_GLOBALS(unichar)

/* }}} */

/* {{{ internal function prototypes */

static PHP_MINIT_FUNCTION(unichar);
static PHP_MSHUTDOWN_FUNCTION(unichar);
static PHP_MINFO_FUNCTION(unichar);

#if ZEND_EXTENSION_API_NO < 220060519
#define PHP_GINIT_FUNCTION(unichar) \
	void unichar_init_globals(zend_unichar_globals *unichar_globals TSRMLS_DC)
#define PHP_GSHUTDOWN_FUNCTION(unichar) \
	void unichar_shutdown_globals(zend_unichar_globals *unichar_globals TSRMLS_DC)
#endif

static PHP_GINIT_FUNCTION(unichar);
static PHP_GSHUTDOWN_FUNCTION(unichar);

/* }}} */

/* {{{ PHP function prototypes and argument informations */

static PHP_FUNCTION(unichr);
static PHP_FUNCTION(unichr_array);
static PHP_FUNCTION(uniord);
static PHP_FUNCTION(uniord_array);
static PHP_FUNCTION(unistr_validate);
static PHP_FUNCTION(unistr_ngram);
static PHP_FUNCTION(unistr_decode_entity);
static PHP_FUNCTION(unistr_rotate);
#ifdef WITH_UNICHAR_ICU
static PHP_FUNCTION(unorm_nfc);
static PHP_FUNCTION(unorm_nfd);
static PHP_FUNCTION(unorm_nfkc);
static PHP_FUNCTION(unorm_nfkd);
#endif
static PHP_FUNCTION(unichar_icu_version);

#ifdef ZEND_ENGINE_2

#ifdef WITH_UNICHAR_ICU
#define UNICHAR_ARG_INFO_GET_INFO ZEND_ARG_INFO(0, get_info)
#else
#define UNICHAR_ARG_INFO_GET_INFO
#endif

#if !defined(PHP_VERSION_ID) || PHP_VERSION_ID < 50300
#define ARG_INFO_STATIC static
#else
#define ARG_INFO_STATIC
#endif

ARG_INFO_STATIC
ZEND_BEGIN_ARG_INFO_EX(arginfo_unichr, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 1)
	ZEND_ARG_INFO(0, code)
	UNICHAR_ARG_INFO_GET_INFO
ZEND_END_ARG_INFO()

ARG_INFO_STATIC
ZEND_BEGIN_ARG_INFO_EX(arginfo_unichr_array, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 1)
	ZEND_ARG_ARRAY_INFO(0, codes, 0)
	UNICHAR_ARG_INFO_GET_INFO
ZEND_END_ARG_INFO()

ARG_INFO_STATIC
ZEND_BEGIN_ARG_INFO_EX(arginfo_uniord, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 1)
	ZEND_ARG_INFO(0, str)
	UNICHAR_ARG_INFO_GET_INFO
ZEND_END_ARG_INFO()

ARG_INFO_STATIC
ZEND_BEGIN_ARG_INFO_EX(arginfo_uniord_array, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 1)
	ZEND_ARG_INFO(0, str)
	UNICHAR_ARG_INFO_GET_INFO
ZEND_END_ARG_INFO()

ARG_INFO_STATIC
ZEND_BEGIN_ARG_INFO_EX(arginfo_unistr_validate, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 1)
	ZEND_ARG_INFO(0, str)
	ZEND_ARG_INFO(1, length)
ZEND_END_ARG_INFO()

ARG_INFO_STATIC
ZEND_BEGIN_ARG_INFO_EX(arginfo_unistr_ngram, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 2)
	ZEND_ARG_INFO(0, str)
	ZEND_ARG_INFO(0, n)
ZEND_END_ARG_INFO()

ARG_INFO_STATIC
ZEND_BEGIN_ARG_INFO_EX(arginfo_unistr_decode_entity, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 1)
	ZEND_ARG_INFO(0, str)
	ZEND_ARG_INFO(0, flags)
ZEND_END_ARG_INFO()

ARG_INFO_STATIC
ZEND_BEGIN_ARG_INFO_EX(arginfo_unistr_rotate, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 2)
	ZEND_ARG_INFO(0, str)
	ZEND_ARG_INFO(0, delta)
	ZEND_ARG_INFO(0, fallback)
ZEND_END_ARG_INFO()

#ifdef WITH_UNICHAR_ICU
ARG_INFO_STATIC
ZEND_BEGIN_ARG_INFO_EX(arginfo_unorm, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 1)
	ZEND_ARG_INFO(0, str)
	ZEND_ARG_INFO(0, encoding)
ZEND_END_ARG_INFO()
#endif

#else
#define arginfo_unichr NULL
#define arginfo_unichr_array NULL
#define arginfo_uniord NULL
#define arginfo_uniord_array NULL
#define arginfo_unistr_validate second_arg_force_ref
#define arginfo_unistr_ngram NULL
#define arginfo_unistr_decode_entity NULL
#define arginfo_unistr_rotate NULL
#define arginfo_unorm NULL
#endif
/* }}} */

/* {{{ unichar_functions[] */
static zend_function_entry unichar_functions[] = {
	/* Unicode character utility functions */
	PHP_FE(unichr,               arginfo_unichr)
	PHP_FE(unichr_array,         arginfo_unichr_array)
	PHP_FE(uniord,               arginfo_uniord)
	PHP_FE(uniord_array,         arginfo_uniord_array)
	PHP_FE(unistr_validate,      arginfo_unistr_validate)
	PHP_FE(unistr_ngram,         arginfo_unistr_ngram)
	PHP_FE(unistr_decode_entity, arginfo_unistr_decode_entity)
	PHP_FE(unistr_rotate,        arginfo_unistr_rotate)
#ifdef WITH_UNICHAR_ICU
	/* Unicode normalization functions */
	PHP_FE(unorm_nfc,  arginfo_unorm)
	PHP_FE(unorm_nfd,  arginfo_unorm)
	PHP_FE(unorm_nfkc, arginfo_unorm)
	PHP_FE(unorm_nfkd, arginfo_unorm)
#endif
	/* ICU version information function */
	PHP_FE(unichar_icu_version, NULL)
	{ NULL, NULL, NULL }
};
/* }}} */

/* {{{ unichar_module_entry
 */
zend_module_entry unichar_module_entry = {
	STANDARD_MODULE_HEADER,
	"unichar",
	unichar_functions,
	PHP_MINIT(unichar),
	PHP_MSHUTDOWN(unichar),
	NULL,
	NULL,
	PHP_MINFO(unichar),
	PHP_UNICHAR_MODULE_VERSION,
#if ZEND_EXTENSION_API_NO >= 220060519
	PHP_MODULE_GLOBALS(unichar),
	PHP_GINIT(unichar),
	PHP_GSHUTDOWN(unichar),
	NULL,
	STANDARD_MODULE_PROPERTIES_EX
#else
	STANDARD_MODULE_PROPERTIES
#endif
};
/* }}} */

#ifdef COMPILE_DL_UNICHAR
ZEND_GET_MODULE(unichar)
#endif

/* {{{ PHP_MINIT_FUNCTION */
static PHP_MINIT_FUNCTION(unichar)
{
#if ZEND_EXTENSION_API_NO < 220060519
	ZEND_INIT_MODULE_GLOBALS(unichar, unichar_init_globals, unichar_shutdown_globals)
#endif

	unichar_register_ent_constants(module_number TSRMLS_CC);

#ifdef WITH_UNICHAR_ICU
	unichar_register_icu_constants(module_number TSRMLS_CC);
#endif

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION */
static PHP_MSHUTDOWN_FUNCTION(unichar)
{
#if ZEND_EXTENSION_API_NO < 220060519 && !defined(ZTS)
	unichar_shutdown_globals(&unichar_globals TSRMLS_CC);
#endif

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_GINIT_FUNCTION */
static PHP_GINIT_FUNCTION(unichar)
{
#if defined(WITH_UNICHAR_ICU) && !defined(IS_UNICODE)
	UErrorCode err = U_ZERO_ERROR;
	unichar_globals->conv = ucnv_open("UTF-8", &err);
#endif

	unichar_init_entity_table(&unichar_globals->entity_ht, &unichar_globals->entity_buf);
}
/* }}} */

/* {{{ PHP_GSHUTDOWN_FUNCTION */
static PHP_GSHUTDOWN_FUNCTION(unichar)
{
#if defined(WITH_UNICHAR_ICU) && !defined(IS_UNICODE)
	ucnv_close(unichar_globals->conv);
#endif

	zend_hash_destroy(&unichar_globals->entity_ht);
	pefree(unichar_globals->entity_buf, 1);
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION */
static PHP_MINFO_FUNCTION(unichar)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "Unicode Character Utility support", "enabled");
	php_info_print_table_row(2, "Module Version", PHP_UNICHAR_MODULE_VERSION);
	php_info_print_table_end();

	php_info_print_table_start();
#ifdef WITH_UNICHAR_ICU
	php_info_print_table_row(2, "ICU support", "enabled");
	php_info_print_table_row(2, "Library Version", unichar_icu_library_version);
	php_info_print_table_row(2, "Unicode Version", unichar_icu_unicode_version);
#else
	php_info_print_table_row(2, "ICU support", "disabled");
#endif
	php_info_print_table_end();
}
/* }}} */

/* {{{ string unichr(int code[, bool get_info])
   */
static PHP_FUNCTION(unichr)
{
	/* variables from argument */
	long code = 0;
	zend_bool get_info = 0;

	/* internal variables */
	char buf[UNICHAR_UTF8_MAXBUFSIZE] = {'\0'};
	char *str, *ptr;
	utf8size_t len = 0;

	/* parse the arguments */
	if (parse_parameters("l|b", &code, &get_info) == FAILURE) {
		return;
	}
#ifndef WITH_UNICHAR_ICU
	if (get_info) {
		raise_error(E_WARNING, "ICU is not enabled");
		RETURN_FALSE;
	}
#endif

	/* do conversion */
	ptr = &(buf[0]);
	str = unichar_unichr((unichar_t)code, &ptr, UNICHAR_UTF8_MAXBUFSIZE, &len);
	if (str == NULL) {
		RETURN_FALSE;
	}

	/* finish */
#ifdef WITH_UNICHAR_ICU
	if (get_info) {
		zval *info = unichar_make_info((UChar32)code, str, len TSRMLS_CC);
		if (info == NULL) {
			RETURN_FALSE;
		} else {
			RETURN_ZVAL(info, 1, 1);
		}
	}
#endif

#ifdef IS_UNICODE
	RETURN_UTF8_STRINGL(str, len, ZSTR_DUPLICATE);
#else
	RETURN_STRINGL(str, len, 1);
#endif
}
/* }}} unichr */

/* {{{ string unichr_array(array codes[, bool get_info])
   */
static PHP_FUNCTION(unichr_array)
{
	/* variables from argument */
	zval *zcodes = NULL;
	HashTable *codes = NULL;
	zend_bool get_info = 0;

	/* internal variables */
	zval **entry = NULL;
	int count = 0;
	char *str = NULL;
	utf8size_t len = 0;
	char *ptr = NULL;
	size_t rest = 0;

	/* parse the arguments */
	if (parse_parameters("a/|b", &zcodes, &get_info) == FAILURE) {
		return;
	}
#ifndef WITH_UNICHAR_ICU
	if (get_info) {
		raise_error(E_WARNING, "ICU is not enabled");
		RETURN_FALSE;
	}
#endif

	/* reset hash */
	codes = Z_ARRVAL_P(zcodes);
	zend_hash_internal_pointer_reset(codes);

	/* check if empty */
	count = zend_hash_num_elements(codes);
	if (count == 0) {
		RETURN_EMPTY_STRING();
	}

	/* allocate for result string */
	if (get_info) {
		array_init(return_value);
#ifdef IS_UNICODE
		Z_ARRVAL_P(return_value)->unicode = UG(unicode);
#endif
		rest = UNICHAR_UTF8_MAXBUFSIZE;
	} else {
		rest = UNICHAR_UTF8_MAXLEN * count + 1;
	}
	str = (char *)emalloc(rest);
	if (!str) {
		raise_error(E_ERROR, "Unable to allocate memory");
		RETURN_NULL();
	}
	ptr = str;

	/* do conversion for each element */
	while (zend_hash_get_current_data(codes, (void **)&entry) == SUCCESS) {
		if (Z_TYPE_PP(entry) != IS_LONG) {
			raise_error(E_WARNING, "Every elements of code points must be an integer");
			efree(str);
			if (get_info) {
				zval_dtor(return_value);
			}
			RETURN_FALSE;
		}

		if (unichar_unichr((unichar_t)Z_LVAL_PP(entry), &ptr, rest, &len) == NULL) {
			efree(str);
			if (get_info) {
				zval_dtor(return_value);
			}
			RETURN_FALSE;
		}

#ifdef WITH_UNICHAR_ICU
		if (get_info) {
			zval *info = unichar_make_info((UChar32)Z_LVAL_PP(entry), str, len TSRMLS_CC);
			if (info == NULL) {
				efree(str);
				zval_dtor(return_value);
				RETURN_FALSE;
			} else {
				add_next_index_zval(return_value, info);
			}
		} else
#endif
		{
			ptr += len;
			rest -= len;
		}
		zend_hash_move_forward(codes);
	}

	/* finish */
	if (!get_info) {
#ifdef IS_UNICODE
		RETVAL_UTF8_STRINGL(str, (size_t)(ptr - str), ZSTR_DUPLICATE);
#else
		RETVAL_STRINGL(str, (size_t)(ptr - str), 1);
#endif
	}
	efree(str);
}
/* }}} unichr_array */

/* {{{ int uniord(string str[, bool get_info])
   */
static PHP_FUNCTION(uniord)
{
	/* variables from argument */
	char *str = NULL;
	int str_len = 0;
	zend_bool get_info = 0;

	/* internal variables */
	unichar_t code = 0;

	/* parse the arguments */
#ifdef IS_UNICODE
	char buf[UNICHAR_UTF8_MAXBUFSIZE] = {'\0'};
	zstr zs;
	zend_uchar str_type;

	if (parse_parameters("t|b", &zs, &str_len, &str_type, &get_info) == FAILURE) {
		return;
	}
#else
	if (parse_parameters("s|b", &str, &str_len, &get_info) == FAILURE) {
		return;
	}
#endif

#ifndef WITH_UNICHAR_ICU
	if (get_info) {
		raise_error(E_WARNING, "ICU is not enabled");
		RETURN_FALSE;
	}
#endif
	if (str_len == 0) {
		raise_error(E_NOTICE, "Empty string is given");
		RETURN_FALSE;
	}

	/* do conversion */
#ifdef IS_UNICODE
	if (str_type == IS_UNICODE) {
		code = (unichar_t)zend_get_codepoint_at(zs.u, str_len, 0);
		if (get_info) {
			str = &(buf[0]);
			(void)unichar_unichr_silent(code, &str, UNICHAR_UTF8_MAXBUFSIZE, NULL);
		}
	} else {
		str = zs.s;
#endif
		code = unichar_uniord(str, (size_t)str_len, NULL);
		if (code == UNICHAR_UTF8_FAILURE) {
			RETURN_FALSE;
		}
#ifdef IS_UNICODE
	}
#endif

	/* finish */
#ifdef WITH_UNICHAR_ICU
	if (get_info) {
		zval *info = NULL;
		size_t len = 0;

		if (code < 0x80U) {
			len = 1;
		} else if (code < 0x800U) {
			len = 2;
		} else if (code < 0x10000U) {
			len = 3;
		} else {
			len = 4;
		}

		info = unichar_make_info((UChar32)code, str, len TSRMLS_CC);
		if (info == NULL) {
			RETURN_FALSE;
		} else {
			RETURN_ZVAL(info, 1, 1);
		}
	} else
#endif
	{
		RETURN_LONG((long)code);
	}
}
/* }}} uniord */

/* {{{ array uniord_array(string str[, bool get_info])
   */
static PHP_FUNCTION(uniord_array)
{
	/* variables from argument */
	char *str = NULL;
	int str_len = 0;
	zend_bool get_info = 0;

	/* internal variables */
	const char *ptr = NULL;
	size_t rest = 0;
	utf8size_t len = 0;

	/* parse the arguments */
#ifdef IS_UNICODE
	zstr zs;
	zend_uchar str_type;

	if (parse_parameters("t|b", &zs, &str_len, &str_type, &get_info) == FAILURE) {
		return;
	}
#else
	if (parse_parameters("s|b", &str, &str_len, &get_info) == FAILURE) {
		return;
	}
#endif

#ifndef WITH_UNICHAR_ICU
	if (get_info) {
		raise_error(E_WARNING, "ICU is not enabled");
		RETURN_FALSE;
	}
#endif
	if (str_len == 0) {
		raise_error(E_NOTICE, "Empty string is given");
		RETURN_FALSE;
	}

	/* setup variables */
	array_init(return_value);
#ifdef IS_UNICODE
	Z_ARRVAL_P(return_value)->unicode = UG(unicode);
#endif
	rest = (size_t)str_len;

	/* do conversion for each character */
#ifdef IS_UNICODE
	if (str_type == IS_UNICODE) {
		int offset = 0;

		while (rest > 0) {
			UChar32 code = zend_get_codepoint_at(zs.u, str_len, offset);

			if (get_info) {
				char buf[UNICHAR_UTF8_MAXBUFSIZE] = {'\0'};
				zval *info;

				str = &(buf[0]);
				(void)unichar_unichr_silent(code, &str, UNICHAR_UTF8_MAXBUFSIZE, &len);
				info = unichar_make_info(code, str, len TSRMLS_CC);
				if (info == NULL) {
					zval_dtor(return_value);
					RETURN_FALSE;
				} else {
					add_next_index_zval(return_value, info);
				}
			} else {
				add_next_index_long(return_value, (long)code);
			}

			if (U_IS_BMP(code) || code > UCHAR_MAX_VALUE) {
				offset += 1;
				rest -= 1;
			} else {
				offset += 2;
				rest -= 2;
			}
		}
	} else {
		str = zs.s;
#endif
		ptr = str;

		while (rest > 0) {
			unichar_t code = unichar_uniord(ptr, rest, &len);
			if (code == UNICHAR_UTF8_FAILURE) {
				zval_dtor(return_value);
				RETURN_FALSE;
			}

#ifdef WITH_UNICHAR_ICU
			if (get_info) {
				zval *info = unichar_make_info((UChar32)code, ptr, len TSRMLS_CC);
				if (info == NULL) {
					zval_dtor(return_value);
					RETURN_FALSE;
				} else {
					add_next_index_zval(return_value, info);
				}
			} else
#endif
			{
				add_next_index_long(return_value, (long)code);
			}

			ptr += len;
			rest -= len;
		}
#ifdef IS_UNICODE
	}
#endif
}
/* }}} uniord_array */

/* {{{ bool unistr_validate(string str[, int &length])
   */
static PHP_FUNCTION(unistr_validate)
{
	/* variables from argument */
	const char *str = NULL;
	int str_len = 0;
	zval *zlen = NULL;

	/* internal variables */
	const char *ptr = NULL;
	size_t rest = 0;
	utf8size_t len = 0;
	long pos = 0;

	/* parse the arguments */
#ifdef IS_UNICODE
	if (parse_parameters("s&|z", &str, &str_len, UG(utf8_conv), &zlen) == FAILURE) {
		return;
	}
#else
	if (parse_parameters("s|z", &str, &str_len, &zlen) == FAILURE) {
		return;
	}
#endif

	/* setup internal variables */
	rest = (size_t)str_len;
	ptr = str;

	/* do conversion for each character */
	while (rest > 0) {
		if (unichar_uniord_silent(ptr, rest, &len) == UNICHAR_UTF8_FAILURE) {
			RETURN_FALSE;
		}
		ptr += len;
		rest -= len;
		pos++;
	}

	if (zlen) {
		zval_dtor(zlen);
		ZVAL_LONG(zlen, pos);
	}

	RETURN_TRUE;
}
/* }}} unistr_validate */

/* {{{ array unistr_ngram(string str, int n)
   */
static PHP_FUNCTION(unistr_ngram)
{
	/* variables from argument */
	char *str = NULL;
	int str_len = 0;
#ifdef IS_UNICODE
	zend_uchar str_type;
#endif
	long ln = 0;
	int n = 0;

	/* internal variables */
	char *ptr = NULL;
	size_t rest = 0;
	utf8size_t len = 0;
	utf8size_t *sizes, *psizes, *eosizes;
	HashTable *ngram = NULL;

	/* parse the arguments */
#ifdef IS_UNICODE
	if (parse_parameters("s&^l", &str, &str_len, UG(utf8_conv), &str_type, &ln) == FAILURE) {
		return;
	}
#else
	if (parse_parameters("sl", &str, &str_len, &ln) == FAILURE) {
		return;
	}
#endif

	if (ln < 1L) {
		raise_error(E_WARNING, "Parameter 2 should be a positive number");
		RETURN_FALSE;
	}

	/* setup internal variables */
	n = (int)ln;
	rest = (size_t)str_len;
	sizes = (utf8size_t *)emalloc(sizeof(utf8size_t) * (rest + 1));
	if (sizes == NULL) {
		RETURN_FALSE;
	}
	psizes = sizes;

	/* get size for each character */
	ptr = str;
	while (rest > 0) {
		if (unichar_uniord(ptr, rest, &len) == UNICHAR_UTF8_FAILURE) {
			efree(sizes);
			RETURN_FALSE;
		}
		ptr += len;
		rest -= len;
		*psizes++ = (utf8size_t)len;
	}
	*psizes = 0;

	/* check for array size */
	if (ln >= (long)(psizes - sizes)) {
		efree(sizes);
		array_init(return_value);
#ifdef IS_UNICODE
		if (str_type == IS_UNICODE) {
			UChar *u;
			int u_len;
			(void)zend_string_to_unicode(UG(utf8_conv), &u, &u_len, str, str_len TSRMLS_CC);
			add_next_index_unicodel(return_value, u, u_len, 0);
			return;
		}
#endif
		add_next_index_stringl(return_value, str, str_len ,1);
		return;
	}

	/* setup return value */
	eosizes = psizes - (n - 1);
	psizes = sizes;
	ALLOC_HASHTABLE(ngram);
#ifdef IS_UNICODE
	zend_u_hash_init(ngram, (uint)(eosizes - psizes), NULL, ZVAL_PTR_DTOR, 0,
			(str_type == IS_UNICODE) ? 1 : 0);
#else
	zend_hash_init(ngram, (uint)(eosizes - psizes), NULL, ZVAL_PTR_DTOR, 0);
#endif
	Z_TYPE_P(return_value) = IS_ARRAY;
	Z_ARRVAL_P(return_value) = ngram;

	/* copy */
	ptr = str;
#ifdef IS_UNICODE
	if (str_type == IS_UNICODE) {
		while (psizes < eosizes) {
			int i = 0, l = 0, u_len = 0;
			UChar *u;
			while (i < n) {
				l += *(psizes + i);
				i++;
			}
			(void)zend_string_to_unicode(UG(utf8_conv), &u, &u_len, ptr, l TSRMLS_CC);
			add_next_index_unicodel(return_value, u, u_len, 0);
			ptr += *psizes++;
		}
	} else
#endif
	{
		while (psizes < eosizes) {
			int i = 0, l = 0;
			while (i < n) {
				l += *(psizes + i);
				i++;
			}
			add_next_index_stringl(return_value, ptr, l, 1);
			ptr += *psizes++;
		}
	}

	efree(sizes);
}
/* }}} unistr_ngram */

/* {{{ string unistr_decode_entity(string str[, int flags])
   */
static PHP_FUNCTION(unistr_decode_entity)
{
	/* variables from argument */
	const char *str = NULL;
	int str_len = 0;
	long flags = UNICHAR_ENT_IGNORE_NUL;

	/* internal variables */
	smart_str result = {0};
	const char *ptr, *eos;
	char ent_buf[UNICHAR_ENTITY_MAXBUFSIZE];
	char dec_buf[UNICHAR_UTF8_MAXBUFSIZE];

	/* parse the arguments */
	if (parse_parameters("s|l", &str, &str_len, &flags) == FAILURE) {
		return;
	}

	ptr = str;
	eos = str + str_len;

	while (ptr < eos) {
		const char *bos = ptr;

		while (ptr < eos && *ptr != '&') {
			ptr++;
		}

		if (ptr > bos) {
			smart_str_appendl(&result, bos, (ptr - bos));
			bos = ptr;
		}

		if (ptr < eos) {
			char *ent_ptr = &ent_buf[0];
			utf8size_t ent_len = 0;
			char *dec_ptr = &dec_buf[0];
			utf8size_t dec_len = 0;

			/* skip for '&' */
			ptr++;

			if (*ptr == '#') {
				/* numeric reference */
				zend_bool is_hex = 0;
				long code = 0;
				char *dec = NULL;

				/* skip for '#' */
				ptr++;

				/* read the number */
				if (ptr < eos && (*ptr == 'X' || *ptr == 'x')) {
					is_hex = 1;
					/* skip for 'x' */
					ptr++;
					/* skip for leading zeros */
					while (ptr < eos && *ptr == '0') {
						ptr++;
					}
					/* check for hexadecimals */
					while (ptr < eos && ent_len < UNICHAR_ENTITY_MAXBUFSIZE &&
							((*ptr >= '0' && *ptr <= '9') || (*ptr >= 'A' && *ptr <= 'F') || (*ptr >= 'a' && *ptr <= 'f')))
					{
						ent_ptr[ent_len++] = *ptr++;
					}
				} else {
					/* skip for leading zeros */
					while (ptr < eos && *ptr == '0') {
						ptr++;
					}
					/* check for decimals */
					while (ptr < eos && ent_len < UNICHAR_ENTITY_MAXBUFSIZE  && *ptr >= '0' && *ptr <= '9') {
						ent_ptr[ent_len++] = *ptr++;
					}
				}

				/* check for NUL(0x00) */
				if (ent_len == 0) {
					if (is_hex) {
						if (ptr - bos > 2) {
							ent_ptr[ent_len++] = '0';
						}
					} else {
						if (ptr - bos > 1) {
							ent_ptr[ent_len++] = '0';
						}
					}
				}

				/* check for buffer length */
				if (ent_len == 0 || ent_len == UNICHAR_ENTITY_MAXBUFSIZE) {
					if (flags & UNICHAR_ENT_STRICT) {
						raise_error(E_WARNING, "Wrong entity found near offset %ld", (long)(bos - str));
						smart_str_free(&result);
						RETURN_FALSE;
					} else {
						if (ptr > bos) {
							smart_str_appendl(&result, bos, (ptr - bos));
						}
						continue;
					}
				}

				/* check for terminating ';' */
				if (*ptr == ';') {
					ptr++;
				} else if (flags & UNICHAR_ENT_STRICT) {
					raise_error(E_WARNING, "Wrong entity found near offset %ld", (long)(bos - str));
					smart_str_free(&result);
					RETURN_FALSE;
				}
				ent_ptr[ent_len] = '\0';

				/* convert to long */
				if (is_hex) {
					code = strtol(ent_ptr, NULL, 16);
				} else {
					code = strtol(ent_ptr, NULL, 10);
				}

				/* check for control character */
				if (code < 32L || (code >= 127L && code < 160L)) {
					if (flags & UNICHAR_ENT_CTRL_TO_SPACE) {
						smart_str_appendc(&result, ' ');
						continue;
					} else if (code == 0 && flags & UNICHAR_ENT_IGNORE_NUL) {
						if (flags & UNICHAR_ENT_WARN_NUL) {
							raise_error(E_WARNING, "Wrong entity found near offset %ld", (long)(bos - str));
							smart_str_free(&result);
							RETURN_FALSE;
						} else {
							if (!(flags & UNICHAR_ENT_REMOVE_NUL)) {
								smart_str_appendl(&result, bos, (ptr - bos));
							}
							continue;
						}
					} else if (flags & UNICHAR_ENT_IGNORE_CTRL) {
						if (flags & UNICHAR_ENT_WARN_CTRL) {
							raise_error(E_WARNING, "Wrong entity found near offset %ld", (long)(bos - str));
							smart_str_free(&result);
							RETURN_FALSE;
						} else {
							if (!(flags & UNICHAR_ENT_REMOVE_CTRL)) {
								smart_str_appendl(&result, bos, (ptr - bos));
							}
							continue;
						}
					}
				}

				/* check for NBSP(0xA0) */
				if (code == 0xA0 && flags & UNICHAR_ENT_NBSP_TO_SPACE) {
					smart_str_appendc(&result, ' ');
					continue;
				}

				/* convert to string */
				dec = unichar_unichr_silent((unichar_t)code, &dec_ptr, UNICHAR_UTF8_MAXBUFSIZE, &dec_len);
				if (dec == NULL) {
					if (flags & UNICHAR_ENT_STRICT) {
						raise_error(E_WARNING, "Wrong entity found near offset %ld", (long)(bos - str));
						smart_str_free(&result);
						RETURN_FALSE;
					} else {
						smart_str_appendl(&result, bos, (ptr - bos));
					}
				} else {
					smart_str_appendl(&result, dec, dec_len);
				}
			} else {
				/* character entity */
				char **entity = NULL;

				/* read the entity */
				while (ptr < eos && ent_len < UNICHAR_ENTITY_MAXBUFSIZE &&
						((*ptr >= '0' && *ptr <= '9') || (*ptr >= 'A' && *ptr <= 'Z') || (*ptr >= 'a' && *ptr <= 'z')))
				{
					ent_ptr[ent_len++] = *ptr++;
				}

				/* check for buffer length */
				if (ent_len == 0 || ent_len == UNICHAR_ENTITY_MAXBUFSIZE) {
					if (flags & UNICHAR_ENT_STRICT) {
						raise_error(E_WARNING, "Wrong entity found near offset %ld", (long)(bos - str));
						smart_str_free(&result);
						RETURN_FALSE;
					} else {
						if (ptr > bos) {
							smart_str_appendl(&result, bos, (ptr - bos));
						}
						continue;
					}
				}

				/* check for terminating ';' */
				if (*ptr == ';') {
					ptr++;
				} else if (flags & UNICHAR_ENT_STRICT) {
					raise_error(E_WARNING, "Wrong entity found near offset %ld", (long)(bos - str));
					smart_str_free(&result);
					RETURN_FALSE;
				}
				ent_ptr[ent_len] = '\0';

				/* check for NBSP(0xA0) */
				if (flags & UNICHAR_ENT_NBSP_TO_SPACE && !strcmp(ent_ptr, "nbsp")) {
					smart_str_appendc(&result, ' ');
					continue;
				}

				/* find the entity */
				if (zend_hash_find(&UNICHAR_G(entity_ht), ent_ptr, (uint)ent_len + 1, (void **)&entity) == SUCCESS) {
					smart_str_appends(&result, *entity);
				} else {
					if (flags & UNICHAR_ENT_STRICT) {
						raise_error(E_WARNING, "Wrong entity found near offset %ld", (long)(bos - str));
						smart_str_free(&result);
						RETURN_FALSE;
					} else {
						smart_str_appendl(&result, bos, (ptr - bos));
					}
				}
			}
		}
	}

	/* Null terminate */
	smart_str_0(&result);

#ifdef IS_UNICODE
	RETURN_UTF8_STRINGL(result.c, result.len, ZSTR_AUTOFREE);
#else
	RETURN_STRINGL(result.c, result.len, 0);
#endif
}
/* }}} unistr_decode_entity */

/* {{{ bool unistr_rotate(string str, int delta[, mixed fallback])
   */
static PHP_FUNCTION(unistr_rotate)
{
	/* variables from argument */
	const char *str = NULL;
	int str_len = 0;
	long delta = 0;
	zval *zfallback = NULL;

	/* internal variables */
	smart_str result = {0};
	const char *ptr, *fallback;
	char work_buf[UNICHAR_UTF8_MAXBUFSIZE];
	char fallback_buf[UNICHAR_UTF8_MAXBUFSIZE];
	char *work, *work_ptr, *fallback_ptr;
	unichar_t code, fallback_code = '*';
	size_t rest;
	utf8size_t len, work_len, fallback_len;

	/* parse the arguments */
	if (parse_parameters("sl|z", &str, &str_len, &delta, &zfallback) == FAILURE) {
		return;
	}

	/* check the fallback character */
	if (zfallback == NULL || Z_TYPE_P(zfallback) == IS_LONG) {
		if (zfallback != NULL) {
			fallback_code = (unichar_t)Z_LVAL_P(zfallback);
		}
		fallback_ptr = &fallback_buf[0];
		fallback = unichar_unichr(fallback_code, &fallback_ptr, UNICHAR_UTF8_MAXBUFSIZE, &fallback_len);
		if (fallback == NULL) {
			RETURN_FALSE;
		}
	} else if (Z_TYPE_P(zfallback) == IS_NULL || Z_TYPE_P(zfallback) == IS_STRING) {
		if (Z_TYPE_P(zfallback) == IS_NULL || Z_STRLEN_P(zfallback) == 0) {
			fallback_len = 0;
			fallback_buf[0] = '\0';
			fallback = &fallback_buf[0];
		} else {
			fallback_ptr = Z_STRVAL_P(zfallback);
			if (unichar_uniord(fallback_ptr, (size_t)Z_STRLEN_P(zfallback), &fallback_len) == UNICHAR_UTF8_FAILURE) {
				RETURN_FALSE;
			}
			fallback = fallback_ptr;
		}
	} else {
		raise_error(E_WARNING, "Parameter 3 should be a valid UTF-8 string or Unicode codepoint");
		RETURN_FALSE;
	}

	/* setup internal variables */
	rest = (size_t)str_len;
	ptr = str;

	/* do conversion for each character */
	while (rest > 0) {
		code = unichar_uniord(ptr, rest, &len);
		if (code == UNICHAR_UTF8_FAILURE) {
			RETURN_FALSE;
		}
		ptr += len;
		rest -= len;

		work_ptr = &work_buf[0];
		work = unichar_unichr_silent(code + (unichar_t)delta, &work_ptr, UNICHAR_UTF8_MAXBUFSIZE, &work_len);
		if (work == NULL) {
			if (fallback_len > 0) {
				smart_str_appendl(&result, fallback, fallback_len);
			}
		} else {
			smart_str_appendl(&result, work, work_len);
		}
	}

	/* Null terminate */
	smart_str_0(&result);

#ifdef IS_UNICODE
	RETURN_UTF8_STRINGL(result.c, result.len, ZSTR_AUTOFREE);
#else
	RETURN_STRINGL(result.c, result.len, 0);
#endif
}
/* }}} unistr_rotate */

#ifdef WITH_UNICHAR_ICU
/* {{{ string unorm_nfc(string str[, string encoding])
   */
static PHP_FUNCTION(unorm_nfc)
{
	unichar_unorm(INTERNAL_FUNCTION_PARAM_PASSTHRU, UNORM_NFC);
}
/* }}} unorm_nfc */

/* {{{ string unorm_nfd(string str[, string encoding])
   */
static PHP_FUNCTION(unorm_nfd)
{
	unichar_unorm(INTERNAL_FUNCTION_PARAM_PASSTHRU, UNORM_NFD);
}
/* }}} unorm_nfd */

/* {{{ string unorm_nfkc(string str[, string encoding])
   */
static PHP_FUNCTION(unorm_nfkc)
{
	unichar_unorm(INTERNAL_FUNCTION_PARAM_PASSTHRU, UNORM_NFKC);
}
/* }}} unorm_nfkc */

/* {{{ string unorm_nfkd(string str[, string encoding])
   */
static PHP_FUNCTION(unorm_nfkd)
{
	unichar_unorm(INTERNAL_FUNCTION_PARAM_PASSTHRU, UNORM_NFKD);
}
/* }}} unorm_nfkd */
#endif /* WITH_UNICHAR_ICU */

/* {{{ string unichar_icu_version()
   */
static PHP_FUNCTION(unichar_icu_version)
{
#ifdef WITH_UNICHAR_ICU
	char *version = unichar_icu_library_version;
	size_t len = strlen(version);
#endif

	if (ZEND_NUM_ARGS() != 0) {
		WRONG_PARAM_COUNT;
	}

#ifdef WITH_UNICHAR_ICU
#ifdef IS_UNICODE
	RETURN_ASCII_STRINGL(version, len, ZSTR_DUPLICATE);
#else
	RETURN_STRINGL(version, len, 1);
#endif
#else
	RETURN_FALSE;
#endif
}
/* }}} unichar_icu_version */

/* {{{ unichar_unichr_ex()
   */
static char *
unichar_unichr_ex(unichar_t code, char **buf, size_t size, utf8size_t *len,
		zend_bool report_errors TSRMLS_DC)
{
	char *str = NULL;
	char *ptr = NULL;

	/* check the code point */
	if (/* Out-of-Unicode */
		code > 0x10FFFFU ||
		/* Surrogate */
		(code > 0xD7FFU && code < 0xE000U) ||
		/* Noncharacter */
		(code > 0xFDCFU && code < 0xFDF0U) || (code & 0xFFFEU) == 0xFFFEU
	) {
		if (report_errors) {
			raise_error(E_WARNING, "Invalid code point (0x%lX) given", (unsigned long)code);
		}
		goto error;
	}

	/* set the pointer */
	if (buf == NULL) {
		str = (char *)emalloc(UNICHAR_UTF8_MAXBUFSIZE);
		if (str == NULL) {
			goto error;
		}
	} else {
		if (size < UNICHAR_UTF8_MAXBUFSIZE) {
			if (report_errors) {
				raise_error(E_ERROR, "Buffer size is not enough");
			}
			goto error;
		}
		str = *buf;
	}
	ptr = str;

	/* do conversion */
	if (code < 0x80U) {
		/* UTF8-1, US-ASCII */
		*ptr++ = (char)code;
	} else if (code < 0x800U) {
		/* UTF8-2 */
		*ptr++ = 0xC0 | (char)(code >> 6);
		*ptr++ = 0x80 | (char)(code & 0x3FU);
	} else if (code < 0x10000U) {
		/* UTF8-3 */
		*ptr++ = 0xE0 | (char)(code >> 12);
		*ptr++ = 0x80 | (char)(code >> 6 & 0x3FU);
		*ptr++ = 0x80 | (char)(code & 0x3FU);
	} else {
		/* UTF8-4, surrogate pairs in UTF-16 */
		*ptr++ = 0xF0 | (char)(code >> 18);
		*ptr++ = 0x80 | (char)(code >> 12 & 0x3FU);
		*ptr++ = 0x80 | (char)(code >> 6 & 0x3FU);
		*ptr++ = 0x80 | (char)(code & 0x3FU);
	}
	*ptr = '\0';

	if (len != NULL) {
		*len = (utf8size_t)(ptr - str);
	}

	return str;

  error:
	if (len != NULL) {
		*len = -1;
	}
	return NULL;
}
/* }}} unichar_unichr_ex */

/* {{{ unichar_uniord_ex()
   */
static unichar_t
unichar_uniord_ex(const char *str, size_t size, utf8size_t *len,
		zend_bool report_errors TSRMLS_DC)
{
	const unsigned char *c = (const unsigned char *)str;
	unichar_t code = 0;
	utf8size_t l = 0;

	if (size == 0) {
		goto error;
	}

	/* check the sequence */
	if (*c > 0xEFU && *c < 0xF5U && size > 3) {
		/* UTF8-4, surrogate pairs */
		if (*(c+2) < 0x80U || *(c+2) > 0xBFU || *(c+3) < 0x80U || *(c+3) > 0xBFU) {
			goto error;
		}
		if (*c == 0xF0U) {
			if (*(c+1) < 0x90U || *(c+1) > 0xBFU) {
				goto error;
			}
		} else if (*c == 0xF4U) {
			if (*(c+1) < 0x80U || *(c+1) > 0x8FU) {
				goto error;
			}
		} else {
			if (*(c+1) < 0x80U || *(c+1) > 0xBFU) {
				goto error;
			}
		}
		code = ((unichar_t)(*c & 0x7U) << 18)
				| ((unichar_t)(*(c+1) & 0x3FU) << 12)
				| ((unichar_t)(*(c+2) & 0x3FU) << 6)
				| (unichar_t)(*(c+3) & 0x3FU);
		l = 4;
	} else if (*c > 0xDFU && *c < 0xF0U && size > 2) {
		/* UTF8-3 */
		if (*(c+2) < 0x80U || *(c+2) > 0xBFU) {
			goto error;
		}
		if (*c == 0xE0U) {
			if (*(c+1) < 0xA0U || *(c+1) > 0xBFU) {
				goto error;
			}
		} else if (*c == 0xEDU) {
			if (*(c+1) < 0x80U || *(c+1) > 0x9FU) {
				goto error;
			}
		} else {
			if (*(c+1) < 0x80U || *(c+1) > 0xBFU) {
				goto error;
			}
		}
		code = ((unichar_t)(*c & 0xFU) << 12)
				| ((unichar_t)(*(c+1) & 0x3FU) << 6)
				| (unichar_t)(*(c+2) & 0x3FU);
		l = 3;
	} else if (*c > 0xC1U && *c < 0xE0U && size > 1) {
		/* UTF8-2 */
		if (*(c+1) < 0x80U || *(c+1) > 0xBFU) {
			goto error;
		}
		code = ((unichar_t)(*c & 0x1FU) << 6) | (unichar_t)(*(c+1) & 0x3FU);
		l = 2;
	} else if (*c < 0x80U) {
		/* UTF8-1, US-ASCII */
		code = (unichar_t)*c;
		l = 1;
	} else {
		goto error;
	}

	/* set the length */
	if (len != NULL) {
		*len = l;
	}

	return code;

  error:
	if (report_errors) {
		raise_error(E_WARNING, "Not a valid UTF-8 sequence");
	}
	if (len != NULL) {
		*len = -1;
	}
	return UNICHAR_UTF8_FAILURE;
}
/* }}} unichar_uniord_ex */

/* {{{ unichar_init_entity_table()
    */
static void
unichar_init_entity_table(HashTable *ht, char **buf)
{
	unichar_entity_map_t entities[] = {
		UE("apos",      39),
		UE("quot",      34), UE("amp",       38), UE("lt",        60), UE("gt",        62),
		UE("nbsp",     160), UE("iexcl",    161), UE("cent",     162), UE("pound",    163),
		UE("curren",   164), UE("yen",      165), UE("brvbar",   166), UE("sect",     167),
		UE("uml",      168), UE("copy",     169), UE("ordf",     170), UE("laquo",    171),
		UE("not",      172), UE("shy",      173), UE("reg",      174), UE("macr",     175),
		UE("deg",      176), UE("plusmn",   177), UE("sup2",     178), UE("sup3",     179),
		UE("acute",    180), UE("micro",    181), UE("para",     182), UE("middot",   183),
		UE("cedil",    184), UE("sup1",     185), UE("ordm",     186), UE("raquo",    187),
		UE("frac14",   188), UE("frac12",   189), UE("frac34",   190), UE("iquest",   191),
		UE("Agrave",   192), UE("Aacute",   193), UE("Acirc",    194), UE("Atilde",   195),
		UE("Auml",     196), UE("Aring",    197), UE("AElig",    198), UE("Ccedil",   199),
		UE("Egrave",   200), UE("Eacute",   201), UE("Ecirc",    202), UE("Euml",     203),
		UE("Igrave",   204), UE("Iacute",   205), UE("Icirc",    206), UE("Iuml",     207),
		UE("ETH",      208), UE("Ntilde",   209), UE("Ograve",   210), UE("Oacute",   211),
		UE("Ocirc",    212), UE("Otilde",   213), UE("Ouml",     214), UE("times",    215),
		UE("Oslash",   216), UE("Ugrave",   217), UE("Uacute",   218), UE("Ucirc",    219),
		UE("Uuml",     220), UE("Yacute",   221), UE("THORN",    222), UE("szlig",    223),
		UE("agrave",   224), UE("aacute",   225), UE("acirc",    226), UE("atilde",   227),
		UE("auml",     228), UE("aring",    229), UE("aelig",    230), UE("ccedil",   231),
		UE("egrave",   232), UE("eacute",   233), UE("ecirc",    234), UE("euml",     235),
		UE("igrave",   236), UE("iacute",   237), UE("icirc",    238), UE("iuml",     239),
		UE("eth",      240), UE("ntilde",   241), UE("ograve",   242), UE("oacute",   243),
		UE("ocirc",    244), UE("otilde",   245), UE("ouml",     246), UE("divide",   247),
		UE("oslash",   248), UE("ugrave",   249), UE("uacute",   250), UE("ucirc",    251),
		UE("uuml",     252), UE("yacute",   253), UE("thorn",    254), UE("yuml",     255),
		UE("OElig",    338), UE("oelig",    339), UE("Scaron",   352), UE("scaron",   353),
		UE("Yuml",     376), UE("circ",     710), UE("tilde",    732), UE("fnof",     402),
		UE("Alpha",    913), UE("Beta",     914), UE("Gamma",    915), UE("Delta",    916),
		UE("Epsilon",  917), UE("Zeta",     918), UE("Eta",      919), UE("Theta",    920),
		UE("Iota",     921), UE("Kappa",    922), UE("Lambda",   923), UE("Mu",       924),
		UE("Nu",       925), UE("Xi",       926), UE("Omicron",  927), UE("Pi",       928),
		UE("Rho",      929), UE("Sigma",    931), UE("Tau",      932), UE("Upsilon",  933),
		UE("Phi",      934), UE("Chi",      935), UE("Psi",      936), UE("Omega",    937),
		UE("alpha",    945), UE("beta",     946), UE("gamma",    947), UE("delta",    948),
		UE("epsilon",  949), UE("zeta",     950), UE("eta",      951), UE("theta",    952),
		UE("iota",     953), UE("kappa",    954), UE("lambda",   955), UE("mu",       956),
		UE("nu",       957), UE("xi",       958), UE("omicron",  959), UE("pi",       960),
		UE("rho",      961), UE("sigmaf",   962), UE("sigma",    963), UE("tau",      964),
		UE("upsilon",  965), UE("phi",      966), UE("chi",      967), UE("psi",      968),
		UE("omega",    969), UE("thetasym", 977), UE("upsih",    978), UE("piv",      982),
		UE("ensp",    8194), UE("emsp",    8195), UE("thinsp",  8201), UE("zwnj",    8204),
		UE("zwj",     8205), UE("lrm",     8206), UE("rlm",     8207), UE("ndash",   8211),
		UE("mdash",   8212), UE("lsquo",   8216), UE("rsquo",   8217), UE("sbquo",   8218),
		UE("ldquo",   8220), UE("rdquo",   8221), UE("bdquo",   8222), UE("dagger",  8224),
		UE("Dagger",  8225), UE("bull",    8226), UE("hellip",  8230), UE("permil",  8240),
		UE("prime",   8242), UE("Prime",   8243), UE("lsaquo",  8249), UE("rsaquo",  8250),
		UE("oline",   8254), UE("frasl",   8260), UE("euro",    8364), UE("image",   8465),
		UE("weierp",  8472), UE("real",    8476), UE("trade",   8482), UE("alefsym", 8501),
		UE("larr",    8592), UE("uarr",    8593), UE("rarr",    8594), UE("darr",    8595),
		UE("harr",    8596), UE("crarr",   8629), UE("lArr",    8656), UE("uArr",    8657),
		UE("rArr",    8658), UE("dArr",    8659), UE("hArr",    8660), UE("forall",  8704),
		UE("part",    8706), UE("exist",   8707), UE("empty",   8709), UE("nabla",   8711),
		UE("isin",    8712), UE("notin",   8713), UE("ni",      8715), UE("prod",    8719),
		UE("sum",     8721), UE("minus",   8722), UE("lowast",  8727), UE("radic",   8730),
		UE("prop",    8733), UE("infin",   8734), UE("ang",     8736), UE("and",     8743),
		UE("or",      8744), UE("cap",     8745), UE("cup",     8746), UE("int",     8747),
		UE("there4",  8756), UE("sim",     8764), UE("cong",    8773), UE("asymp",   8776),
		UE("ne",      8800), UE("equiv",   8801), UE("le",      8804), UE("ge",      8805),
		UE("sub",     8834), UE("sup",     8835), UE("nsub",    8836), UE("sube",    8838),
		UE("supe",    8839), UE("oplus",   8853), UE("otimes",  8855), UE("perp",    8869),
		UE("sdot",    8901), UE("lceil",   8968), UE("rceil",   8969), UE("lfloor",  8970),
		UE("rfloor",  8971), UE("lang",    9001), UE("rang",    9002), UE("loz",     9674),
		UE("spades",  9824), UE("clubs",   9827), UE("hearts",  9829), UE("diams",   9830),
		{ NULL, 0, 0 }
	};

	char *ptr;
	size_t pos = 0, esize = 4, nmemb = 256;

#ifdef safe_pemalloc
	*buf = (char *)safe_pemalloc(nmemb, esize, 0, 1);
#else
	*buf = (char *)pemalloc(esize * nmemb, 1);
#endif
	ptr = *buf;

	zend_hash_init(ht, nmemb, NULL, NULL, 1);

	while (entities[pos].name != NULL) {
		char *ent;
		uint16_t code;

		ent = ptr;
		code = entities[pos].code;

		if (code > 0x7FFU) {
			*ptr++ = 0xE0U | (char)(code >> 12);
			*ptr++ = 0x80U | (char)(code >> 6 & 0x3FU);
			*ptr++ = 0x80U | (char)(code & 0x3FU);
		} else if (code > 0x7FU) {
			*ptr++ = 0xC0U | (char)(code >> 6);
			*ptr++ = 0x80U | (char)(code & 0x3FU);
		} else {
			*ptr++ = (char)code;
		}
		*ptr++ = '\0';

		zend_hash_add(ht, entities[pos].name, entities[pos].size, (void *)&ent, sizeof(char *), NULL);

		pos++;
	}
}
/* }}} */

#ifdef WITH_UNICHAR_ICU
/* {{{ unichar_ucnv_open()
   */
static void
unichar_ucnv_open(unichar_ucnv_t *cnv, const char *name, UErrorCode *err TSRMLS_DC)
{
	if (name == NULL || ucnv_compareNames("UTF-8", name) == 0) {
#ifdef IS_UNICODE
		cnv->ptr = UG(utf8_conv);
#else
		cnv->ptr = UNICHAR_G(conv);
#endif
		cnv->is_default = 1;
	} else {
		cnv->ptr = ucnv_open(name, err);
		if (U_FAILURE(*err)) {
			cnv->ptr = NULL;
		}
		cnv->is_default = 0;
	}
}
/* }}} unichar_ucnv_open */

/* {{{ unichar_ucnv_close()
   */
static void
unichar_ucnv_close(unichar_ucnv_t *cnv)
{
	if (!cnv->is_default && cnv->ptr != NULL) {
		ucnv_close(cnv->ptr);
	}
}
/* }}} unichar_ucnv_close */

/* {{{ unichar_unorm()
   */
static void
unichar_unorm(INTERNAL_FUNCTION_PARAMETERS, UNormalizationMode mode)
{
	/* variables from argument */
	char *str = NULL;
	int str_len = 0;
#ifdef IS_UNICODE
	zstr txt;
	zend_uchar str_type;
#else
	zend_uchar str_type = IS_STRING;
#endif
	const char *encoding = NULL;
	int encoding_len = 0;

	/* internal variables */
	unichar_ucnv_t cnv;
	UErrorCode err = U_ZERO_ERROR;
	char *res = NULL;
	UChar *uni = NULL;
	UChar *nrm = NULL;
	int32_t res_len = 0, uni_len = 0, nrm_len = 0, buf_len = 0;

	/* parse the arguments */
#ifdef IS_UNICODE
	if (parse_parameters("t|s", &txt, &str_len, &str_type, &encoding, &encoding_len) == FAILURE) {
		return;
	}
#else
	if (parse_parameters("s|s", &str, &str_len, &encoding, &encoding_len) == FAILURE) {
		return;
	}
#endif

	/* empty */
	if (str_len == 0) {
		UC_RETURN_EMPTY_TEXT(str_type);
	}

#ifdef IS_UNICODE
	if (str_type == IS_UNICODE) {
		if (encoding != NULL) {
			raise_error(E_NOTICE, "Parameter 2 is ignored for Unicode string");
		}
		uni = txt.u;
		uni_len = str_len;
		cnv.is_default = 1;
		cnv.ptr = NULL;
	} else {
		str = txt.s;
#endif
		/* open the converter */
		unichar_ucnv_open(&cnv, encoding, &err TSRMLS_CC);
		if (U_FAILURE(err)) {
			raise_error(E_WARNING, "Failed to open Unicode converter (%s)", u_errorName(err));
			RETURN_FALSE;
		}

		/* convert from char to UChar */
		err = U_ZERO_ERROR;
		buf_len = str_len / ucnv_getMinCharSize(cnv.ptr);
		uni = od_umalloc((size_t)buf_len + 1);
		uni_len = ucnv_toUChars(cnv.ptr, uni, buf_len, str, (int32_t)str_len, &err);
		if (U_FAILURE(err)) {
			od_ufree(uni, str_type);
			unichar_ucnv_close(&cnv);
			raise_error(E_WARNING, "Failed to convert from char to UChar (%s)", u_errorName(err));
			RETURN_FALSE;
		}
#ifdef IS_UNICODE
	}
#endif

	/* check for normalize */
	err = U_ZERO_ERROR;
	if (unorm_quickCheck(uni, uni_len, mode, &err) == UNORM_YES) {
		od_ufree(uni, str_type);
		unichar_ucnv_close(&cnv);
		if (U_FAILURE(err)) {
			raise_error(E_WARNING, "Failed to check for normalize (%s)", u_errorName(err));
			RETURN_FALSE;
		}
		UC_RETURN_TEXTL(uni, str, str_len, str_type, 1);
	}

	/* check for result length */
	err = U_ZERO_ERROR;
	nrm_len = unorm_normalize(uni, uni_len, mode, 0, NULL, 0, &err);
	if (U_FAILURE(err) && err != U_BUFFER_OVERFLOW_ERROR) {
		od_ufree(uni, str_type);
		unichar_ucnv_close(&cnv);
		raise_error(E_WARNING, "Failed to check for normalize (%s)", u_errorName(err));
		RETURN_FALSE;
	}

	/* do normalize */
	err = U_ZERO_ERROR;
	nrm = od_umalloc((size_t)nrm_len + 1);
	(void)unorm_normalize(uni, uni_len, mode, 0, nrm, nrm_len, &err);
	od_ufree(uni, str_type);
	if (U_FAILURE(err)) {
		efree(nrm);
		unichar_ucnv_close(&cnv);
		raise_error(E_WARNING, "Failed to normalize (%s)", u_errorName(err));
		RETURN_FALSE;
	}

#ifdef IS_UNICODE
	/* return unicode */
	if (str_type == IS_UNICODE) {
		unichar_ucnv_close(&cnv);
		RETURN_UNICODEL(nrm, nrm_len, 0);
	}
#endif

	/* convert from UChar to char */
	err = U_ZERO_ERROR;
	buf_len = nrm_len * ucnv_getMaxCharSize(cnv.ptr);
	res = (char *)emalloc((size_t)buf_len + 1);
	res_len = ucnv_fromUChars(cnv.ptr, res, buf_len, nrm, nrm_len, &err);
	efree(nrm);
	if (U_FAILURE(err)) {
		efree(res);
		unichar_ucnv_close(&cnv);
		raise_error(E_WARNING, "Failed to convert from UChar to char (%s)", u_errorName(err));
		RETURN_FALSE;
	}
	res[res_len] = '\0';

	/* finish */
	unichar_ucnv_close(&cnv);
	if (res_len < buf_len) {
		res = (char *)erealloc(res, (size_t)res_len + 1);
	}
	RETURN_STRINGL(res, res_len, 0);
}
/* }}} unichar_unorm */

/* {{{ unichar_make_info()
   */
static zval *
unichar_make_info(UChar32 codepoint, const char *str, size_t len TSRMLS_DC)
{
	zval *zv;
	UScriptCode scriptCode;
	UErrorCode err = U_ZERO_ERROR;

	if (len == 0 || codepoint > 0x10FFFF) {
		return NULL;
	}

	MAKE_STD_ZVAL(zv);
	array_init(zv);

#ifdef IS_UNICODE
	Z_ARRVAL_P(zv)->unicode = UG(unicode);
#endif

	add_assoc_long(zv, "code", (long)codepoint);
#ifdef IS_UNICODE
	add_assoc_utf8_stringl(zv, "utf8", (char *)str, (uint)len, ZSTR_DUPLICATE);
#else
	add_assoc_stringl(zv, "utf8", (char *)str, (uint)len, 1);
#endif
	add_assoc_long(zv, "category", (long)u_charType(codepoint));
	add_assoc_long(zv, "block", (long)ublock_getCode(codepoint));

	scriptCode = uscript_getScript(codepoint, &err);
	if (U_FAILURE(err)) {
		add_assoc_long(zv, "script", -1L);
		add_assoc_null(zv, "script_name");
		add_assoc_null(zv, "script_short_name");
	} else {
		add_assoc_long(zv, "script", (long)scriptCode);
#ifdef IS_UNICODE
		add_assoc_ascii_string(zv, "script_name", (char *)uscript_getName(scriptCode), ZSTR_DUPLICATE);
		add_assoc_ascii_string(zv, "script_short_name", (char *)uscript_getShortName(scriptCode), ZSTR_DUPLICATE);
#else
		add_assoc_string(zv, "script_name", (char *)uscript_getName(scriptCode), 1);
		add_assoc_string(zv, "script_short_name", (char *)uscript_getShortName(scriptCode), 1);
#endif
	}

	return zv;
}
/* }}} unichar_make_info */
#endif /* WITH_UNICHAR_ICU */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
