/*
 * uhub - A tiny ADC p2p connection hub
 * Copyright (C) 2007-2009, Jan Vidar Krey
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along wtimeout_evtith this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "uhub.h"
#include "util/timeout.h"

void timeout_evt_initialize(struct timeout_evt* t, timeout_evt_cb cb, void* ptr)
{
	t->callback = cb;
	t->ptr = ptr;
	t->prev = 0;
	t->next = 0;
}

void timeout_evt_reset(struct timeout_evt* t)
{
	t->prev = 0;
	t->next = 0;
}

int timeout_evt_is_scheduled(struct timeout_evt* t)
{
	if (!t) return 0;
	return !!t->prev;
}

void timeout_queue_initialize(struct timeout_queue* t, time_t now, size_t max)
{
	t->last = now;
	t->max = max;
	t->events = hub_malloc_zero(max * sizeof(struct timeout_evt*));
}

void timeout_queue_shutdown(struct timeout_queue* t)
{
	hub_free(t->events);
	t->events = 0;
	t->max = 0;
}

size_t timeout_queue_process(struct timeout_queue* t, time_t now)
{
	size_t pos;
	size_t events = 0;
	struct timeout_evt* evt = 0;
	for (pos = t->last; pos < now; pos++)
	{
		while ((evt = t->events[pos % t->max]))
		{
			timeout_queue_remove(t, evt);
			timeout_evt_reset(evt);
			evt->callback(evt);
			events++;
		}
	}
	t->last = now;
	return events;
}

void timeout_queue_insert(struct timeout_queue* t, struct timeout_evt* evt, size_t seconds)
{
	struct timeout_evt* first;
	size_t pos = ((t->last + seconds) % t->max);
	evt->timestamp = t->last + seconds;
	evt->next = 0;
	
	first = t->events[pos];
	
	if (!first)
	{
		t->events[pos] = evt;
		evt->prev = evt;
	}
	else
	{
		evt->prev = first->prev;
		first->prev = evt;
	}
}

void timeout_queue_remove(struct timeout_queue* t, struct timeout_evt* evt)
{
	size_t pos = (evt->timestamp % t->max);
	struct timeout_evt* first = t->events[pos];

	if (!first)
		return;

	if (first == evt)
	{
		if (first->next)
			first->next->prev = first->prev;
		t->events[pos] = first->next;
	}
	else
	{
		evt->prev->next = evt->next;
		if (evt->next)
				evt->next->prev = evt->prev;
		else
				first->prev = evt->prev;
	}
}

void timeout_queue_reschedule(struct timeout_queue* t, struct timeout_evt* evt, size_t seconds)
{
	if (timeout_evt_is_scheduled(evt))
		timeout_queue_remove(t, evt);
	timeout_queue_insert(t, evt, seconds);
}