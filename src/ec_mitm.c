/*
    ettercap -- mitm management module

    Copyright (C) ALoR & NaGA

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

    $Id: ec_mitm.c,v 1.6 2003/11/01 15:52:58 alor Exp $
*/

#include <ec.h>
#include <ec_mitm.h>

/* globals */


static SLIST_HEAD (, mitm_entry) mitm_table;

struct mitm_entry {
   int selected;
   int started;
   struct mitm_method *mm;
   SLIST_ENTRY (mitm_entry) next;
};

static char *mitm_args = "";

/* protos */

void mitm_add(struct mitm_method *mm);
int mitm_set(u_char *name);
void mitm_start(void);
void mitm_stop(void);

/*******************************************/

/*
 * register a new mitm method in the table 
 */
void mitm_add(struct mitm_method *mm)
{
   struct mitm_entry *e;

   SAFE_CALLOC(e, 1, sizeof(struct mitm_entry));

   /* copy the mm struct */
   SAFE_CALLOC(e->mm, 1, sizeof(struct mitm_method));
   
   memcpy(e->mm, mm, sizeof(struct mitm_method));
   
   SLIST_INSERT_HEAD(&mitm_table, e, next);
   
}


/*
 * set the 'selected' flag in the table
 * used by ec_parse.c
 */
int mitm_set(u_char *name)
{
   struct mitm_entry *e;

   if ((mitm_args = strchr(name, ':')) != NULL) {
      *mitm_args = '\0';
      mitm_args ++;
   } else {
      mitm_args = "";
   }
   
   DEBUG_MSG("mitm_set: %s (%s)", name, mitm_args);
   
   /* search the name and set it */
   SLIST_FOREACH(e, &mitm_table, next) {
      if (!strcasecmp(e->mm->name, name)) {
         e->selected = 1;
         return ESUCCESS;
      }
   }

   return -ENOTFOUND;
}


/* 
 * starts all the method with the selected flag set.
 * it is possible to start multiple method simultaneusly
 */
void mitm_start(void)
{
   struct mitm_entry *e;

   /* reading from file we won't start mitm */
   if (GBL_OPTIONS->read || GBL_OPTIONS->unoffensive) {
      DEBUG_MSG("mitm_start: skipping");
      return;
   }
   
   DEBUG_MSG("mitm_start");
   
   /* start all the selected methods */
   SLIST_FOREACH(e, &mitm_table, next) {
      if (e->selected) {
         DEBUG_MSG("mitm_start: starting %s", e->mm->name);
         e->mm->start(mitm_args);
         e->started = 1;
      }
   }
}


/*
 * stop all the previously started method
 */
void mitm_stop(void)
{
   struct mitm_entry *e;

   DEBUG_MSG("mitm_stop");
   
   /* stop all the started methods */
   SLIST_FOREACH(e, &mitm_table, next) {
      if (e->started) {
         DEBUG_MSG("mitm_stop: stopping %s", e->mm->name);
         e->mm->stop();
      }
   }
   
}

/* EOF */

// vim:ts=3:expandtab

