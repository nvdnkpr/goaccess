/**
 * util.c -- a set of handy functions to help parsing
 * Copyright (C) 2009-2013 by Gerardo Orellana <goaccess@prosoftcorp.com>
 * GoAccess - An Ncurses apache weblog analyzer & interactive viewer
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * A copy of the GNU General Public License is attached to this
 * source distribution for its full text.
 *
 * Visit http://goaccess.prosoftcorp.com for new releases.
 */

#define _LARGEFILE_SOURCE
#define _LARGEFILE64_SOURCE
#define _FILE_OFFSET_BITS 64

#define _XOPEN_SOURCE     700

#include <arpa/inet.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

#include "util.h"
#include "error.h"
#include "xmalloc.h"

/* {"search string", "belongs to"} */
static char *os[][2] = {
   {"Windows NT 6.2", "Windows"},
   {"Windows NT 6.1", "Windows"},
   {"Windows NT 6.0", "Windows"},
   {"Windows NT 5.2", "Windows"},
   {"Windows NT 5.1", "Windows"},
   {"Windows NT 5.01", "Windows"},
   {"Windows NT 5.0", "Windows"},
   {"Windows NT 4.0", "Windows"},
   {"Windows 98;", "Windows"},
   {"Windows 98", "Windows"},
   {"Windows 95", "Windows"},
   {"Windows CE", "Windows"},
   {"Android", "Android"},
   {"Debian", "Linux"},
   {"Ubuntu", "Linux"},
   {"Fedora", "Linux"},
   {"Mint", "Linux"},
   {"SUSE", "Linux"},
   {"Mandriva", "Linux"},
   {"Red Hat", "Linux"},
   {"Gentoo", "Linux"},
   {"CentOS", "Linux"},
   {"PCLinuxOS", "Linux"},
   {"Linux", "Linux"},
   {"iPad", "Macintosh"},
   {"iPod", "Macintosh"},
   {"iPhone", "Macintosh"},
   {"iTunes", "Macintosh"},
   {"Mac OS X", "Macintosh"},
   {"Mac OS", "Macintosh"},
   {"FreeBSD", "BSD"},
   {"NetBSD", "BSD"},
   {"OpenBSD", "BSD"},
   {"SunOS", "Others"},
   {"AmigaOS", "Others"},
   {"BlackBerry", "Others"},
   {"Symbian OS", "Others"},
   {"Xbox", "Others"},
   {"Nokia", "Others"},
   {"PlayStation", "Others"}
};

/* {"search string", "belongs to"} */
static char *browsers[][2] = {
   /* browsers & offline browsers */
   {"Avant Browser", "Others"},
   {"America Online Browser", "Others"},
   {"MSIE", "MSIE"},
   {"Flock", "Others"},
   {"Epiphany", "Others"},
   {"SeaMonkey", "Others"},
   {"Iceweasel", "Others"},
   {"Minefield", "Others"},
   {"GranParadiso", "Others"},
   {"YaBrowser", "Others"},
   {"Firefox", "Firefox"},
   {"Opera Mini", "Opera"},
   {"Opera", "Opera"},
   {"OPR", "Opera"},
   {"Netscape", "Others"},
   {"Konqueror", "Others"},
   {"Wget", "Others"},
   {"w3m", "Others"},
   {"ELinks", "Others"},
   {"Links", "Others"},
   {"Lynx", "Others"},
   {"Camino", "Others"},
   {"Dillo", "Others"},
   {"Kazehakase", "Others"},
   {"Iceape", "Others"},
   {"K-Meleon", "Others"},
   {"Galeon", "Others"},
   {"BrowserX", "Others"},
   {"IBrowse", "Others"},
   {"Mosaic", "Others"},
   {"midori", "Others"},
   {"Midori", "Others"},
   {"Firebird", "Others"},
   {"Chrome", "Chrome"},
   {"Safari", "Safari"},

   {"W3C_Validator", "Crawlers"},
   {"W3C_CSS_Validator", "Crawlers"},
   {"facebook", "Crawlers"},
   {"msnbot-media", "Crawlers"},
   {"msnbot", "Crawlers"},
   {"bingbot", "Crawlers"},
   {"ia_archiver", "Crawlers"},
   {"Mediapartners-Google", "Crawlers"},
   {"Googlebot-Image", "Crawlers"},
   {"Googlebot", "Crawlers"},
   {"slurp", "Crawlers"},
   {"Baiduspider", "Crawlers"},
   {"FlipboardProxy", "Crawlers"},
   {"YandexBot", "Crawlers"},
   {"YandexAntivirus", "Crawlers"},
   {"YandexDirect", "Crawlers"},
   {"YandexBlogs", "Crawlers"},
   {"YandexMetrika", "Crawlers"},
   {"YandexNewslinks", "Crawlers"},
   {"Sosospider", "Crawlers"},
   {"Sosoimagespider", "Crawlers"},
   {"Feedfetcher-Google", "Crawlers"},
   {"AdsBot-Google", "Crawlers"},
   {"AdsBot-Google-Mobile", "Crawlers"},
   {"AhrefsBot", "Crawlers"},
   {"Speedy Spider", "Crawlers"},
   {"AcoonBot", "Crawlers"},
   {"Java", "Crawlers"},
   {"Gigabot", "Crawlers"},
   {"Twiceler", "Crawlers"},
   {"YoudaoBot", "Crawlers"},
   {"Turnitin", "Crawlers"},
   {"Ask Jeeves", "Crawlers"},
   {"Exabot", "Crawlers"},
   {"archive.org_bot", "Crawlers"},
   {"Google-Sitemaps", "Crawlers"},
   {"Butterfly", "Crawlers"},
   {"CareerBot", "Crawlers"},
   {"CloudServerMarketSpider", "Crawlers"},
   {"CompSpyBot", "Crawlers"},
   {"Crowsnest", "Crawlers"},
   {"EasouSpider", "Crawlers"},
   {"findlinks", "Crawlers"},
   {"meanpathbot", "Crawlers"},
   {"oBot", "Crawlers"},
   {"PagesInventory", "Crawlers"},
   {"SemrushBot", "Crawlers"},
   {"SeznamBot", "Crawlers"},
   {"TweetmemeBot", "Crawlers"},
   {"UnwindFetchor", "Crawlers"},
   {"PostRank", "Crawlers"},
   {"KaloogaBot", "Crawlers"},
   {"Twitter", "Crawlers"},
   {"yacy", "Crawlers"},
   {"Netcraft", "Crawlers"},
   {"Nutch", "Crawlers"},
   {"ichiro", "Crawlers"},
   {"Sogou", "Crawlers"},
   {"wikiwix", "Crawlers"},
   {"KaloogaBot", "Crawlers"},
   {"Mozilla", "Others"}
};

