/* ui-shared.c: common web output functions
 *
 * Copyright (C) 2006 Lars Hjemli
 *
 * Licensed under GNU General Public License v2
 *   (see COPYING for full license text)
 */

#include "cgit.h"

const char cgit_doctype[] =
"<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\"\n"
"  \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n";

static char *http_date(time_t t)
{
	static char day[][4] = 
		{"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
	static char month[][4] = 
		{"Jan", "Feb", "Mar", "Apr", "May", "Jun",
		 "Jul", "Aug", "Sep", "Oct", "Now", "Dec"};
	struct tm *tm = gmtime(&t);
	return fmt("%s, %02d %s %04d %02d:%02d:%02d GMT", day[tm->tm_wday],
		   tm->tm_mday, month[tm->tm_mon], 1900+tm->tm_year,
		   tm->tm_hour, tm->tm_min, tm->tm_sec);
}

static long ttl_seconds(long ttl)
{
	if (ttl<0)
		return 60 * 60 * 24 * 365;
	else
		return ttl * 60;
}

void cgit_print_error(char *msg)
{
	html("<div class='error'>");
	html_txt(msg);
	html("</div>\n");
}

char *cgit_repourl(const char *reponame)
{
	if (cgit_virtual_root) {
		return fmt("%s/%s/", cgit_virtual_root, reponame);
	} else {
		return fmt("?r=%s", reponame);
	}
}

char *cgit_pageurl(const char *reponame, const char *pagename, 
		   const char *query)
{
	if (cgit_virtual_root) {
		return fmt("%s/%s/%s/?%s", cgit_virtual_root, reponame, 
			   pagename, query);
	} else {
		return fmt("?r=%s&p=%s&%s", reponame, pagename, query);
	}
}

char *cgit_currurl()
{
	if (!cgit_virtual_root)
		return "./cgit.cgi";
	else if (cgit_query_page)
		return fmt("%s/%s/%s/", cgit_virtual_root, cgit_query_repo, cgit_query_page);
	else if (cgit_query_repo)
		return fmt("%s/%s/", cgit_virtual_root, cgit_query_repo);
	else
		return fmt("%s/", cgit_virtual_root);
}


void cgit_print_date(unsigned long secs)
{
	char buf[32];
	struct tm *time;

	time = gmtime(&secs);
	strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", time);
	html_txt(buf);
	
}

void cgit_print_docstart(char *title, struct cacheitem *item)
{
	html("Content-Type: text/html; charset=utf-8\n");
	htmlf("Last-Modified: %s\n", http_date(item->st.st_mtime));
	htmlf("Expires: %s\n", http_date(item->st.st_mtime +
					 ttl_seconds(item->ttl)));
	html("\n");
	html(cgit_doctype);
	html("<html>\n");
	html("<head>\n");
	html("<title>");
	html_txt(title);
	html("</title>\n");
	htmlf("<meta name='generator' content='cgit v%s'/>\n", cgit_version);
	html("<link rel='stylesheet' type='text/css' href='");
	html_attr(cgit_css);
	html("'/>\n");
	html("</head>\n");
	html("<body>\n");
}

void cgit_print_docend()
{
	html("</td></tr></table>");
	html("</body>\n</html>\n");
}

void cgit_print_pageheader(char *title, int show_search)
{
	html("<table id='layout'><tr><td id='header'>");
	htmlf("<a href='%s'>", cgit_logo_link);
	htmlf("<img id='logo' src='%s'/>\n", cgit_logo);
	htmlf("</a>");
	if (show_search) {
		html("<form method='get' href='");
		html_attr(cgit_currurl());
		html("'>");
		if (!cgit_virtual_root) {
			if (cgit_query_repo)
				html_hidden("r", cgit_query_repo);
			if (cgit_query_page)
				html_hidden("p", cgit_query_page);
		}
		if (cgit_query_head)
			html_hidden("h", cgit_query_head);
		if (cgit_query_sha1)
			html_hidden("id", cgit_query_sha1);
		if (cgit_query_sha2)
			html_hidden("id2", cgit_query_sha2);
		html("<input type='text' name='q' value='");
		html_attr(cgit_query_search);
		html("'/></form>");
	}
	if (cgit_query_repo)
		htmlf("<a href='%s'>", cgit_repourl(cgit_query_repo));
	html_txt(title);
	if (cgit_query_repo)
		html("</a>");
	html("</td></tr><tr><td id='content'>");
}

void cgit_print_snapshot_start(const char *mimetype, const char *filename, 
			       struct cacheitem *item)
{
	htmlf("Content-Type: %s\n", mimetype);
	htmlf("Content-Disposition: inline; filename=\"%s\"\n", filename);
	htmlf("Last-Modified: %s\n", http_date(item->st.st_mtime));
	htmlf("Expires: %s\n", http_date(item->st.st_mtime +
					 ttl_seconds(item->ttl)));
	html("\n");
}
