/*
 * This file is part of Newtown.
 *
 * Copyright (C) 2013 Borko Jandras <bjandras@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef UTIL_H_
#define UTIL_H_

#include <sys/time.h>
#include <iostream>
#include <cassert>


class Stopwatch
{
public:
	static int const numInstances = 10;
	static Stopwatch instance[numInstances];

public:
	Stopwatch()
	{
		reset();
	}

	void reset()
	{
		m_total = 0;
		m_count = 0;
	}

	void begin()
	{
		gettimeofday(&m_t1, NULL);
	}

	void end(unsigned int max = 1000, char const * msg = "")
	{
		struct timeval t2;
		gettimeofday(&t2, NULL);

		m_total += (t2.tv_sec - m_t1.tv_sec)*1000000 + (t2.tv_usec - m_t1.tv_usec);

		if (++m_count >= max)
		{
			print(msg);
			reset();
		}
	}

	void print(char const * msg)
	{
		assert(m_count > 0);
		std::cout << msg << ": " << m_total/m_count << std::endl;
	}

private:
	unsigned int m_total;
	unsigned int m_count;
	struct timeval m_t1;
};


#endif // ifndef UTIL_H_