static char *code_type[][2] = {
   {"1", "1xx Informational"},
   {"2", "2xx Success"},
   {"3", "3xx Redirection"},
   {"4", "4xx Client Error"},
   {"5", "5xx Server Error"},
};

static char *codes[][2] = {
   {"100", "100 - Continue: Server has received the request headers"},
   {"101", "101 - Switching Protocols: Client asked to switch protocols"},
   {"200", "200 - OK: The request sent by the client was successful"},
   {"201", "201 - Created: The request has been fulfilled and created"},
   {"202", "202 - Accepted: The request has been accepted for processing"},
   {"203", "203 - Non-Authoritative Information"},
   {"204", "204 - No Content: Request is not returning any content"},
   {"205", "205 - Reset Content: User-Agent should reset the document"},
   {"206", "206 - Partial Content: The partial GET has been successful"},
   {"300", "300 - Multiple Choices: Multiple options for the resource"},
   {"301", "301 - Moved Permanently: Resource has permanently moved"},
   {"302", "302 - Moved Temporarily (redirect)"},
   {"303", "303 - See Other Document: The response is at a different URI"},
   {"304", "304 - Not Modified: Resource has not been modified"},
   {"305", "305 - Use Proxy: Can only be accessed through the proxy"},
   {"307", "307 - Temporary Redirect: Resource temporarily moved"},
   {"400", "400 - Bad Request: The syntax of the request is invalid"},
   {"401", "401 - Unauthorized: Request needs user authentication"},
   {"402", "402 - Payment Required"},
   {"403", "403 - Forbidden: Server is refusing to respond to it"},
   {"404", "404 - Document Not Found: Requested resource could not be found"},
   {"405", "405 - Method Not Allowed: Request method not supported"},
   {"406", "406 - Not Acceptable"},
   {"407", "407 - Proxy Authentication Required"},
   {"408",
    "408 - Request Timeout: The server timed out waiting for the request"},
   {"409", "409 - Conflict: Conflict in the request"},
   {"410", "410 - Gone: Resource requested is no longer available"},
   {"411", "411 - Length Required: Invalid Content-Length"},
   {"412", "412 - Precondition Failed: Server does not meet preconditions"},
   {"413", "413 - Requested Entity Too Long"},
   {"414", "414 - Requested Filename Too Long"},
   {"415", "415 - Unsupported Media Type: Media type is not supported"},
   {"416", "416 - Requested Range Not Satisfiable: Cannot supply that portion"},
   {"417", "417 - Expectation Failed"},
   {"500", "500 - Internal Server Error"},
   {"501", "501 - Not Implemented"},
   {"502", "502 - Bad Gateway: Received an invalid response from the upstream"},
   {"503", "503 - Service Unavailable: The server is currently unavailable"},
   {"504", "504 - Gateway Timeout: The upstream server failed to send request"},
   {"505", "505 - HTTP Version Not Supported"}
};

