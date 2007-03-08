/********************************************************************\
 * qofutil.c -- QOF utility functions                               *
 * Copyright (C) 1997 Robin D. Clark                                *
 * Copyright (C) 1997-2001,2004 Linas Vepstas <linas@linas.org>     *
 * Copyright 2006  Neil Williams  <linux@codehelp.co.uk>            *
 *                                                                  *
 * This program is free software; you can redistribute it and/or    *
 * modify it under the terms of the GNU General Public License as   *
 * published by the Free Software Foundation; either version 2 of   *
 * the License, or (at your option) any later version.              *
 *                                                                  *
 * This program is distributed in the hope that it will be useful,  *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of   *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the    *
 * GNU General Public License for more details.                     *
 *                                                                  *
 * You should have received a copy of the GNU General Public License*
 * along with this program; if not, contact:                        *
 *                                                                  *
 * Free Software Foundation           Voice:  +1-617-542-5942       *
 * 51 Franklin Street, Fifth Floor    Fax:    +1-617-542-2652       *
 * Boston, MA  02110-1301,  USA       gnu@gnu.org                   *
 *                                                                  *
 *   Author: Rob Clark (rclark@cs.hmc.edu)                          *
 *   Author: Linas Vepstas (linas@linas.org)                        *
\********************************************************************/

#include "config.h"

#include <ctype.h>
#include <glib.h>
#include <stdlib.h>
#include <string.h>
#include "qof.h"
#include "qofbackend-p.h"

static QofLogModule log_module = QOF_MOD_UTIL;

/* Search for str2 in first nchar chars of str1, ignore case..  Return
 * pointer to first match, or null.  */
gchar *
strncasestr(const guchar *str1, const guchar *str2, size_t len) 
{
  while (*str1 && len--) 
  {
    if (toupper(*str1) == toupper(*str2)) 
    {
      if (strncasecmp(str1,str2,strlen(str2)) == 0) 
      {
        return (gchar *) str1;
      }
    }
    str1++;
  }
  return NULL;
}

/* Search for str2 in str1, ignore case.  Return pointer to first
 * match, or null.  */
gchar *
strcasestr(const gchar *str1, const gchar *str2) 
{
   size_t len = strlen (str1);
   gchar * retval = strncasestr (str1, str2, len);
   return retval;
}

gint 
safe_strcmp (const gchar * da, const gchar * db)
{
	if ((da) && (db)) {
		if ((da) != (db)) {
			gint retval = strcmp ((da), (db));
			/* if strings differ, return */
			if (retval) return retval;
		}     
	} else
	if ((!(da)) && (db)) {
		return -1;
	} else
	if ((da) && (!(db))) {
		return +1;
	}
	return 0;
}

gint 
safe_strcasecmp (const gchar * da, const gchar * db)
{
	if ((da) && (db)) {
		if ((da) != (db)) {
			gint retval = strcasecmp ((da), (db));
			/* if strings differ, return */
			if (retval) return retval;
		}     
	} else
	if ((!(da)) && (db)) {
		return -1;
	} else
	if ((da) && (!(db))) {
		return +1;
    }
   return 0;
}

gint 
null_strcmp (const gchar * da, const gchar * db)
{
   if (da && db) return strcmp (da, db);
   if (!da && db && 0==db[0]) return 0;
   if (!db && da && 0==da[0]) return 0;
   if (!da && db) return -1;
   if (da && !db) return +1;
   return 0;
}

#define MAX_DIGITS 50

/* inverse of strtoul */
gchar *
ultostr (gulong val, gint base)
{
  gchar buf[MAX_DIGITS];
  gulong broke[MAX_DIGITS];
  gint i;
  gulong places=0, reval;
  
  if ((2>base) || (36<base)) return NULL;

  /* count digits */
  places = 0;
  for (i=0; i<MAX_DIGITS; i++) {
     broke[i] = val;
     places ++;
     val /= base;
     if (0 == val) break;
  }

  /* normalize */
  reval = 0;
  for (i=places-2; i>=0; i--) {
    reval += broke[i+1];
    reval *= base;
    broke[i] -= reval;
  }

  /* print */
  for (i=0; i<(gint)places; i++) {
    if (10>broke[i]) {
       buf[places-1-i] = 0x30+broke[i];  /* ascii digit zero */
    } else {
       buf[places-1-i] = 0x41-10+broke[i];  /* ascii capital A */
    }
  }
  buf[places] = 0x0;

  return g_strdup (buf);
}

/* =================================================================== */
/* returns TRUE if the string is a number, possibly with whitespace */
/* =================================================================== */

gboolean
gnc_strisnum(const guchar *s)
{
  if (s == NULL) return FALSE;
  if (*s == 0) return FALSE;

  while (*s && isspace(*s))
    s++;

  if (*s == 0) return FALSE;
  if (!isdigit(*s)) return FALSE;

  while (*s && isdigit(*s))
    s++;

  if (*s == 0) return TRUE;

  while (*s && isspace(*s))
    s++;

  if (*s == 0) return TRUE;

  return FALSE;
}

