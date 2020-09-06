/************************************************************************/
/*                                                                      */
/* This file is part of VDrift.                                         */
/*                                                                      */
/* VDrift is free software: you can redistribute it and/or modify       */
/* it under the terms of the GNU General Public License as published by */
/* the Free Software Foundation, either version 3 of the License, or    */
/* (at your option) any later version.                                  */
/*                                                                      */
/* VDrift is distributed in the hope that it will be useful,            */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of       */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        */
/* GNU General Public License for more details.                         */
/*                                                                      */
/* You should have received a copy of the GNU General Public License    */
/* along with VDrift.  If not, see <http://www.gnu.org/licenses/>.      */
/*                                                                      */
/************************************************************************/

#ifndef _SIGNALSLOT_H
#define _SIGNALSLOT_H

#include "delegate.h"
#include <vector>

template <class Delegate>
class Slot;

template <class Delegate>
class Signal
{
public:
	Signal(void) {};
	Signal(const Signal & other);
	Signal & operator=(const Signal & other);
	void disconnect(void);
	bool connected(void) const;

protected:
	friend class Slot<Delegate>;
	~Signal(void);

	struct Connection
	{
		Slot<Delegate> * slot;
		std::size_t id;
	};
	std::vector<Connection> m_connections;
};

template <class Delegate>
class Slot
{
public:
	Slot(void) {};
	~Slot(void);
	Slot(const Slot & other);
	Slot & operator=(const Slot & other);
	void connect(Signal<Delegate> & signal);
	void disconnect(void);
	bool connected(void) const;
	Delegate call;

protected:
	friend class Signal<Delegate>;
	struct Connection
	{
		Signal<Delegate> * signal;
		std::size_t id;
	};
	std::vector<Connection> m_connections;
};

using Slot0 = Slot<Delegate0<void>>;

template <typename P>
using Slot1 = Slot<Delegate1<void, P>>;

template <typename P, typename R>
using Slot2 = Slot<Delegate2<void, P, R>>;

class Signal0 : public Signal<Delegate0<void> >
{
public:
	void operator()() const;
};

template <typename P>
class Signal1 : public Signal<Delegate1<void, P> >
{
public:
	void operator()(P p) const;
};

template <typename P, typename R>
class Signal2 : public Signal<Delegate2<void, P, R> >
{
public:
	void operator()(P p, R r) const;
};

// Implementation

template <class Delegate>
inline Slot<Delegate>::Slot(const Slot & other)
{
	*this = other;
}

template <class Delegate>
inline Slot<Delegate>::~Slot()
{
	disconnect();
}

template <class Delegate>
inline Slot<Delegate> & Slot<Delegate>::operator=(const Slot<Delegate> & other)
{
	if (this != &other)
	{
		for (auto & con : m_connections)
		{
			connect(*con.signal);
		}
		call = other.call;
	}
	return *this;
}

template <class Delegate>
inline void Slot<Delegate>::connect(Signal<Delegate> & signal)
{
	typename Signal<Delegate>::Connection sg;
	sg.slot = this;
	sg.id = m_connections.size();

	Connection sl;
	sl.signal = &signal;
	sl.id = signal.m_connections.size();

	m_connections.push_back(sl);
	signal.m_connections.push_back(sg);
}

template <class Delegate>
inline void Slot<Delegate>::disconnect(void)
{
	for (const auto & con : m_connections)
	{
		// remove slot from signal by swapping
		auto & signal = *con.signal;
		auto id = con.id;
		signal.m_connections[id] = signal.m_connections[signal.m_connections.size() - 1];

		// update id of the swapped connection
		auto & swapped_con = signal.m_connections[id];
		swapped_con.slot->m_connections[swapped_con.id].id = id;

		signal.m_connections.pop_back();
	}
	m_connections.resize(0);
}

template <class Delegate>
inline bool Slot<Delegate>::connected(void) const
{
	return m_connections.size();
}

template <class Delegate>
inline Signal<Delegate>::Signal(const Signal & other)
{
	*this = other;
}

template <class Delegate>
inline Signal<Delegate> & Signal<Delegate>::operator=(const Signal & other)
{
	if (this != &other)
	{
		for (auto & con : other.m_connections)
		{
			con.slot->connect(*this);
		}
	}
	return *this;
}

template <class Delegate>
inline Signal<Delegate>::~Signal(void)
{
	disconnect();
}

template <class Delegate>
inline void Signal<Delegate>::disconnect(void)
{
	for (const auto & con : m_connections)
	{
		// remove signal from slot by swapping
		auto & slot = *con.slot;
		auto id = con.id;
		slot.m_connections[id] = slot.m_connections[slot.m_connections.size() - 1];

		// update id of the swapped connection
		auto & swapped_con = slot.m_connections[id];
		swapped_con.signal->m_connections[swapped_con.id].id = id;

		slot.m_connections.pop_back();
	}
	m_connections.resize(0);
}

template <class Delegate>
inline bool Signal<Delegate>::connected(void) const
{
	return m_connections.size();
}

inline void Signal0::operator()() const
{
	for (const auto & con : m_connections)
	{
		con.slot->call();
	}
}

template <typename P>
inline void Signal1<P>::operator()(P p) const
{
	for (const auto & con : Signal<Delegate1<void, P> >::m_connections)
	{
		con.slot->call(p);
	}
}

template <typename P, typename R>
inline void Signal2<P, R>::operator()(P p, R r) const
{
	for (const auto & con : Signal<Delegate2<void, P, R> >::m_connections)
	{
		con.slot->call(p, r);
	}
}

#endif //_SIGNALSLOT_H