/* helper functions */
char *
substring (const char *str, int begin, int len)
{
   char *buffer;
   if (str == NULL)
      return NULL;
   if (begin < 0)
      begin = strlen (str) + begin;
   if (begin < 0)
      begin = 0;
   if (len < 0)
      len = 0;
   if (((size_t) begin) > strlen (str))
      begin = strlen (str);
   if (((size_t) len) > strlen (&str[begin]))
      len = strlen (&str[begin]);
   if ((buffer = xmalloc (len + 1)) == NULL)
      return NULL;
   memcpy (buffer, &(str[begin]), len);
   buffer[len] = '\0';

   return buffer;
}

char *
alloc_string (const char *str)
{
   char *new = xmalloc (strlen (str) + 1);
   strcpy (new, str);
   return new;
}

int
count_occurrences (const char *s1, char c)
{
   const char *ptr = s1;
   int n = 0;
   do {
      if (*ptr == c)
         n++;
   } while (*(ptr++));
   return n;
}

char *
convert_date (char *result, char *data, char *from, char *to, int size)
{
   struct tm tm;

   memset (&tm, 0, sizeof (tm));

   char *end = strptime (data, from, &tm);
   if (end == NULL || *end != '\0')
      return NULL;
   if (strftime (result, size, to, &tm) <= 0)
      return NULL;

   return result;
}

int
invalid_ipaddr (char *str)
{
   if (str == NULL || *str == '\0')
      return 1;

   union
   {
      struct sockaddr addr;
      struct sockaddr_in6 addr6;
      struct sockaddr_in addr4;
   } a;
   memset (&a, 0, sizeof (a));
   if (1 == inet_pton (AF_INET, str, &a.addr4.sin_addr))
      return 0;
   else if (1 == inet_pton (AF_INET6, str, &a.addr6.sin6_addr))
      return 0;
   return 1;
}

/* off_t becomes 64 bit aware */
off_t
file_size (const char *filename)
{
   struct stat st;

   if (stat (filename, &st) == 0)
      return st.st_size;

   fprintf (stderr, "Cannot determine size of %s: %s\n", filename,
            strerror (errno));

   return -1;
}

char *
verify_os (const char *str, GOpeSysStr o_type)
{
   char *a, *b, *lookfor, *p;
   int space = 0;
   size_t i;

   if (str == NULL || *str == '\0')
      return (NULL);

   for (i = 0; i < ARRAY_SIZE (os); i++) {
      if ((a = strstr (str, os[i][0])) == NULL)
         continue;

      /* agents w/ space in between */
      if ((lookfor = "Windows", strstr (str, lookfor)) != NULL ||
          (lookfor = "iPhone OS", strstr (str, lookfor)) != NULL ||
          (lookfor = "Red Hat", strstr (str, lookfor)) != NULL ||
          (lookfor = "Win", strstr (str, lookfor)) != NULL) {

         if (o_type == OPESYS)
            return alloc_string (os[i][0]);
         else
            return alloc_string (os[i][1]);
      }

      /* parse mac os x */
      if ((lookfor = "Mac OS X", strstr (a, lookfor)) != NULL) {
         p = a;
         while (*p != ';' && *p != ')' && *p != '(' && *p != '\0') {
            if (*p == '_')
               *p = '.';
            if (*p == ' ')
               space++;
            if (space > 3)
               break;
            p++;
         }
         *p = 0;

         if (o_type == OPESYS)
            return alloc_string (a);
         else
            return alloc_string (os[i][1]);
      }

      /* parse android */
      if ((lookfor = "Android", strstr (a, lookfor)) != NULL) {
         p = a;
         while (*p != ';' && *p != ')' && *p != '(' && *p != '\0')
            p++;
         *p = 0;

         if (o_type == OPESYS)
            return alloc_string (a);
         else
            return alloc_string (os[i][1]);
      }

      if (!(b = a))
         return (NULL);

      /* all others */
      for (p = a; *p; p++) {
         if (*p != ' ' && isalnum (p[0]) && *p != '\0') {
            a++;
            continue;
         } else
            break;
      }
      *p = 0;
      if (strlen (a) == (p - b)) {
      }

      if (o_type == OPESYS)
         return alloc_string (b);
      return alloc_string (os[i][1]);
   }

   return alloc_string ("Unknown");
}