/* =================================================================== */
/* Return NULL if the field is whitespace (blank, tab, formfeed etc.)  
 * Else return pointer to first non-whitespace character. */
/* =================================================================== */

const gchar *
qof_util_whitespace_filter (const gchar * val)
{
	size_t len;
	if (!val) return NULL;

	len = strspn (val, "\a\b\t\n\v\f\r ");
	if (0 == val[len]) return NULL;
	return val+len;
}

/* =================================================================== */
/* Return integer 1 if the string starts with 't' or 'T' or contains the 
 * word 'true' or 'TRUE'; if string is a number, return that number. */
/* =================================================================== */

gint
qof_util_bool_to_int (const gchar * val)
{
	const gchar * p = qof_util_whitespace_filter (val);
	if (!p) return 0;
	if ('t' == p[0]) return 1;
	if ('T' == p[0]) return 1;
	if ('y' == p[0]) return 1;
	if ('Y' == p[0]) return 1;
	if (strstr (p, "true")) return 1;
	if (strstr (p, "TRUE")) return 1;
	if (strstr (p, "yes")) return 1;
	if (strstr (p, "YES")) return 1;
	return atoi (val);
}

/* =================================================================== */
/* Entity edit and commit utilities */
/* =================================================================== */


/* =================================================================== */
/* The QOF string cache */
/* =================================================================== */

static GCache * qof_string_cache = NULL;

#ifdef THESE_CAN_BE_USEFUL_FOR_DEGUGGING
static guint g_str_hash_KEY(gconstpointer v) {
    return g_str_hash(v);
}
static guint g_str_hash_VAL(gconstpointer v) {
    return g_str_hash(v);
}
static gpointer g_strdup_VAL(gpointer v) {
    return g_strdup(v);
}
static gpointer g_strdup_KEY(gpointer v) {
    return g_strdup(v);
}
static void g_free_VAL(gpointer v) {
    return g_free(v);
}
static void g_free_KEY(gpointer v) {
    return g_free(v);
}
static gboolean qof_util_str_equal(gconstpointer v, gconstpointer v2)
{
    return (v && v2) ? g_str_equal(v, v2) : FALSE;
}
#endif
#ifdef QOF_DISABLE_DEPRECATED 
static GCache*
qof_util_get_string_cache(void)
#else
GCache*
qof_util_get_string_cache(void)
#endif    
{
    if(!qof_string_cache) {
        qof_string_cache = g_cache_new(
            (GCacheNewFunc) g_strdup, /* value_new_func     */
            g_free,                   /* value_destroy_func */
            (GCacheDupFunc) g_strdup, /* key_dup_func       */
            g_free,                   /* key_destroy_func   */
            g_str_hash,               /* hash_key_func      */
            g_str_hash,               /* hash_value_func    */
            g_str_equal);             /* key_equal_func     */
    }
    return qof_string_cache;
}

void
qof_util_string_cache_destroy (void)
{
    if (qof_string_cache)
        g_cache_destroy (qof_string_cache);
    qof_string_cache = NULL;
}

void
qof_util_string_cache_remove(gconstpointer key)
{
    if (key)
    g_cache_remove(qof_util_get_string_cache(), key);
}

gpointer
qof_util_string_cache_insert(gconstpointer key)
{
    if (key)
        return g_cache_insert(qof_util_get_string_cache(), (gpointer)key);
    return NULL;
}