char *
verify_browser (const char *str, GBrowserStr b_type)
{
   char *a;
   char *b;
   char *p;
   char *ptr;
   char *slash;
   size_t i;

   if (str == NULL || *str == '\0')
      return (NULL);

   for (i = 0; i < ARRAY_SIZE (browsers); i++) {
      if ((a = strstr (str, browsers[i][0])) == NULL)
         continue;
      if (!(b = a))
         return (NULL);

      ptr = a;
      /* Opera has the version number at the end */
      if ((strstr (a, "Opera")) != NULL) {
         slash = strrchr (str, '/');
         if ((slash != NULL) && (a < slash))
            memmove (a + 5, slash, strlen (slash) + 1);
      }
      /* MSIE needs additional code. sigh... */
      if ((strstr (a, "MSIE")) != NULL || (strstr (a, "Ask")) != NULL) {
         while (*ptr != ';' && *ptr != ')' && *ptr != '-' && *ptr != '\0') {
            if (*ptr == ' ')
               *ptr = '/';
            ptr++;
         }
      }
      /* everything else is parsed here */
      for (p = a; *p; p++) {
         if (isalnum (p[0]) || *p == '.' || *p == '/' || *p == '_' || *p == '-') {
            a++;
            continue;
         } else
            break;
      }
      *p = 0;
      if (strlen (a) == (p - b)) {
      }

      /* Opera +15 uses OPR/# */
      if ((strstr (b, "OPR")) != NULL) {
         slash = strrchr (b, '/');
         if ((slash != NULL)) {
            char *val = xmalloc (snprintf (NULL, 0, "Opera %s", slash) + 1);
            sprintf (val, "Opera %s", slash);
            return val;
         }
      }

      if (b_type == BROWSER)
         return alloc_string (b);
      return alloc_string (browsers[i][1]);
   }

   return alloc_string ("Unknown");
}

char *
verify_status_code_type (const char *str)
{
   size_t i;
   for (i = 0; i < ARRAY_SIZE (code_type); i++)
      if (strchr (code_type[i][0], str[0]) != NULL)
         return code_type[i][1];

   return "Unknown";
}

char *
verify_status_code (char *str)
{
   size_t i;
   for (i = 0; i < ARRAY_SIZE (codes); i++)
      if (strstr (str, codes[i][0]) != NULL)
         return codes[i][1];

   return "Unknown";
}

char *
trim_str (char *str)
{
   char *p;
   if (!str)
      return NULL;
   if (!*str)
      return str;
   for (p = str + strlen (str) - 1; (p >= str) && isspace (*p); --p);
   p[1] = '\0';

   return str;
}

char *
filesize_str (unsigned long long log_size)
{
   char *size = (char *) xmalloc (sizeof (char) * 12);
   if (log_size >= GB)
      snprintf (size, 12, "%.2f GiB", (double) (log_size) / GB);
   else if (log_size >= MB)
      snprintf (size, 12, "%.2f MiB", (double) (log_size) / MB);
   else if (log_size >= KB)
      snprintf (size, 12, "%.2f KiB", (double) (log_size) / KB);
   else
      snprintf (size, 12, "%.1f  B", (double) (log_size));

   return size;
}

char *
usecs_to_str (unsigned long long usec)
{
   char *size = (char *) xmalloc (sizeof (char) * 11);
   if (usec >= HOUR)
      snprintf (size, 11, "%.2f hr", (double) (usec) / HOUR);
   else if (usec >= MINS)
      snprintf (size, 11, "%.2f mn", (double) (usec) / MINS);
   else if (usec >= SECS)
      snprintf (size, 11, "%.2f  s", (double) (usec) / SECS);
   else if (usec >= MILS)
      snprintf (size, 11, "%.2f ms", (double) (usec) / MILS);
   else
      snprintf (size, 11, "%.2f us", (double) (usec));

   return size;
}

char *
clean_date (char *s)
{
   if ((s == NULL) || (*s == '\0'))
      return NULL;

   char *buffer = xmalloc (strlen (s) + 1);
   if (sscanf (s, "%11[^|]", buffer) != 1) {
      free (buffer);
      return NULL;
   }

   return buffer;
}

char *
clean_month (char *s)
{
   if ((s == NULL) || (*s == '\0'))
      return NULL;

   char *buffer = xmalloc (strlen (s) + 1);
   if (sscanf (s, "%6[^|]", buffer) != 1) {
      free (buffer);
      return NULL;
   }

   return buffer;
}

char *
int_to_str (int d)
{
   char *s = xmalloc (snprintf (NULL, 0, "%d", d) + 1);
   sprintf (s, "%d", d);

   return s;
}

int
intlen (int num)
{
   int l = 1;
   while (num > 9) {
      l++;
      num /= 10;
   }

   return l;
}

char *
char_repeat (int n, char c)
{
   char *dest = xmalloc (n + 1);
   memset (dest, c, n);
   dest[n] = '\0';

   return dest;
}

char *
char_replace (char *str, char o, char n)
{
   if (str == NULL || *str == '\0')
      return str;

   char *p = str;
   while ((p = strchr (p, o)) != NULL)
      *p++ = n;

   return str;
}