gchar*
qof_util_param_as_string(QofInstance *ent, QofParam *param)
{
	gchar       *param_string, param_date[MAX_DATE_LENGTH];
	gchar       param_sa[GUID_ENCODING_LENGTH + 1];
    gboolean    known_type;
	QofType     paramType;
	const GUID *param_guid;
	time_t      param_t;
	gnc_numeric param_numeric,  (*numeric_getter) (QofInstance*, QofParam*);
	Timespec    param_ts,       (*date_getter)    (QofInstance*, QofParam*);
	double      param_double,   (*double_getter)  (QofInstance*, QofParam*);
	gboolean    param_boolean,  (*boolean_getter) (QofInstance*, QofParam*);
	gint32      param_i32,      (*int32_getter)   (QofInstance*, QofParam*);
	gint64      param_i64,      (*int64_getter)   (QofInstance*, QofParam*);
	gchar       param_char,     (*char_getter)    (QofInstance*, QofParam*);

	param_string = NULL;
    known_type = FALSE;
	paramType = param->param_type;
	if(paramType == QOF_TYPE_STRING)  { 
			param_string = g_strdup(param->param_getfcn(ent, param));
			if(param_string == NULL) { param_string = ""; }
            known_type = TRUE;
			return param_string;
		}
		if(paramType == QOF_TYPE_DATE) { 
			date_getter = (Timespec (*)(QofInstance*, QofParam*))param->param_getfcn;
			param_ts = date_getter(ent, param);
			param_t = timespecToTime_t(param_ts);
			strftime(param_date, MAX_DATE_LENGTH, 
                QOF_UTC_DATE_FORMAT, gmtime(&param_t));
			param_string = g_strdup(param_date);
            known_type = TRUE;
			return param_string;
		}
		if((paramType == QOF_TYPE_NUMERIC)  ||
		(paramType == QOF_TYPE_DEBCRED)) { 
			numeric_getter = (gnc_numeric (*)(QofInstance*, QofParam*)) param->param_getfcn;
			param_numeric = numeric_getter(ent, param);
			param_string = g_strdup(gnc_numeric_to_string(param_numeric));
            known_type = TRUE;
			return param_string;
		}
		if(paramType == QOF_TYPE_GUID) { 
			param_guid = param->param_getfcn(ent, param);
			guid_to_string_buff(param_guid, param_sa);
			param_string = g_strdup(param_sa);
            known_type = TRUE;
			return param_string;
		}
		if(paramType == QOF_TYPE_INT32) { 
			int32_getter = (gint32 (*)(QofInstance*, QofParam*)) param->param_getfcn;
			param_i32 = int32_getter(ent, param);
			param_string = g_strdup_printf("%d", param_i32);
            known_type = TRUE;
			return param_string;
		}
		if(paramType == QOF_TYPE_INT64) { 
			int64_getter = (gint64 (*)(QofInstance*, QofParam*)) param->param_getfcn;
			param_i64 = int64_getter(ent, param);
			param_string = g_strdup_printf("%"G_GINT64_FORMAT, param_i64);
            known_type = TRUE;
			return param_string;
		}
		if(paramType == QOF_TYPE_DOUBLE) { 
			double_getter = (double (*)(QofInstance*, QofParam*)) param->param_getfcn;
			param_double = double_getter(ent, param);
			param_string = g_strdup_printf("%f", param_double);
            known_type = TRUE;
			return param_string;
		}
		if(paramType == QOF_TYPE_BOOLEAN){ 
			boolean_getter = (gboolean (*)(QofInstance*, QofParam*)) param->param_getfcn;
			param_boolean = boolean_getter(ent, param);
			/* Boolean values need to be lowercase for QSF validation. */
			if(param_boolean == TRUE) { param_string = g_strdup("true"); }
			else { param_string = g_strdup("false"); }
            known_type = TRUE;
			return param_string;
		}
		/* "kvp" contains repeating values, cannot be a single string for the frame. */
		if(paramType == QOF_TYPE_KVP) {
            KvpFrame *frame = NULL;
            frame = param->param_getfcn(ent, param);
            known_type = TRUE;
            if(!kvp_frame_is_empty(frame)) 
            {
                GHashTable *hash = kvp_frame_get_hash(frame);
                param_string = g_strdup_printf("%s(%d)", g_type_name (QOF_TYPE_KVP),
                    g_hash_table_size(hash));
            }
            return param_string; 
        }
		if(paramType == QOF_TYPE_CHAR) { 
			char_getter = (gchar (*)(QofInstance*, QofParam*)) param->param_getfcn;
			param_char = char_getter(ent, param);
            known_type = TRUE;
			return g_strdup_printf("%c", param_char);
		}
		/* "collect" contains repeating values, cannot be a single string. */
        if(paramType == QOF_TYPE_COLLECT)
        {
            QofCollection *col = NULL;
            col = param->param_getfcn(ent, param);
            known_type = TRUE;
            return g_strdup_printf("%s(%d)", 
                g_type_name (qof_collection_get_g_type(col)), qof_collection_count(col));
        }
        if(paramType == QOF_TYPE_CHOICE)
        {
            QofInstance *child = NULL;
            child = param->param_getfcn(ent, param);
            if(!child) { return param_string; }
            known_type = TRUE;
            return g_strdup(qof_instance_to_string (child));
        }
        if(paramType == QOF_PARAM_BOOK)
        {
            QofBackend *be;
            QofBook *book;
            book = param->param_getfcn(ent, param);
            PINFO (" book param %p", book);
            be = qof_book_get_backend(book);
            known_type = TRUE;
            PINFO (" backend=%p", be);
            if(!be) 
            { 
                return "book"; /* FIXME: could be used g_type_name (QOF_TYPE_BOOK) */ 
            }
            param_string = g_strdup(be->fullpath);
            PINFO (" fullpath=%s", param_string);
            if(param_string) { return param_string; }
			param_guid = qof_instance_get_guid(QOF_INSTANCE (book));
			guid_to_string_buff(param_guid, param_sa);
            PINFO (" book GUID=%s", param_sa);
			param_string = g_strdup(param_sa);
            return param_string;
        }
        if(!known_type)
        {
            QofInstance *child = NULL;
            child = param->param_getfcn(ent, param);
            if(!child) { return param_string; }
            return g_strdup(qof_instance_to_string (child));
        }
	return g_strdup("");
}

void
qof_init (void)
{
	qof_util_get_string_cache ();
	guid_init ();
	qof_object_initialize ();
	qof_query_init ();
	/*qof_book_register (); FIXME: This is broken for now but not needed... I think..*/
}

void
qof_close(void)
{
	qof_query_shutdown ();
	qof_object_shutdown ();
	guid_shutdown ();
	qof_util_string_cache_destroy ();
}

/* ************************ END OF FILE ***************************** */
